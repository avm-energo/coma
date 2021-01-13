#include "protocom_p.h"

#include "../gen/datamanager.h"
#include "protocomprivate.h"
ProtocomPrivate::ProtocomPrivate()
{
    Q_Q(Protocom);
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
