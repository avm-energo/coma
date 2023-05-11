#include "protocom.h"

#include "protocomthread.h"
#include "settingstypes.h"
#include "usbhidport.h"

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <gen/files.h>
#include <gen/stdfunc.h>

#ifdef Q_OS_WINDOWS
// clang-format off
#include <windows.h>
// Header dbt must be the last header, thanx to microsoft
#include <dbt.h>
// clang-format on
#endif

using Proto::CommandStruct;
using Proto::Starters;

Protocom::Protocom(QObject *parent) : BaseInterface(parent)
{
    qRegisterMetaType<UsbHidSettings>();
}

Protocom::~Protocom()
{
}

bool Protocom::start(const ConnectStruct &st)
{
    // Q_ASSERT(Board::GetInstance().interfaceType() == Board::InterfaceType::USB);
    Q_ASSERT(std::holds_alternative<UsbHidSettings>(st.settings));

    if (!std::holds_alternative<UsbHidSettings>(st.settings))
        return false;
    const auto settings = std::get<UsbHidSettings>(st.settings);
    return start(settings);
}

bool Protocom::start(const UsbHidSettings &usbhid)
{
    auto port = new UsbHidPort(usbhid, Log.get());
    auto parser = new ProtocomThread;
    auto portThread = new QThread;
    auto parseThread = new QThread;

    // NOTE После остановки потоков мы всё еще обращаемся
    // к интерфейсу для обновления данных
    QList<QMetaObject::Connection> connections;
    // Старт
    connect(portThread, &QThread::started, port, &UsbHidPort::poll);
    connect(parseThread, &QThread::started, parser, &ProtocomThread::run);
    // Рабочий режим
    connections << connect(this, &Protocom::wakeUpParser, parser, &ProtocomThread::wakeUp, Qt::DirectConnection);
    connect(port, &UsbHidPort::dataReceived, parser, &ProtocomThread::processReadBytes, Qt::DirectConnection);
    connect(parser, &ProtocomThread::writeDataAttempt, port, &UsbHidPort::writeDataAttempt, Qt::DirectConnection);
    // Прерывание
    connect(port, &UsbHidPort::clearQueries, parser, &ProtocomThread::clear, Qt::DirectConnection);
    // Остановка
    connect(port, &UsbHidPort::finished, parser, &ProtocomThread::wakeUp, Qt::DirectConnection);
    connect(port, &UsbHidPort::finished, [=] {
        for (const auto &var : connections)
        {
            QObject::disconnect(var);
        }
    });
    connect(port, &UsbHidPort::finished, portThread, &QThread::quit);
    connect(port, &UsbHidPort::finished, parseThread, &QThread::quit);
    connect(portThread, &QThread::finished, port, &QObject::deleteLater);
    connect(parseThread, &QThread::finished, parser, &QObject::deleteLater);
    connect(portThread, &QThread::finished, &QObject::deleteLater);
    connect(parseThread, &QThread::finished, &QObject::deleteLater);

    if (!port->setupConnection())
    {
        port->deleteLater();
        parser->deleteLater();
        parseThread->deleteLater();
        portThread->deleteLater();
        return false;
    }
#ifdef Q_OS_WINDOWS
    connect(
        this, &BaseInterface::nativeEvent, port,
        [port](auto &&msg) {
            auto message = static_cast<MSG *>(msg);
            if (!msg)
                return;
            auto devint = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE *>(message->lParam);
            if (!devint)
                return;
            USBMessage usbMessage;
            usbMessage.guid = QString::fromStdWString(&devint->dbcc_name[0]);
            usbMessage.type = devint->dbcc_devicetype;

            QMetaObject::invokeMethod(
                port, [=] { port->usbEvent(usbMessage, message->wParam); }, Qt::QueuedConnection);
        },
        Qt::DirectConnection);
#endif
    connect(port, &UsbHidPort::stateChanged, this, &BaseInterface::stateChanged, Qt::QueuedConnection);
    qInfo() << metaObject()->className() << "connected";
    port->moveToThread(portThread);
    parser->moveToThread(parseThread);
    setState(Run);
    portThread->start();
    parseThread->start();
    return true;
}

// Proto::WCommands Protocom::getWCommand(Queries::Commands cmd)
//{
//    static const QMap<Queries::Commands, Proto::WCommands> protoWCommandMap {
//        { Queries::QC_SetStartupValues, Proto::InitStartupValues },    //
//        { Queries::QC_ClearStartupValues, Proto::EraseStartupValues }, //
//    };
//    return protoWCommandMap.value(cmd);
//}

// Proto::WSCommands Protocom::getWSCommand(Queries::Commands cmd)
//{
//    static const QMap<Queries::Commands, Proto::WSCommands> protoWSCommandMap {
//        { Queries::QC_SetStartupValues, Proto::InitStartupAll },             //
//        { Queries::QC_SetStartupPhaseA, Proto::InitStartupPhaseA },          //
//        { Queries::QC_SetStartupPhaseB, Proto::InitStartupPhaseB },          //
//        { Queries::QC_SetStartupPhaseC, Proto::InitStartupPhaseC },          //
//        { Queries::QC_SetStartupUnbounced, Proto::InitStartupUnbounced },    //
//        { Queries::QC_SetTransOff, Proto::SetTransOff },                     //
//        { Queries::QC_ClearStartupValues, Proto::EraseStartupAll },          //
//        { Queries::QC_ClearStartupError, Proto::EraseStartupInitError },     //
//        { Queries::QC_ClearStartupUnbounced, Proto::ClearStartupUnbounced }, //
//    };
//    return protoWSCommandMap.value(cmd);
//}
