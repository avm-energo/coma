#include "protocom.h"

#include "../gen/datamanager.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "protocom_p.h"
#include "protocomthread.h"
#include "settingstypes.h"
#include "usbhidport.h"

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#ifdef Q_OS_WINDOWS
// clang-format off
#include <windows.h>
// Header dbt must be the last header, thanx to microsoft
#include <dbt.h>
// clang-format on
#endif
using Proto::CommandStruct;
using Proto::Starters;

Protocom::Protocom(QObject *parent) : BaseInterface(parent), d_ptr(new ProtocomPrivate())
{
    Q_D(Protocom);
    d->q_ptr = this;
    qRegisterMetaType<UsbHidSettings>();
}

Protocom::~Protocom()
{
    delete d_ptr;
}

bool Protocom::start(const ConnectStruct &st)
{
    // Q_ASSERT(Board::GetInstance().interfaceType() == Board::InterfaceType::USB);
    Q_ASSERT(std::holds_alternative<UsbHidSettings>(st.settings));

    if (!std::holds_alternative<UsbHidSettings>(st.settings))
        return false;
    const auto settings = std::get<UsbHidSettings>(st.settings);
    return start(settings);
}

bool Protocom::start(const UsbHidSettings &usbhid)
{
    UsbHidPort *port = new UsbHidPort(usbhid, Log);

    ProtocomThread *parser = new ProtocomThread;

    QThread *portThread = new QThread;
    QThread *parseThread = new QThread;

    // NOTE После остановки потоков мы всё еще обращаемся
    // к интерфейсу для обновления данных
    QList<QMetaObject::Connection> connections;
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
        for (const auto &var : connections)
        {
            QObject::disconnect(var);
        }
    });
    connect(port, &UsbHidPort::finished, portThread, &QThread::quit);
    connect(port, &UsbHidPort::finished, parseThread, &QThread::quit);
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
#ifdef Q_OS_WINDOWS
    connect(
        this, &BaseInterface::nativeEvent, port,
        [port](auto &&msg) {
            MSG *message = static_cast<MSG *>(msg);
            if (!msg)
                return;
            auto *devint = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE *>(message->lParam);
            if (!devint)
                return;
            USBMessage usbMessage;
            usbMessage.guid = QString::fromStdWString(&devint->dbcc_name[0]);
            usbMessage.type = devint->dbcc_devicetype;

            QMetaObject::invokeMethod(
                port, [=] { port->usbEvent(usbMessage, message->wParam); }, Qt::QueuedConnection);
        },
        Qt::DirectConnection);
#endif
    connect(port, &UsbHidPort::stateChanged, this, &BaseInterface::stateChanged, Qt::QueuedConnection);
    qInfo() << metaObject()->className() << "connected";
    port->moveToThread(portThread);
    parser->moveToThread(parseThread);
    setState(Run);
    portThread->start();
    parseThread->start();
    return true;
}

void Protocom::reqTime()
{
    CommandStruct inp { Proto::Commands::ReadTime, QVariant(), QVariant(), {} };
    DataManager::addToInQueue(inp);
    emit wakeUpParser();
}

void Protocom::reqFile(quint32 filenum, FileFormat format)
{
    QByteArray ba = StdFunc::arrayFromNumber(quint16(filenum));

    CommandStruct inp {
        Proto::Commands::ReadFile, // Command
        filenum,                   // File number
        format,                    // Is file should be restored
        ba                         // Empty QByteArray
    };
    DataManager::addToInQueue(inp);
    emit wakeUpParser();
}

void Protocom::reqStartup(quint32 sigAdr, quint32 sigCount)
{
    Q_D(Protocom);
    if (!isValidRegs(sigAdr, sigCount))
        return;
    CommandStruct inp {
        Proto::Commands::FakeReadRegData,               // Fake command
        sigAdr,                                         // Signal addr
        sigCount,                                       // Count signals
        StdFunc::arrayFromNumber(d->blockByReg(sigAdr)) // Protocom block
    };
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

void Protocom::reqBSIExt()
{
    CommandStruct inp {
        Proto::Commands::ReadBlkStartInfoExt, // Command
        QVariant(),                           // Board type(Null because only 1 board contains bsi)
        QVariant(),                           // Null arg
        {}                                    // Null
    };
    DataManager::addToInQueue(inp);
    emit wakeUpParser();
}

void Protocom::reqBitStrings(quint32 sigAdr, quint32 sigCount)
{
    Q_D(Protocom);
    if (!isValidRegs(sigAdr, sigCount))
        return;
    CommandStruct inp {
        Proto::Commands::FakeReadBitString,             // Fake command
        sigAdr,                                         // Signal addr
        sigCount,                                       // Count signals
        StdFunc::arrayFromNumber(d->blockByReg(sigAdr)) // Protocom block
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

void Protocom::writeTime(quint32 time)
{
    CommandStruct inp { Proto::Commands::WriteTime, time, QVariant(), {} };
    DataManager::addToInQueue(inp);
    emit wakeUpParser();
}
#ifdef Q_OS_LINUX
void Protocom::writeTime(const timespec &time) const
{
    CommandStruct inp { Proto::Commands::WriteTime, QVariant::fromValue(time), QVariant(), {} };
    DataManager::addToInQueue(inp);
    emit wakeUpParser();
}
#endif
void Protocom::reqFloats(quint32 sigAdr, quint32 sigCount)
{
    Q_D(Protocom);
    if (!isValidRegs(sigAdr, sigCount))
        return;
    CommandStruct inp {
        Proto::Commands::FakeReadRegData,               // Fake command
        sigAdr,                                         // Signal addr
        sigCount,                                       // Count signals
        StdFunc::arrayFromNumber(d->blockByReg(sigAdr)) // Protocom block
    };
    DataManager::addToInQueue(inp);
    emit wakeUpParser();
}

void Protocom::writeRaw(const QByteArray &ba)
{
    CommandStruct inp { Proto::Commands::RawCommand, QVariant(), QVariant(), ba };
    DataManager::addToInQueue(inp);
    emit wakeUpParser();
}

InterfaceSettings Protocom::parseSettings(QDomElement domElement) const
{
    return BaseInterface::parseSettings<Proto::ProtocomGroup>(domElement);
}

void Protocom::writeCommand(Queries::Commands cmd, QVariant item)
{
    Q_D(Protocom);
    using namespace Proto;
    using DataTypes::Signal;

    auto protoCmd = getProtoCommand.value(cmd);

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
        {
            const auto signal = item.value<Signal>();
            if (isValidRegs(signal.addr, signal.value))
                d->handleBlk(protoCmd, item.value<Signal>());
        }
        break;

    case Commands::FakeReadAlarms:

        Q_ASSERT(item.canConvert<Signal>());
        if (item.canConvert<Signal>())
        {
            const auto signal = item.value<Signal>();
            if (isValidRegs(signal.addr, signal.value))
                d->handleBlk(protoCmd, item.value<Signal>());
        }
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

    case Commands::WriteSingleCommand:

        Q_ASSERT(item.canConvert<DataTypes::SingleCommand>());
        d->handleCommand(protoCmd, item.value<DataTypes::SingleCommand>());
        break;

    case Commands::WriteHardware:

        Q_ASSERT(item.canConvert<DataTypes::HardwareStruct>());
        d->handleBlk(protoCmd, item.value<DataTypes::BlockStruct>());
        break;

    case Commands::WriteBlkData:

        Q_ASSERT(item.canConvert<DataTypes::BlockStruct>());
        d->handleBlk(protoCmd, item.value<DataTypes::BlockStruct>());
        break;

    case Commands::WriteMode:

        Q_ASSERT(item.canConvert<quint8>());
        d->handleInt(protoCmd, StdFunc::arrayFromNumber(item.value<quint8>()));
        break;

    case Commands::WriteVariant:

        Q_ASSERT(item.canConvert<quint8>());
        d->handleInt(protoCmd, StdFunc::arrayFromNumber(item.value<quint8>()));
        break;

    case Commands::EraseTech:

        Q_ASSERT(item.canConvert<quint8>());
        d->handleInt(protoCmd, StdFunc::arrayFromNumber(item.value<quint8>()));
        break;

    // default case as case for Proto WCommand
    default:
    {
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
        else
            d->handleCommand(protoCmd);
    }
    }
    emit wakeUpParser();
}

void Protocom::writeCommand(Queries::Commands cmd, const QVariantList &list)
{
    Q_D(Protocom);
    using namespace Proto;
    switch (cmd)
    {
    case Queries::QC_WriteUserValues:
    {
        Q_ASSERT(list.first().canConvert<DataTypes::FloatStruct>());
        const quint16 start_addr = list.first().value<DataTypes::FloatStruct>().sigAdr;
        Q_ASSERT(isValidRegs(start_addr, list.size()));
        const auto blockNum = d->blockByReg(start_addr);
        DataTypes::BlockStruct block { blockNum, {} };
        for (const auto &i : list)
        {
            const float value = i.value<DataTypes::FloatStruct>().sigVal;
            block.data.append(StdFunc::arrayFromNumber(value));
        }
        writeCommand(cmd, QVariant::fromValue(block));
        break;
    }
    default:
        Q_ASSERT(false && "Not realized");
        break;
    }
}

bool Protocom::isValidRegs(const quint32 sigAdr, const quint32 sigCount)
{
    const auto &st = settings<InterfaceInfo<Proto::ProtocomGroup>>();
    Q_ASSERT(st.dictionary().contains(sigAdr));
    const auto val = st.dictionary().value(sigAdr);
    Q_ASSERT(val.count == sigCount);
    return val.count == sigCount;
}
