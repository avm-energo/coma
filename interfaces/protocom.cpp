#include "protocom.h"

#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "protocom_p.h"
#include "protocomthread.h"
#include "settingstypes.h"
#include "usbhidport.h"

#include <QDebug>
#include <QThread>

using Proto::CommandStruct;
using Proto::Starters;

Protocom::Protocom(QObject *parent) : BaseInterface(parent), d_ptr(new ProtocomPrivate())
{
    Q_D(Protocom);
    d->q_ptr = this;
    qRegisterMetaType<UsbHidSettings>();
}

bool Protocom::start(const ConnectStruct &st)
{
    Q_ASSERT(Board::GetInstance().interfaceType() == Board::InterfaceType::USB);
    Q_ASSERT(std::holds_alternative<UsbHidSettings>(st.settings));

    if (!std::holds_alternative<UsbHidSettings>(st.settings))
        return false;
    const auto settings = std::get<UsbHidSettings>(st.settings);
    return start(settings);
}

bool Protocom::start(const UsbHidSettings &usbhid)
{
    UsbHidPort *port = new UsbHidPort(usbhid, Log);
#ifdef QT_GUI_LIB
    port->connectToGui(this);
#endif
    ProtocomThread *parser = new ProtocomThread;

    QThread *portThread = new QThread;
    QThread *parseThread = new QThread;

    // NOTE После остановки потоков мы всё еще обращаемся
    // к интерфейсу для обновления данных
    QList<QMetaObject::Connection> connections;
    // FIXME Разобраться с удалением и закрытием потоков
    // Старт
    connect(portThread, &QThread::started, port, &UsbHidPort::poll);
    connect(parseThread, &QThread::started, parser, &ProtocomThread::parse);
    // Рабочий режим
    connections << connect(this, &Protocom::wakeUpParser, parser, &ProtocomThread::wakeUp, Qt::DirectConnection);
    connect(port, &UsbHidPort::dataReceived, parser, &ProtocomThread::appendReadDataChunk, Qt::DirectConnection);
    connect(parser, &ProtocomThread::writeDataAttempt, port, &UsbHidPort::writeDataAttempt, Qt::DirectConnection);
    // Прерывание
    connect(port, &UsbHidPort::clearQueries, parser, &ProtocomThread::clear, Qt::DirectConnection);
    // Остановка
    connect(port, &UsbHidPort::finished, parser, &ProtocomThread::wakeUp, Qt::DirectConnection);
    connect(port, &UsbHidPort::finished, [=] {
        for (auto var : connections)
        {
            QObject::disconnect(var);
        }
    });
    connect(port, &UsbHidPort::finished, portThread, &QThread::quit);
    connect(port, &UsbHidPort::finished, parseThread, &QThread::quit);
    // connect(port, &UsbHidPort::finished, portThread, &QObject::deleteLater);
    // connect(port, &UsbHidPort::finished, parseThread, &QObject::deleteLater);
    //  connect(port, &UsbHidPort::finished, port, &QObject::deleteLater);
    //  connect(port, &UsbHidPort::finished, parser, &QObject::deleteLater);
    connect(portThread, &QThread::finished, port, &QObject::deleteLater);
    connect(parseThread, &QThread::finished, parser, &QObject::deleteLater);
    connect(portThread, &QThread::finished, &QObject::deleteLater);
    connect(parseThread, &QThread::finished, &QObject::deleteLater);

    if (!port->setupConnection())
    {
        port->deleteLater();
        parser->deleteLater();
        parseThread->deleteLater();
        portThread->deleteLater();
        return false;
    }

    Board::GetInstance().setConnectionState(Board::ConnectionState::Connected);

    qInfo() << metaObject()->className() << "connected";
    port->moveToThread(portThread);
    parser->moveToThread(parseThread);
    portThread->start();
    parseThread->start();
    return true;
}

void Protocom::stop()
{
    // emit finish();
    // FIXME Реализовать
}

void Protocom::reqTime()
{
    CommandStruct inp { Proto::Commands::ReadTime, QVariant(), QVariant(), {} };
    DataManager::addToInQueue(inp);
    emit wakeUpParser();
}

void Protocom::reqFile(quint32 filenum, bool isConfigFile)
{
    QByteArray ba = StdFunc::arrayFromNumber(quint16(filenum));

    CommandStruct inp {
        Proto::Commands::ReadFile, // Command
        filenum,                   // File number
        isConfigFile,              // Is file should be restored
        ba                         // Empty QByteArray
    };
    DataManager::addToInQueue(inp);
    emit wakeUpParser();
}

void Protocom::reqStartup(quint32 sigAdr, quint32 sigCount)
{
    if (!isValidRegs(sigAdr, sigCount))
        return;
    CommandStruct inp { Proto::Commands::FakeReadRegData, sigAdr, sigCount, {} };
    DataManager::addToInQueue(inp);
    emit wakeUpParser();
}

void Protocom::reqBSI()
{
    CommandStruct inp {
        Proto::Commands::ReadBlkStartInfo, // Command
        QVariant(),                        // Board type(Null because only 1 board contains bsi)
        QVariant(),                        // Null arg
        {}                                 // Null
    };
    DataManager::addToInQueue(inp);
    emit wakeUpParser();
}

void Protocom::writeFile(quint32 filenum, const QByteArray &file)
{
    Q_UNUSED(filenum);
    CommandStruct inp {
        Proto::Commands::WriteFile,                    // Command
        QVariant(), /*DataTypes::FilesEnum(filenum),*/ // File number
        QVariant(),                                    // Null arg
        file                                           // Buffer with file
    };
    DataManager::addToInQueue(inp);
    emit wakeUpParser();
}

// void Protocom::writeConfigFile()
//{
//    QByteArray ba;
//    S2::StoreDataMem(&ba.data()[0], S2::config, Files::Config);
//    writeFile(Files::Config, ba);
//}

void Protocom::writeTime(quint32 time)
{
    CommandStruct inp { Proto::Commands::WriteTime, time, QVariant(), {} };
    DataManager::addToInQueue(inp);
    emit wakeUpParser();
}

void Protocom::reqFloats(quint32 sigAdr, quint32 sigCount)
{
    if (!isValidRegs(sigAdr, sigCount))
        return;
    CommandStruct inp { Proto::Commands::FakeReadRegData, sigAdr, sigCount, {} };
    DataManager::addToInQueue(inp);
    emit wakeUpParser();
}

void Protocom::writeRaw(const QByteArray &ba)
{
    CommandStruct inp { Proto::Commands::RawCommand, QVariant(), QVariant(), ba };
    DataManager::addToInQueue(inp);
    emit wakeUpParser();
}

void Protocom::writeCommand(Queries::Commands cmd, QVariant item)
{
    Q_D(Protocom);
    using namespace Proto;
    using DataTypes::Signal;

    auto protoCmd = getProtoCommand.value(cmd);
    // Q_ASSERT(protoCmd != ReadBlkData && "This command couldn't be used directly");
    if (!protoCmd)
    {
        auto wCmd = getWCommand.value(cmd);
        if (!wCmd)
        {
            qCritical() << Error::WrongCommandError;
            return;
        }
        d->handleCommand(wCmd);
    }
    switch (protoCmd)
    {
    case Commands::ReadBlkData:

        Q_ASSERT(item.canConvert<quint32>());
        if (item.canConvert<quint32>())
            d->handleBlk(protoCmd, item.toUInt());
        break;

    case Commands::FakeReadRegData:

        Q_ASSERT(item.canConvert<Signal>());
        if (item.canConvert<Signal>())
            d->handleBlk(protoCmd, item.value<Signal>());
        break;

    case Commands::ReadBlkAC:

        Q_ASSERT(item.canConvert<quint32>());
        d->handleBlk(protoCmd, item.toUInt());
        break;

    case Commands::ReadBlkDataA:

        Q_ASSERT(item.canConvert<quint32>());
        d->handleBlk(protoCmd, item.toUInt());
        break;

    case Commands::ReadBlkTech:

        Q_ASSERT(item.canConvert<quint32>());
        d->handleBlk(protoCmd, item.toUInt());
        break;

    case Commands::WriteBlkAC:

        Q_ASSERT(item.canConvert<DataTypes::BlockStruct>());
        d->handleBlk(protoCmd, item.value<DataTypes::BlockStruct>());
        break;

    case Commands::WriteBlkTech:

        Q_ASSERT(item.canConvert<DataTypes::BlockStruct>());
        d->handleBlk(protoCmd, item.value<DataTypes::BlockStruct>());
        break;

    case Commands::WriteBlkCmd:

        Q_ASSERT(item.canConvert<quint32>());
        d->handleBlk(protoCmd, item.toUInt());
        break;

    case Commands::WriteBlkData:

        Q_ASSERT(item.canConvert<DataTypes::BlockStruct>());
        d->handleBlk(protoCmd, item.value<DataTypes::BlockStruct>());
        break;

    case Commands::WriteMode:

        Q_ASSERT(item.canConvert<quint8>());
        d->handleInt(protoCmd, StdFunc::arrayFromNumber(quint8(item.value<quint8>())));
        break;

    case Commands::WriteVariant:

        Q_ASSERT(item.canConvert<quint8>());
        d->handleInt(protoCmd, StdFunc::arrayFromNumber(quint8(item.value<quint8>())));
        break;

    default:
    {
        d->handleCommand(protoCmd);
    }
    }
    emit wakeUpParser();
}

bool Protocom::isValidRegs(const quint32 sigAdr, const quint32 sigCount)
{
    const auto &st = settings<InterfaceInfo<Proto::ProtocomGroup>>();
    Q_ASSERT(st.dictionary().contains(sigAdr));
    const auto val = st.dictionary().value(sigAdr);
    Q_ASSERT(val.count == sigCount);
    return val.count == sigCount;
}
