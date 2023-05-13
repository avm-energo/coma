#include "modbus.h"

#include "../s2/s2.h"
#include "interfacesettings.h"
#include "modbusthread.h"
#include "serialport.h"
#include "settingstypes.h"

#include <QDebug>
#include <QStandardPaths>
#include <QThread>
#include <QWaitCondition>
#include <algorithm>
#include <gen/error.h>
#include <gen/registers.h>
#include <gen/stdfunc.h>
#include <gen/timefunc.h>

using namespace MBS;
using namespace Interface;

ModBus::ModBus(QObject *parent) : BaseInterface(parent)
{
}

ModBus::~ModBus()
{
}

bool ModBus::start(const ConnectStruct &connectStruct)
{
    Log->Init(QString(metaObject()->className()) + ".log");
    Log->info(logStart);
    Q_ASSERT(std::holds_alternative<SerialPortSettings>(connectStruct.settings));
    qInfo() << metaObject()->className() << "is connecting...";
    if (!std::holds_alternative<SerialPortSettings>(connectStruct.settings))
        return false;

    const auto &st = std::get<SerialPortSettings>(connectStruct.settings);
    auto thr = new QThread;
    auto port = new SerialPort(this);
    auto parser = new ModbusThread;
    parser->setDelay(obtainDelay(st.Baud));
    parser->setDeviceAddress(st.Address);
    thr->setObjectName("Modbus thread");

    connect(thr, &QThread::started, parser, &ModbusThread::run);
    connect(parser, &ModbusThread::finished, thr, &QThread::quit);
    connect(thr, &QThread::finished, port, &SerialPort::disconnect);
    connect(thr, &QThread::finished, port, &QObject::deleteLater);
    connect(thr, &QThread::finished, thr, &QObject::deleteLater);
    connect(thr, &QThread::finished, parser, &QObject::deleteLater);
    connect(port, &SerialPort::read, parser, &ModbusThread::processReadBytes);
    connect(parser, &ModbusThread::write, port, &SerialPort::writeBytes);
    connect(parser, &ModbusThread::clearBuffer, port, &SerialPort::clear);
    connect(port, &SerialPort::error, this, &ModBus::sendReconnectSignal);
    connect(this, &BaseInterface::reconnect, port, &SerialPort::reconnect);
    connect(port, &SerialPort::connected, port, [=] {
        setState(State::Run);
        parser->moveToThread(thr);
        thr->start();
        StdFunc::Wait(1000);
    });
    if (!port->init(st))
    {
        port->deleteLater();
        parser->deleteLater();
        thr->deleteLater();
        return false;
    }
    return true;
}

void ModBus::sendReconnectSignal()
{
    switch (state())
    {
    case State::Run:
        setState(State::Wait);
        break;
    case State::Wait:
        emit reconnect();
        break;
    default:
        break;
    }
}

const quint8 ModBus::obtainDelay(const quint32 baudRate) const
{
    switch (baudRate)
    {
    case 2400:
        return 16;
    case 4800:
        return 8;
    case 9600:
        return 4;
    case 19200:
        return 3;
    default:
        return 2;
    }
}
