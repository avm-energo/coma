#pragma once

#include <QMutex>
#include <QTimer>
#include <array>
#include <gen/logclass.h>
#include <interfaces/conn/baseconnection.h>
#include <interfaces/types/modbus_types.h>
#include <interfaces/types/settingstypes.h>

using namespace Interface;

class ModBus final : public BaseConnection
{
    Q_OBJECT

public:
    ModBus(QObject *parent = nullptr);
    ~ModBus();

    bool start(const ConnectStruct &connectStruct) override;

signals:
    void clearBuffer();

private:
    quint8 obtainDelay(const quint32 baudRate);
};
