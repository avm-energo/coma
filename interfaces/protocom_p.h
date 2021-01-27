#pragma once
#include "protocom.h"
class ProtocomPrivate
{
    Q_DECLARE_PUBLIC(Protocom);

public:
    ProtocomPrivate();
    auto blockByReg(const quint32 regAddr)
    {
        Q_Q(Protocom);
        return q->settings<InterfaceInfo<Proto::ProtocomGroup>>().dictionary().value(regAddr).block;
    }
    void handleBlk(const Proto::Commands cmd, const quint32 blk, QByteArray data = {}, const quint32 count = 0);
    void handleBlk(const Proto::Commands cmd, const quint32 addr, const quint32 count);

    void handleBlk(const Proto::Commands cmd, const DataTypes::Signal &signal);
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
