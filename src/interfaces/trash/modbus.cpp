#include "interfaces/conn/modbus.h"

#include <QDebug>
#include <QStandardPaths>
#include <QThread>
#include <QWaitCondition>
#include <algorithm>
#include <gen/error.h>
#include <gen/stdfunc.h>
#include <gen/timefunc.h>
#include <interfaces/ports/serialport.h>
#include <interfaces/threads/modbusthread.h>
#include <interfaces/types/interfacesettings.h>
#include <interfaces/types/settingstypes.h>
#include <s2/s2util.h>

using namespace MBS;
using namespace Interface;

ModBus::ModBus(QObject *parent) : BaseConnection(parent)
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
    port->init(st);
    ifacePort = port;
    auto parser = new ModbusThread(m_queue);
    parser->setDeviceAddress(st.Address);

    // Старт
    connect(portThread, &QThread::started, port, &BasePort::poll, Qt::QueuedConnection);
    connect(parseThread, &QThread::started, parser, &BaseConnectionThread::run);
    // Рабочий режим
    connect(this, &BaseConnection::wakeUpParser, parser, &BaseConnectionThread::wakeUp, Qt::DirectConnection);
    connect(port, &BasePort::dataReceived, parser, &BaseConnectionThread::processReadBytes, Qt::DirectConnection);
    connect(parser, &BaseConnectionThread::sendDataToPort, port, &BasePort::writeDataSync, Qt::QueuedConnection);
    connect(parser, &BaseConnectionThread::responseSend, this, &BaseConnection::responseHandle, Qt::DirectConnection);
    connect(port, &BasePort::stateChanged, this, &BaseConnection::stateChanged, Qt::QueuedConnection);
    // Очистка
    connect(port, &BasePort::clearQueries, parser, &BaseConnectionThread::clear, Qt::DirectConnection);
    // Остановка
    connect(port, &BasePort::finished, parser, &BaseConnectionThread::wakeUp, Qt::DirectConnection);
    connect(port, &BasePort::finished, portThread, &QThread::quit);
    connect(port, &BasePort::finished, parseThread, &QThread::quit);
    connect(portThread, &QThread::finished, port, &QObject::deleteLater);
    connect(parseThread, &QThread::finished, parser, &QObject::deleteLater);
    connect(portThread, &QThread::finished, &QObject::deleteLater);
    connect(parseThread, &QThread::finished, &QObject::deleteLater);
    connect(parser, &BaseConnectionThread::finished, parseThread, &QThread::quit);

    // TODO: Нужен полноценный обработчик ошибок от портов для каждого интерфейса
    // To Think: Можно сделать виртуальный слот в BaseInterface, а в потомках его переопределять
    connect(
        port, &BasePort::error, this,
        [this, parser, port](BasePort::PortErrors error) {
            if (error == BasePort::PortErrors::Timeout
                && parser->m_currentCommand.command == Interface::Commands::C_ReqBSI)
            {
                port->closeConnection();
                qCritical() << "Превышено время ожидания блока BSI. Disconnect...";
                emit disconnected();
            }
        },
        Qt::DirectConnection);

    connect(port, &BasePort::started, port, [=] {
        qInfo() << metaObject()->className() << "connected";
        parser->moveToThread(parseThread);
        port->moveToThread(portThread);
        parseThread->start();
        portThread->start();
        StdFunc::Wait(1000);
    });
    if (!port->connect())
    {
        port->closeConnection();
        port->deleteLater();
        parser->deleteLater();
        parseThread->deleteLater();
        portThread->deleteLater();
        return false;
    }
    return true;
}

// quint8 ModBus::obtainDelay(const quint32 baudRate)
//{
//    switch (baudRate)
//    {
//    case 2400:
//        return 16;
//    case 4800:
//        return 8;
//    case 9600:
//        return 4;
//    case 19200:
//        return 3;
//    default:
//        return 2;
//    }
//}
