#include "interfaces/utils/device_watcher.h"

#include <interfaces/ifaces/usbhidport.h>

#ifdef Q_OS_WINDOWS
// clang-format off
#include <windows.h>
// Header dbt must be the last header, thanx to microsoft
#include <dbt.h>
// clang-format on
#endif

namespace Interface
{

DeviceWatcher::DeviceWatcher(QWidget *parent) noexcept : QObject(parent)
{
    registerForDeviceNotifications(parent);
}

bool DeviceWatcher::registerForDeviceNotifications(QWidget *widget)
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

void DeviceWatcher::handleNativeEvent(const QByteArray &eventType, void *msg) noexcept
{
#if defined(Q_OS_WINDOWS)
    if (eventType != "windows_generic_MSG")
        return;
    if (msg == nullptr)
        return;
    auto message = static_cast<MSG *>(msg);
    int msgClass = message->message;
    if (msgClass != WM_DEVICECHANGE)
        return;
    auto devInterface = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE *>(message->lParam);
    if (devInterface == nullptr)
        return;
    QString guid = QString::fromStdWString(&devInterface->dbcc_name[0]);
    quint32 deviceType = devInterface->dbcc_devicetype;
    quint32 msgType = message->wParam;
    if (deviceType != DBT_DEVTYP_DEVICEINTERFACE)
        return;
    QRegularExpression regex(HID::headerValidator);
    QRegularExpressionMatch match = regex.match(guid);
    if (!match.hasMatch())
        return;
    if (match.captured(0) != "USB" && match.captured(0) != "HID")
        return;
    // Тип уведомления об устройстве, которое пришло от ОС.
    switch (msgType)
    {
    // Устройство подключено
    case DBT_DEVICEARRIVAL:
        emit deviceConnected(guid);
        break;
    // Устройство отключено
    case DBT_DEVICEREMOVECOMPLETE:
        emit deviceDisconnected(guid);
        break;
    default:
        qInfo() << "Unhandled case: " << QString::number(msgType, 16);
    }
#elif defined(Q_OS_LINUX)
    Q_UNUSED(eventType);
    Q_UNUSED(msg);
#endif
}

} // namespace Interface
