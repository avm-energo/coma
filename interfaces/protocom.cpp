#include "protocom.h"

#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../gen/modulebsi.h"
#include "QDebug"
#include "protocomthread.h"
#include "settingstypes.h"
#include "usbhidport.h"

#include <QThread>
using Proto::CommandStruct;
using Proto::Direction;
using Proto::Starters;
// clang-format off
const QMap<Queries::Commands, Proto::Commands> Protocom::m_dict
{
    { Queries::Commands::QC_StartFirmwareUpgrade, Proto::Commands::WriteUpgrade },
    { Queries::QC_SetStartupValues, Proto::Commands::InitStartupValues },
    { Queries::QC_ClearStartupValues, Proto::Commands::EraseStartupValues },
    { Queries::QC_SetNewConfiguration, Proto::Commands::WriteBlkTech },
    { Queries::QC_EraseJournals, Proto::Commands::EraseTech },
    { Queries::QC_ReqBitStrings, Proto::Commands::ReadProgress },
    { Queries::QC_EraseTechBlock, Proto::Commands::EraseTech },
    { Queries::QC_Test, Proto::Commands::Test },
    { Queries::QUSB_ReqTuningCoef, Proto::Commands::ReadBlkAC },
    { Queries::QUSB_WriteTuningCoef, Proto::Commands::WriteBlkAC },
    { Queries::QUSB_ReqBlkDataA, Proto::Commands::ReadBlkDataA },
    { Queries::QUSB_ReqBlkDataTech, Proto::Commands::ReadBlkTech },
    { Queries::QUSB_WriteBlkDataTech, Proto::Commands::WriteBlkTech }
};
// clang-format on
Protocom::Protocom(QObject *parent) : BaseInterface(parent)
{
}

bool Protocom::start(const ConnectStruct &st)
{
    Q_ASSERT(Board::GetInstance().interfaceType() == Board::InterfaceType::USB);
    Q_ASSERT(std::holds_alternative<UsbHidSettings>(st.settings));
    qInfo() << metaObject()->className() << "connected";
    if (!std::holds_alternative<UsbHidSettings>(st.settings))
        return false;
    const auto settings = std::get<UsbHidSettings>(st.settings);
    UsbHidPort *port = new UsbHidPort(settings, Log);
    ProtocomThread *parser = new ProtocomThread;

    QThread *portThread = new QThread;
    QThread *parseThread = new QThread;

    port->moveToThread(portThread);
    parser->moveToThread(parseThread);

    // FIXME Разобраться с удалением и закрытием потоков
    connect(portThread, &QThread::started, port, &UsbHidPort::poll);
    connect(parseThread, &QThread::started, parser, &ProtocomThread::parse);

    connect(port, &UsbHidPort::finished, portThread, &QThread::quit);
    connect(port, &UsbHidPort::finished, portThread, &QThread::deleteLater);
    connect(port, &UsbHidPort::finished, port, &UsbHidPort::deleteLater);

    connect(port, &UsbHidPort::dataReceived, [&](const QByteArray &ba) {
        parser->appendReadDataChunk(ba);
        parseThread->start();
    });

    connect(parser, &ProtocomThread::writeDataAttempt, port, &UsbHidPort::writeDataAttempt);

    if (!port->setupConnection())
        return false;

    Board::GetInstance().setConnectionState(Board::ConnectionState::Connected);
    portThread->start();
    parseThread->start();
    return true;
}

void Protocom::stop()
{
    // FIXME Реализовать
}

void Protocom::reqTime()
{
    CommandStruct inp { Proto::Commands::ReadTime, 0, 0, {} };
    DataManager::addToInQueue(inp);
}

void Protocom::reqFile(quint32 filenum, bool isConfigFile)
{
    CommandStruct inp {
        Proto::Commands::ReadFile, // Command
        filenum,                   // File number
        isConfigFile,              // Is file should be restored
        {}                         // Empty QByteArray
    };
    DataManager::addToInQueue(inp);
}

void Protocom::reqStartup(quint32 sigAdr, quint32 sigCount)
{
    Q_ASSERT(!Proto::getBlkByReg.contains(sigAdr));
    auto blkPair = Proto::getBlkByReg.value(sigAdr);
    Q_ASSERT(blkPair.second != sigCount);
    quint32 blk = blkPair.first;
    CommandStruct inp {
        Proto::Commands::ReadBlkData,                              // Command
        blk,                                                       // Block number
        QVariant(),                                                // Null arg
        QByteArray(sizeof(sigCount) * sigCount, Qt::Uninitialized) // Buffer
    };
    DataManager::addToInQueue(inp);
}

void Protocom::reqBSI()
{
    CommandStruct inp {
        Proto::Commands::ReadBlkStartInfo,                    // Command
        BoardTypes::BT_NONE,                                  // Board type
        QVariant(),                                           // Null arg
        QByteArray(sizeof(ModuleBSI::Bsi), Qt::Uninitialized) // Buffer for bsi
    };
    DataManager::addToInQueue(inp);
}

void Protocom::writeFile(quint32 filenum, const QByteArray &file)
{
    CommandStruct inp {
        Proto::Commands::WriteFile, // Command
        filenum,                    // File number
        QVariant(),                 // Null arg
        file                        // Buffer with file
    };
    DataManager::addToInQueue(inp);
}

void Protocom::writeTime(quint32 time)
{
    CommandStruct inp { Proto::Commands::WriteTime, time, QVariant(), {} };
    DataManager::addToInQueue(inp);
}

void Protocom::reqFloats(quint32 sigAdr, quint32 sigCount)
{
    CommandStruct inp { Proto::Commands::ReadBlkData, sigAdr, sigCount, {} };
    DataManager::addToInQueue(inp);
}

void handleBlk(const Proto::Commands cmd, const quint32 blk, QByteArray data = {})
{
    CommandStruct inp {
        cmd,        // Command
        blk,        // Block number, cmd number for WriteCommand, or empty for some cmds
        QVariant(), // Null arg
        data        // QByteArray data, maybe empty
    };
    DataManager::addToInQueue(inp);
}

inline void handleBlk(Proto::Commands cmd, quint32 addr, quint32 count)
{
    Q_ASSERT(!Proto::getBlkByReg.contains(addr));
    auto blkPair = Proto::getBlkByReg.value(addr);
    Q_ASSERT(blkPair.second != count);
    handleBlk(cmd, blkPair.first);
}

inline void handleInt(Proto::Commands cmd, QByteArray data)
{
    handleBlk(cmd, NULL, data);
}

inline void handleBlk(const Proto::Commands cmd, const DataTypes::Signal &signal)
{
    handleBlk(cmd, signal.addr, signal.value);
}

inline void handleBlk(const Proto::Commands cmd, const DataTypes::ConfParameterStruct &str)
{
    handleBlk(cmd, str.ID, str.data);
}

void handleCommand(const Proto::Commands cmd)
{
    CommandStruct inp { cmd, QVariant(), QVariant(), QByteArray {} };
    DataManager::addToInQueue(inp);
}

void Protocom::writeCommand(Queries::Commands cmd, QVariant item)
{
    using namespace Proto;
    using DataTypes::Signal;
    auto ourCmd = translate(cmd);
    switch (ourCmd)
    {
        // Раздел псевдо команд
    case Commands::EraseStartupValues:

        Q_ASSERT(item.canConvert<quint32>());
        handleInt(Commands::WriteBlkCmd, QByteArray::number(item.toUInt()));
        break;
    case Commands::InitStartupValues:

        Q_ASSERT(item.canConvert<quint32>());
        handleInt(Commands::WriteBlkCmd, QByteArray::number(item.toUInt()));
        break;

    case Commands::WriteStartupValues:

        Q_ASSERT(item.canConvert<quint32>());
        handleInt(Commands::WriteBlkCmd, QByteArray::number(item.toUInt()));
        break;
        // Конец раздела псевдо команд
    case Commands::ReadBlkData:

        Q_ASSERT(item.canConvert<Signal>());
        handleBlk(ourCmd, item.value<Signal>());
        break;

    case Commands::ReadBlkAC:

        Q_ASSERT(item.canConvert<quint32>());
        handleBlk(ourCmd, item.toUInt());
        break;

    case Commands::ReadBlkDataA:

        Q_ASSERT(item.canConvert<quint32>());
        handleBlk(ourCmd, item.toUInt());
        break;

    case Commands::ReadBlkTech:

        Q_ASSERT(item.canConvert<quint32>());
        handleBlk(ourCmd, item.toUInt());
        break;

    case Commands::WriteBlkAC:

        Q_ASSERT(item.canConvert<DataTypes::ConfParameterStruct>());
        handleBlk(ourCmd, item.value<DataTypes::ConfParameterStruct>());
        break;

    case Commands::WriteBlkTech:

        Q_ASSERT(item.canConvert<DataTypes::BlkParameterStruct>());
        handleBlk(ourCmd, item.value<DataTypes::BlkParameterStruct>());
        break;

    case Commands::WriteBlkCmd:

        Q_ASSERT(item.canConvert<quint32>());
        handleBlk(ourCmd, item.toUInt());
        break;

    case Commands::WriteMode:

        Q_ASSERT(item.canConvert<quint32>());
        handleInt(ourCmd, QByteArray::number(item.toUInt()));
        break;

    case Commands::WriteVariant:

        Q_ASSERT(item.canConvert<quint32>());
        handleInt(ourCmd, QByteArray::number(item.toUInt()));
        break;

    default:
    {
        handleCommand(ourCmd);
    }
    }
}
