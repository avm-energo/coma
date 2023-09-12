#include "connectionmanager.h"

#ifdef Q_OS_WINDOWS
// clang-format off
#include <windows.h>
// Header dbt must be the last header, thanx to microsoft
#include <dbt.h>
// clang-format on
#endif

namespace Interface
{

ConnectionManager::ConnectionManager(QObject *parent) : QObject(parent)
{
}

bool ConnectionManager::registerForDeviceNotification(QWidget *widget)
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
        if (msgType != WM_DEVICECHANGE)
            return false;
        emit sendMessage(message);
    }
#elif defined(Q_OS_LINUX)
    Q_UNUSED(eventType);
    Q_UNUSED(message);
#endif
    return false;
}

} // namespace Interface
