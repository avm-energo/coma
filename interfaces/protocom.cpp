#include "protocom.h"

#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../gen/modulebsi.h"
#include "protocomthread.h"
#include "settingstypes.h"
#include "usbhidport.h"

#include <QThread>

// clang-format off
const QMap<Queries::Commands, CN::Commands> Protocom::m_dict
{
    { Queries::Commands::QC_StartFirmwareUpgrade, CN::Commands::WriteUpgrade },
    { Queries::QC_SetStartupValues, CN::Commands::InitStartupValues },
    { Queries::QC_ClearStartupValues, CN::Commands::EraseStartupValues },
    { Queries::QC_SetNewConfiguration, CN::Commands::WriteBlkTech },
    { Queries::QC_EraseJournals, CN::Commands::EraseTech },
    { Queries::QC_ReqBitStrings, CN::Commands::ReadProgress },
    { Queries::QC_EraseTechBlock, CN::Commands::EraseTech },
    { Queries::QC_Test, CN::Commands::Test },
    { Queries::QUSB_ReqTuningCoef, CN::Commands::ReadBlkAC },
    { Queries::QUSB_WriteTuningCoef, CN::Commands::WriteBlkAC },
    { Queries::QUSB_ReqBlkDataA, CN::Commands::ReadBlkDataA },
    { Queries::QUSB_ReqBlkDataTech, CN::Commands::ReadBlkTech },
    { Queries::QUSB_WriteBlkDataTech, CN::Commands::WriteBlkTech }
};
// clang-format on
Protocom::Protocom(QObject *parent) : BaseInterface(parent)
{
}

bool Protocom::start(const ConnectStruct &st)
{
    Q_ASSERT(Board::GetInstance().interfaceType() == Board::InterfaceType::USB);
    Q_ASSERT(std::holds_alternative<UsbHidSettings>(st.settings));
    INFOMSG("Modbus: connect");
    if (!std::holds_alternative<UsbHidSettings>(st.settings))
        return false;

    UsbHidPort *port = new UsbHidPort(std::get<UsbHidSettings>(st.settings), Log, this);
    ProtocomThread *parser = new ProtocomThread(this);

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

void Protocom::reqTime()
{
    CommandStruct inp { CN::Commands::ReadTime, 0, 0, {} };
    DataManager::addToInQueue(inp);
}

void Protocom::reqFile(quint32 filenum, bool isConfigFile)
{
    // TODO Как использовать флаг?
    CommandStruct inp {
        CN::Commands::ReadFile, // Command
        filenum,                // File number
        isConfigFile,           // Is file should be restored
        {}                      // Empty QByteArray
    };
    DataManager::addToInQueue(inp);
}

void Protocom::reqStartup(quint32 sigAdr, quint32 sigCount)
{
    Q_ASSERT(!getBlkByReg.contains(sigAdr));
    auto blkPair = getBlkByReg.value(sigAdr);
    Q_ASSERT(blkPair.second != sigCount);
    quint32 blk = blkPair.first;
    CommandStruct inp {
        CN::Commands::ReadBlkData,                                 // Command
        blk,                                                       // Block number
        QVariant(),                                                // Null arg
        QByteArray(sizeof(sigCount) * sigCount, Qt::Uninitialized) // Buffer
    };
    DataManager::addToInQueue(inp);
}

void Protocom::reqBSI()
{
    CommandStruct inp {
        CN::Commands::ReadBlkStartInfo,                       // Command
        BoardTypes::BT_NONE,                                  // Board type
        QVariant(),                                           // Null arg
        QByteArray(sizeof(ModuleBSI::Bsi), Qt::Uninitialized) // Buffer for bsi
    };
    DataManager::addToInQueue(inp);
}

void Protocom::writeFile(quint32 filenum, const QByteArray &file)
{
    CommandStruct inp {
        CN::Commands::WriteFile, // Command
        filenum,                 // File number
        QVariant(),              // Null arg
        file                     // Buffer with file
    };
    DataManager::addToInQueue(inp);
}

void Protocom::writeTime(quint32 time)
{
    CommandStruct inp { CN::Commands::WriteTime, time, QVariant(), {} };
    DataManager::addToInQueue(inp);
}

void Protocom::reqFloats(quint32 sigAdr, quint32 sigCount)
{
    CommandStruct inp { CN::Commands::ReadBlkData, sigAdr, sigCount, {} };
    DataManager::addToInQueue(inp);
}

void handleBlk(const CN::Commands cmd, const quint32 blk, QByteArray data = {})
{
    CommandStruct inp {
        cmd,        // Command
        blk,        // Block number, cmd number for WriteCommand, or empty for some cmds
        QVariant(), // Null arg
        data        // QByteArray data, maybe empty
    };
    DataManager::addToInQueue(inp);
}

inline void handleBlk(CN::Commands cmd, quint32 addr, quint32 count)
{
    Q_ASSERT(!getBlkByReg.contains(addr));
    auto blkPair = getBlkByReg.value(addr);
    Q_ASSERT(blkPair.second != count);
    handleBlk(cmd, blkPair.first);
}

inline void handleInt(CN::Commands cmd, QByteArray data)
{
    handleBlk(cmd, NULL, data);
}

inline void handleBlk(const CN::Commands cmd, const DataTypes::Signal &signal)
{
    handleBlk(cmd, signal.addr, signal.value);
}

inline void handleBlk(const CN::Commands cmd, const DataTypes::ConfParameterStruct &str)
{
    handleBlk(cmd, str.ID, str.data);
}

void handleCommand(const CN::Commands cmd)
{
    CommandStruct inp { cmd, QVariant(), QVariant(), QByteArray {} };
    DataManager::addToInQueue(inp);
}

void Protocom::writeCommand(Queries::Commands cmd, QVariant item)
{
    using namespace CN;
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
