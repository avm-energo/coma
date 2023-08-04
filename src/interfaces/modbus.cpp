#include "modbus.h"

#include "../s2/s2util.h"
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
    auto portThread = new QThread;
    auto parseThread = new QThread;
    auto port = new SerialPort;
    ifacePort = port;
    auto parser = new ModbusThread;
    parser->setDelay(obtainDelay(st.Baud));
    parser->setDeviceAddress(st.Address);
    parseThread->setObjectName("Modbus thread");

    connect(portThread, &QThread::started, port, &BasePort::poll, Qt::QueuedConnection);
    connect(parseThread, &QThread::started, parser, &ModbusThread::run);
    connect(parser, &ModbusThread::finished, parseThread, &QThread::quit);
    connect(this, &BaseInterface::wakeUpParser, parser, &BaseInterfaceThread::wakeUp, Qt::DirectConnection);
    connect(port, &BasePort::dataReceived, parser, &BaseInterfaceThread::processReadBytes, Qt::DirectConnection);
    connect(parser, &ModbusThread::sendDataToPort, port, &BasePort::writeDataSync, Qt::QueuedConnection);
    connect(parser, &ModbusThread::clearBuffer, port, &SerialPort::clear, Qt::DirectConnection);
    connect(port, &BasePort::clearQueries, parser, &BaseInterfaceThread::clear, Qt::DirectConnection);
    // connect(port, &SerialPort::error, this, &ModBus::sendReconnectSignal, Qt::DirectConnection);
    connect(this, &BaseInterface::reconnect, port, &BasePort::reconnect, Qt::DirectConnection);
    connect(port, &BasePort::stateChanged, this, &BaseInterface::stateChanged, Qt::QueuedConnection);
    // TODO: Нужен полноценный обработчик ошибок от портов для каждого интерфейса
    // To Think: Можно сделать виртуальный слот в BaseInterface, а в потомках его переопределять
    connect(
        port, &BasePort::error, this,
        [this, parser, port](BasePort::PortErrors error) {
            if (error == BasePort::PortErrors::Timeout && parser->m_currentCommand.command == C_ReqBSI)
            {
                port->closeConnection();
                qCritical() << "Превышено время ожидания блока BSI. Disconnect...";
                emit disconnected();
            }
        },
        Qt::DirectConnection);

    connect(port, &BasePort::finished, portThread, &QThread::quit);
    connect(port, &BasePort::finished, parseThread, &QThread::quit);
    connect(portThread, &QThread::finished, port, &QObject::deleteLater);
    connect(parseThread, &QThread::finished, parser, &QObject::deleteLater);
    connect(portThread, &QThread::finished, &QObject::deleteLater);
    connect(parseThread, &QThread::finished, &QObject::deleteLater);

    connect(port, &SerialPort::started, port, [=] {
        setState(State::Run);
        parser->moveToThread(parseThread);
        port->moveToThread(portThread);
        parseThread->start();
        portThread->start();
        StdFunc::Wait(1000);
    });
    if (!port->init(st))
    {
        port->closeConnection();
        return false;
    }
    emit connected();
    return true;
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
