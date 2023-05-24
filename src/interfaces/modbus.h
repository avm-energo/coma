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

signals:
    void clearBuffer();

private:
    quint8 obtainDelay(const quint32 baudRate);

    // private slots:
    //    void sendReconnectSignal();
};
