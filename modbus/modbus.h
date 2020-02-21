#ifndef MODBUS_H
#define MODBUS_H


#include <QObject>
#include <QTimer>
#include <QMutex>
#include <QModbusServer>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QUrl>
#include "../gen/s2.h"
#include "../gen/log.h"





class ModBus : public QObject
{
Q_OBJECT

public:

    struct ModBus_Settings
    {
        QString baud;
        QString parity;
        QString stop;
        QString adr;
    };

ModBus(ModBus_Settings Settings, QObject *parent);
~ModBus();

 QModbusRtuSerialMaster *modbusDevice;
 QModbusDevice::State* state;






public slots:


signals:
 void ModBusState(QModbusDevice::State*);


private:

private slots:

protected:
};

#endif
