#include "../../interfaces/baseinterface.h"
#include "../../interfaces/emulator.h"
#include "../../interfaces/iec104.h"
#include "../../interfaces/modbus.h"
#include "../../interfaces/protocom.h"

#include <coma_core/connectionmanager.h>
#include <gen/std_ext.h>

#ifdef Q_OS_WINDOWS
// clang-format off
#include <windows.h>
// Header dbt must be the last header, thanx to microsoft
#include <dbt.h>
// clang-format on
#endif

namespace Core
{

ConnectionManager::ConnectionManager(QObject *parent) : QObject(parent), m_reconnect(false)
{
}

void ConnectionManager::createConnection(const ConnectStruct &connectionData)
{
    BaseInterface::InterfacePointer device;
    std::visit( //
        overloaded {
            [&]([[maybe_unused]] const UsbHidSettings &settings) {
                device.reset(new Protocom(this));
                ;
            },
            [&]([[maybe_unused]] const SerialPortSettings &settings) {
                device.reset(new ModBus(this));
                ;
            },
            [&]([[maybe_unused]] const IEC104Settings &settings) {
                device.reset(new IEC104(this));
                ;
            },
            [&]([[maybe_unused]] const EmulatorSettings &settings) {
                device.reset(new Emulator(this));
                ;
            } //
        },
        connectionData.settings);
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

bool ConnectionManager::nativeEventHandler(const QByteArray &eventType, void *message)
{
#if defined(Q_OS_WINDOWS)
    if (eventType == "windows_generic_MSG")
    {
        auto msg = static_cast<MSG *>(message);
        int msgType = msg->message;
        if (msgType == WM_DEVICECHANGE)
            emit sendMessage(message);
    }
#elif defined(Q_OS_LINUX)
    Q_UNUSED(eventType);
    Q_UNUSED(message);
#endif
    return false;
}

} // namespace Interface
