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
    connect(thr, &QThread::finished, port, &BasePort::disconnect);
    connect(thr, &QThread::finished, port, &QObject::deleteLater);
    connect(thr, &QThread::finished, thr, &QObject::deleteLater);
    connect(thr, &QThread::finished, parser, &QObject::deleteLater);
    connect(this, &BaseInterface::wakeUpParser, parser, &BaseInterfaceThread::wakeUp, Qt::DirectConnection);
    connect(port, &BasePort::dataReceived, parser, &ModbusThread::processReadBytes);
    connect(parser, &ModbusThread::sendDataToPort, port, &BasePort::writeData);
    connect(parser, &ModbusThread::clearBuffer, port, &SerialPort::clear);
    connect(port, &SerialPort::error, this, &ModBus::sendReconnectSignal);
    connect(this, &BaseInterface::reconnect, port, &BasePort::reconnect);
    connect(port, &SerialPort::started, port, [=] {
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
    emit connected();
    return true;
}

void ModBus::sendReconnectSignal()
{
    switch (state())
    {
    case State::Run:
        setState(State::Reconnect);
        break;
    case State::Reconnect:
        emit reconnect();
        break;
    default:
        break;
    }
}

quint8 ModBus::obtainDelay(const quint32 baudRate)
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
