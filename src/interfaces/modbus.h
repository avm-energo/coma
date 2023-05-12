#pragma once

#include "baseinterface.h"
#include "modbusprivate.h"
#include "settingstypes.h"

#include <QMutex>
#include <QTimer>
#include <array>
#include <gen/logclass.h>

using namespace Interface;

class ModBus final : public BaseInterface
{
    Q_OBJECT

public:
    ModBus(QObject *parent = nullptr);
    ~ModBus();

    bool start(const ConnectStruct &connectStruct) override;
    void pause() override {};
    void resume() override {};

signals:
    void clearBuffer();

private:
    bool isValidRegs(MBS::CommandStruct &cmd) const;
    bool isValidRegs(const quint32 sigAdr, const quint32 sigCount) const;
    MBS::TypeId type(const quint32 addr) const;
    MBS::TypeId type(const quint32 addr, const quint32 count, const MBS::Commands cmd) const;
    void writeFloat(const DataTypes::FloatStruct &flstr);
    void writeInt16(const quint32 addr, const quint16 value);
    const quint8 obtainDelay(const quint32 baudRate) const;
    CommandRegisters getAddrByCommand(Queries::Commands cmd);

private slots:
    void sendReconnectSignal();
};
