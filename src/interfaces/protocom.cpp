#include "protocom.h"

#include "protocom_p.h"
#include "protocomthread.h"
#include "settingstypes.h"
#include "usbhidport.h"

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <gen/files.h>
#include <gen/stdfunc.h>

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
    auto port = new UsbHidPort(usbhid, Log.get());
    auto parser = new ProtocomThread;
    auto portThread = new QThread;
    auto parseThread = new QThread;

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
            auto message = static_cast<MSG *>(msg);
            if (!msg)
                return;
            auto devint = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE *>(message->lParam);
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
    DataManager::GetInstance().addToInQueue(inp);
    emit wakeUpParser();
}

void Protocom::reqFile(quint32 filenum, FileFormat format)
{
    QByteArray ba = StdFunc::ArrayFromNumber(quint16(filenum));

    CommandStruct inp {
        Proto::Commands::ReadFile, // Command
        filenum,                   // File number
        format,                    // Is file should be restored
        ba                         // Empty QByteArray
    };
    DataManager::GetInstance().addToInQueue(inp);
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
        StdFunc::ArrayFromNumber(d->blockByReg(sigAdr)) // Protocom block
    };
    DataManager::GetInstance().addToInQueue(inp);
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
    DataManager::GetInstance().addToInQueue(inp);
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
    DataManager::GetInstance().addToInQueue(inp);
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
        StdFunc::ArrayFromNumber(d->blockByReg(sigAdr)) // Protocom block
    };
    DataManager::GetInstance().addToInQueue(inp);
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
    DataManager::GetInstance().addToInQueue(inp);
    emit wakeUpParser();
}

void Protocom::writeTime(quint32 time)
{
    CommandStruct inp { Proto::Commands::WriteTime, time, QVariant(), {} };
    DataManager::GetInstance().addToInQueue(inp);
    emit wakeUpParser();
}

#ifdef Q_OS_LINUX
void Protocom::writeTime(const timespec &time) const
{
    CommandStruct inp { Proto::Commands::WriteTime, QVariant::fromValue(time), QVariant(), {} };
    DataManager::GetInstance().addToInQueue(inp);
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
        StdFunc::ArrayFromNumber(d->blockByReg(sigAdr)) // Protocom block
    };
    DataManager::GetInstance().addToInQueue(inp);
    emit wakeUpParser();
}

void Protocom::writeRaw(const QByteArray &ba)
{
    CommandStruct inp { Proto::Commands::RawCommand, QVariant(), QVariant(), ba };
    DataManager::GetInstance().addToInQueue(inp);
    emit wakeUpParser();
}

void Protocom::writeCommand(Queries::Commands cmd, QVariant item)
{
    Q_D(Protocom);
    using namespace Proto;
    using DataTypes::Signal;

    auto protoCmd = getProtoCommand(cmd);

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
        d->handleInt(protoCmd, StdFunc::ArrayFromNumber(item.value<quint8>()));
        break;

    case Commands::WriteVariant:
        Q_ASSERT(item.canConvert<quint8>());
        d->handleInt(protoCmd, StdFunc::ArrayFromNumber(item.value<quint8>()));
        break;

    case Commands::EraseTech:
        Q_ASSERT(item.canConvert<quint8>());
        d->handleInt(protoCmd, StdFunc::ArrayFromNumber(item.value<quint8>()));
        break;

    // default case as case for Proto WCommand
    default:
    {
        if (!protoCmd)
        {
            auto wsCmd = getWSCommand(cmd);
            if (!wsCmd)
            {
                qCritical() << Error::WrongCommandError;
                return;
            }
            d->handleCommand(wsCmd, item.value<bool>());
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
        if (!isValidRegs(start_addr, list.size()))
        {
            qCritical() << Error::UnknownBlock;
            return;
        }
        const auto blockNum = d->blockByReg(start_addr);
        DataTypes::BlockStruct block { blockNum, {} };
        for (const auto &i : list)
        {
            const float value = i.value<DataTypes::FloatStruct>().sigVal;
            block.data.append(StdFunc::ArrayFromNumber(value));
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
    const auto &st = settings<InterfaceInfo<ProtocomGroup>>();
    if (!st.dictionary().contains(sigAdr))
        return false;
    const auto val = st.dictionary().value(sigAdr);
    return val.count == sigCount;
}

Proto::Commands Protocom::getProtoCommand(Queries::Commands cmd)
{
    static const QMap<Queries::Commands, Proto::Commands> protoCommandMap {
        { Queries::QC_StartFirmwareUpgrade, Proto::WriteUpgrade },     //
        { Queries::QC_SetNewConfiguration, Proto::WriteBlkTech },      //
        { Queries::QC_WriteUserValues, Proto::WriteBlkData },          //
        { Queries::QC_EraseJournals, Proto::EraseTech },               //
        { Queries::QC_ReqBitStrings, Proto::ReadProgress },            //
        { Queries::QC_EraseTechBlock, Proto::EraseTech },              //
        { Queries::QC_Test, Proto::Test },                             //
        { Queries::QC_WriteSingleCommand, Proto::WriteSingleCommand }, //
        { Queries::QUSB_ReqTuningCoef, Proto::ReadBlkAC },             //
        { Queries::QUSB_WriteTuningCoef, Proto::WriteBlkAC },          //
        { Queries::QUSB_ReqBlkData, Proto::ReadBlkData },              //
        { Queries::QUSB_ReqBlkDataA, Proto::ReadBlkDataA },            //
        { Queries::QUSB_ReqBlkDataTech, Proto::ReadBlkTech },          //
        { Queries::QUSB_ReqOscInfo, Proto::ReadBlkTech },              //
        { Queries::QUSB_WriteBlkDataTech, Proto::WriteBlkTech },       //
        { Queries::QUSB_Reboot, Proto::WriteBlkCmd },                  //
        { Queries::QC_ReqAlarms, Proto::FakeReadAlarms },              //
        { Queries::QUSB_GetMode, Proto::ReadMode },                    //
        { Queries::QUSB_SetMode, Proto::WriteMode },                   //
        { Queries::QUSB_WriteHardware, Proto::WriteHardware }          //
    };
    return protoCommandMap.value(cmd);
}

Proto::WCommands Protocom::getWCommand(Queries::Commands cmd)
{
    static const QMap<Queries::Commands, Proto::WCommands> protoWCommandMap {
        { Queries::QC_SetStartupValues, Proto::InitStartupValues },    //
        { Queries::QC_ClearStartupValues, Proto::EraseStartupValues }, //
    };
    return protoWCommandMap.value(cmd);
}

Proto::WSCommands Protocom::getWSCommand(Queries::Commands cmd)
{
    static const QMap<Queries::Commands, Proto::WSCommands> protoWSCommandMap {
        { Queries::QC_SetStartupValues, Proto::InitStartupAll },             //
        { Queries::QC_SetStartupPhaseA, Proto::InitStartupPhaseA },          //
        { Queries::QC_SetStartupPhaseB, Proto::InitStartupPhaseB },          //
        { Queries::QC_SetStartupPhaseC, Proto::InitStartupPhaseC },          //
        { Queries::QC_SetStartupUnbounced, Proto::InitStartupUnbounced },    //
        { Queries::QC_SetTransOff, Proto::SetTransOff },                     //
        { Queries::QC_ClearStartupValues, Proto::EraseStartupAll },          //
        { Queries::QC_ClearStartupError, Proto::EraseStartupInitError },     //
        { Queries::QC_ClearStartupUnbounced, Proto::ClearStartupUnbounced }, //
    };
    return protoWSCommandMap.value(cmd);
}
