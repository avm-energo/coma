#pragma once
#include "protocom.h"
class ProtocomPrivate
{
    Q_DECLARE_PUBLIC(Protocom);

public:
    ProtocomPrivate();
    void handleBlk(const Proto::Commands cmd, const quint32 blk, QByteArray data = {}, const quint32 count = 0);
    void handleBlk(const Proto::Commands cmd, const quint32 addr, const quint32 count)
    {
        Q_Q(Protocom);
        if (!q->isValidRegs(addr, count))
            return;
        auto blockNumber = q->settings<InterfaceInfo<Proto::ProtocomGroup>>().dictionary().value(addr).block;
        handleBlk(cmd, blockNumber);
    }

    void handleBlk(const Proto::Commands cmd, const DataTypes::Signal &signal)
    {
        handleBlk(cmd, signal.addr, QByteArray(), signal.value);
    }
    void handleBlk(const Proto::Commands cmd, const DataTypes::ConfParameterStruct &str)
    {
        handleBlk(cmd, str.ID, str.data);
    }
    void handleBlk(const Proto::Commands cmd, const DataTypes::BlockStruct &str)
    {
        handleBlk(cmd, str.ID, str.data);
    }
    void handleInt(const Proto::Commands cmd, const QByteArray data);
    void handleCommand(const Proto::Commands cmd);
    void handleCommand(const Proto::WCommands cmd);
    Protocom *q_ptr;
};
