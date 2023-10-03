#include "interfaces/connectionmanager.h"

#include <gen/std_ext.h>
#include <interfaces/ports/serialport.h>
#include <interfaces/ports/usbhidport.h>
#include <interfaces/threads/modbusthread.h>
#include <interfaces/threads/protocomthread.h>

#ifdef Q_OS_WINDOWS
// clang-format off
#include <windows.h>
// Header dbt must be the last header, thanx to microsoft
#include <dbt.h>
// clang-format on
#endif

namespace Interface
{

ConnectionManager::ConnectionManager(QWidget *parent)
    : QObject(parent), m_currentConnection(nullptr), m_reconnect(false), m_parent(parent)
{
}

void ConnectionManager::createConnection(const ConnectStruct &connectionData)
{
    if (m_currentConnection != nullptr)
        breakConnection();

    m_currentConnection = new Connection(this);
    std::visit( //
        overloaded {
            [this](const UsbHidSettings &settings) {
                auto port = new UsbHidPort(settings);
                auto parser = new ProtocomThread(m_currentConnection->m_queue);
                m_context.init(port, parser, Strategy::Sync, Qt::DirectConnection);
            },
            [this](const SerialPortSettings &settings) {
                auto port = new SerialPort();
                port->init(settings);
                auto parser = new ModbusThread(m_currentConnection->m_queue);
                parser->setDeviceAddress(settings.Address);
                m_context.init(port, parser, Strategy::Sync, Qt::QueuedConnection);
            },
            [this]([[maybe_unused]] const IEC104Settings &settings) {
                ;
                ;
            },
            [this]([[maybe_unused]] const EmulatorSettings &settings) {
#ifdef ENABLE_EMULATOR
                ;
                ;
#endif
            } //
        },
        connectionData.settings);
    connect(m_context.m_port, &BasePort::error, this, &ConnectionManager::portErrorHadler, Qt::DirectConnection);
    if (m_context.run(m_currentConnection))
        emit connectSuccesfull();
    else
        emit connectFailed();
    Connection::setIface(Connection::InterfacePointer { m_currentConnection });
}

void ConnectionManager::reconnectConnection()
{
    ;
    emit reconnect();
}

void ConnectionManager::breakConnection()
{
    m_context.reset();
    m_currentConnection = nullptr;
    Connection::s_connection.reset();
}

void ConnectionManager::portErrorHadler(const BasePort::PortErrors error)
{
    if (error == BasePort::PortErrors::Timeout
        && m_context.m_parser->m_currentCommand.command == Interface::Commands::C_ReqBSI)
    {
        qCritical() << "Превышено время ожидания блока BSI. Disconnect...";
        breakConnection();
    }
}

bool ConnectionManager::registerDeviceNotifications(QWidget *widget)
{
    if (widget != nullptr)
    {
#if defined(Q_OS_WINDOWS)
        DEV_BROADCAST_DEVICEINTERFACE devInt;
        ZeroMemory(&devInt, sizeof(devInt));
        GUID _guid { 0xa5dcbf10, 0x6530, 0x11d2, { 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed } };
        devInt.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
        devInt.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        // With DEVICE_NOTIFY_ALL_INTERFACE_CLASSES this property ignores
        devInt.dbcc_classguid = _guid;
        HANDLE widgetHandle { reinterpret_cast<HANDLE>(widget->winId()) };
        // NOTE Проверить со всеми модулями
        HDEVNOTIFY blub = RegisterDeviceNotification(widgetHandle, &devInt, //
            DEVICE_NOTIFY_ALL_INTERFACE_CLASSES /*DBT_DEVTYP_OEM*/ /*DEVICE_NOTIFY_WINDOW_HANDLE*/);
        return blub != NULL;
#elif defined(Q_OS_LINUX)
        // TODO: linux code goes here
        return true;
#endif
    }
    return false;
}

bool ConnectionManager::nativeEventHandler(const QByteArray &eventType, void *msg)
{
#if defined(Q_OS_WINDOWS)
    do
    {
        if (eventType != "windows_generic_MSG")
            break;
        if (msg == nullptr)
            break;
        auto message = static_cast<MSG *>(msg);
        int msgClass = message->message;
        if (msgClass != WM_DEVICECHANGE)
            break;
        auto devInterface = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE *>(message->lParam);
        if (devInterface == nullptr)
            break;
        QString guid = QString::fromStdWString(&devInterface->dbcc_name[0]);
        quint32 deviceType = devInterface->dbcc_devicetype;
        quint32 msgType = message->wParam;
        if (deviceType != DBT_DEVTYP_DEVICEINTERFACE)
            break;
        QRegularExpression regex(HID::headerValidator);
        QRegularExpressionMatch match = regex.match(guid);
        if (!match.hasMatch())
            break;
        if (match.captured(0) != "USB" && match.captured(0) != "HID")
            break;
        // emit usbEvent(guid, msgType);
    } while (false);
#elif defined(Q_OS_LINUX)
    Q_UNUSED(eventType);
    Q_UNUSED(msg);
#endif
    return false;
}

} // namespace Interface
