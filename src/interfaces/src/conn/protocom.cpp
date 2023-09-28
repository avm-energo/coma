#include "interfaces/conn/protocom.h"

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <gen/files.h>
#include <gen/stdfunc.h>
#include <interfaces/ports/usbhidport.h>
#include <interfaces/threads/protocomthread.h>
#include <interfaces/types/settingstypes.h>

//#ifdef Q_OS_WINDOWS
//// clang-format off
//#include <windows.h>
//// Header dbt must be the last header, thanx to microsoft
//#include <dbt.h>
//// clang-format on
//#endif

using Proto::CommandStruct;
using Proto::Starters;

Protocom::Protocom(QObject *parent) : BaseConnection(parent)
{
    qRegisterMetaType<UsbHidSettings>();
}

Protocom::~Protocom()
{
}

bool Protocom::start(const ConnectStruct &st)
{
    Q_ASSERT(std::holds_alternative<UsbHidSettings>(st.settings));
    if (!std::holds_alternative<UsbHidSettings>(st.settings))
        return false;
    const auto settings = std::get<UsbHidSettings>(st.settings);
    return start(settings);
}

bool Protocom::start(const UsbHidSettings &usbhid)
{
    auto port = new UsbHidPort(usbhid);
    ifacePort = port;
    auto parser = new ProtocomThread(m_queue);
    auto portThread = new QThread;
    auto parseThread = new QThread;

    // Старт
    connect(portThread, &QThread::started, port, &BasePort::poll);
    connect(parseThread, &QThread::started, parser, &BaseConnectionThread::run);
    // Рабочий режим
    connect(this, &BaseConnection::wakeUpParser, parser, &BaseConnectionThread::wakeUp, Qt::DirectConnection);
    connect(port, &BasePort::dataReceived, parser, &BaseConnectionThread::processReadBytes, Qt::DirectConnection);
    connect(parser, &BaseConnectionThread::sendDataToPort, port, &BasePort::writeDataSync, Qt::DirectConnection);
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

    if (!port->connect())
    {
        port->deleteLater();
        parser->deleteLater();
        parseThread->deleteLater();
        portThread->deleteLater();
        return false;
    }
    //#ifdef Q_OS_WINDOWS
    //    connect(
    //        this, &BaseConnection::nativeEvent, port,
    //        [port](auto &&msg) {
    //            auto message = static_cast<MSG *>(msg);
    //            if (!msg)
    //                return;
    //            auto devint = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE *>(message->lParam);
    //            if (!devint)
    //                return;
    //            USBMessage usbMessage;
    //            usbMessage.guid = QString::fromStdWString(&devint->dbcc_name[0]);
    //            usbMessage.type = devint->dbcc_devicetype;

    //            QMetaObject::invokeMethod(
    //                port, [=] { port->usbEvent(usbMessage, message->wParam); }, Qt::QueuedConnection);
    //        },
    //        Qt::DirectConnection);
    //#endif
    qInfo() << metaObject()->className() << "connected";
    port->moveToThread(portThread);
    parser->moveToThread(parseThread);
    setState(State::Run);
    portThread->start();
    parseThread->start();
    return true;
}
