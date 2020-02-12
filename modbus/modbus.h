#ifndef MODBUS_H
#define MODBUS_H

#include "../gen/s2.h"
#include "../gen/log.h"

#include <QObject>
#include <QTimer>
#include <QMutex>
#include <QModbusServer>


class ModBus : public QObject
{
Q_OBJECT

public:
ModBus();
~ModBus();





public slots:


signals:


private:

private slots:

protected:
};

#endif
