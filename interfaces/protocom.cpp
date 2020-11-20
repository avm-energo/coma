#include "protocom.h"

#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../gen/stdfunc.h"
#include "protocomthread.h"
#include "settingstypes.h"
#include "usbhidport.h"

#include <QDebug>
#include <QThread>
using Proto::CommandStruct;
using Proto::Direction;
using Proto::Starters;
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

    connect(
        port, &UsbHidPort::dataReceived, this,
        [parser](const QByteArray &ba) {
            parser->appendReadDataChunk(ba);
            parser->wakeUp();
        },
        Qt::DirectConnection);
    qDebug() << QThread::currentThreadId();
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
    // QByteArray ba;
    // ba.setNum(filenum);
    QByteArray ba = StdFunc::arrayFromNumber(quint16(filenum));

    CommandStruct inp {
        Proto::Commands::ReadFile, // Command
        filenum,                   // File number
        isConfigFile,              // Is file should be restored
        ba                         // Empty QByteArray
    };
    DataManager::addToInQueue(inp);
}

void Protocom::reqStartup(quint32 sigAdr, quint32 sigCount)
{
    Q_ASSERT(Proto::getBlkByReg.contains(sigAdr));
    Q_ASSERT(Proto::getBlkByReg.value(sigAdr).second == sigCount);
    CommandStruct inp { Proto::Commands::ReadBlkData, sigAdr, sigCount, {} };
    DataManager::addToInQueue(inp);
}

void Protocom::reqBSI()
{
    CommandStruct inp {
        Proto::Commands::ReadBlkStartInfo, // Command
        QVariant(),                        // Board type
        QVariant(),                        // Null arg
        {}                                 // QByteArray(sizeof(ModuleBSI::Bsi), Qt::Uninitialized) // Buffer for bsi
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
    Q_ASSERT(Proto::getBlkByReg.contains(sigAdr));
    Q_ASSERT(Proto::getBlkByReg.value(sigAdr).second == sigCount);
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

void handleCommand(const Proto::WCommands cmd)
{
    CommandStruct inp { Proto::WriteBlkCmd, cmd, QVariant(), QByteArray {} };
    DataManager::addToInQueue(inp);
}

void Protocom::writeCommand(Queries::Commands cmd, QVariant item)
{
    using namespace Proto;
    using DataTypes::Signal;

    auto protoCmd = getProtoCommand.value(cmd);
    // Q_ASSERT(protoCmd != ReadBlkData && "This command couldn't be used directly");
    if (!protoCmd)
    {
        auto wCmd = getWCommand.value(cmd);
        handleCommand(wCmd);
    }
    switch (protoCmd)
    {
    case Commands::ReadBlkData:

        //  NOTE Эта команда выполняется иначе
        Q_ASSERT(item.canConvert<Signal>());
        handleBlk(protoCmd, item.value<Signal>());
        break;

    case Commands::ReadBlkAC:

        Q_ASSERT(item.canConvert<quint32>());
        handleBlk(protoCmd, item.toUInt());
        break;

    case Commands::ReadBlkDataA:

        Q_ASSERT(item.canConvert<quint32>());
        handleBlk(protoCmd, item.toUInt());
        break;

    case Commands::ReadBlkTech:

        Q_ASSERT(item.canConvert<quint32>());
        handleBlk(protoCmd, item.toUInt());
        break;

    case Commands::WriteBlkAC:

        Q_ASSERT(item.canConvert<DataTypes::BlockStruct>());
        handleBlk(protoCmd, item.value<DataTypes::BlockStruct>());
        break;

    case Commands::WriteBlkTech:

        Q_ASSERT(item.canConvert<DataTypes::BlockStruct>());
        handleBlk(protoCmd, item.value<DataTypes::BlockStruct>());
        break;

    case Commands::WriteBlkCmd:

        Q_ASSERT(item.canConvert<quint32>());
        handleBlk(protoCmd, item.toUInt());
        break;

    case Commands::WriteMode:

        Q_ASSERT(item.canConvert<quint32>());
        handleInt(protoCmd, QByteArray::number(item.toUInt()));
        break;

    case Commands::WriteVariant:

        Q_ASSERT(item.canConvert<quint32>());
        handleInt(protoCmd, QByteArray::number(item.toUInt()));
        break;

    default:
    {
        handleCommand(protoCmd);
    }
    }
}
