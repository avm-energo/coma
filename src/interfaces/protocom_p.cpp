#include "protocom_p.h"

#include "../gen/datamanager.h"
#include "../gen/datatypes.h"
#include "../gen/stdfunc.h"
#include "protocomprivate.h"
ProtocomPrivate::ProtocomPrivate()
{
}

void ProtocomPrivate::handleBlk(const Proto::Commands cmd, const quint32 blk, QByteArray data, const quint32 count)
{
    Proto::CommandStruct inp {
        cmd,   // Command
        blk,   // Block number or empty for some cmds or regAddr
        count, // Null arg or regCount
        data   // QByteArray data, maybe empty
    };
    DataManager::addToInQueue(inp);
}

void ProtocomPrivate::handleBlk(const Proto::Commands cmd, const quint32 addr, const quint32 count)
{
    Q_Q(Protocom);
    if (!q->isValidRegs(addr, count))
        return;
    auto blockNumber = blockByReg(addr);
    handleBlk(cmd, blockNumber);
}

void ProtocomPrivate::handleBlk(const Proto::Commands cmd, const DataTypes::Signal &signal)
{
    quint8 block = blockByReg(signal.addr);
    handleBlk(cmd, signal.addr, StdFunc::arrayFromNumber(quint8(block)), signal.value);
}

void ProtocomPrivate::handleInt(const Proto::Commands cmd, const QByteArray data)
{
    Proto::CommandStruct inp {
        cmd,        // Command
        QVariant(), // Block number or empty for some cmds or regAddr
        QVariant(), // Null arg or regCount
        data        // QByteArray data, maybe empty
    };
    DataManager::addToInQueue(inp);
}

void ProtocomPrivate::handleCommand(const Proto::Commands cmd)
{
    Proto::CommandStruct inp { cmd, QVariant(), QVariant(), QByteArray {} };
    DataManager::addToInQueue(inp);
}

void ProtocomPrivate::handleCommand(const Proto::WCommands cmd)
{
    Proto::CommandStruct inp { Proto::WriteBlkCmd, cmd, QVariant(), QByteArray {} };
    DataManager::addToInQueue(inp);
}

void ProtocomPrivate::handleCommand(const Proto::Commands cmd, const DataTypes::SingleCommand singleCmd)
{
    Proto::CommandStruct inp { cmd, QVariant::fromValue(singleCmd.addr), QVariant(),
        StdFunc::arrayFromNumber(singleCmd.value) };
    DataManager::addToInQueue(inp);
}
