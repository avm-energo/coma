#pragma once

#include <QByteArray>
#include <QWidget>

namespace Interface
{

class DeviceWatcher : public QObject
{
    Q_OBJECT
private:
    /// \brief Registering device's notifications for a passed widget.
    /// \details The current implementation only supports Windows.
    bool registerDeviceNotifications(QWidget *widget);

public:
    explicit DeviceWatcher(QWidget *parent) noexcept;
    /// \brief Handling native events from OS.
    /// \details The current implementation only supports Windows.
    void handleNativeEvent(const QByteArray &eventType, void *msg) noexcept;

signals:
    /// \brief That signal is emitted when the HID device has been disconnected from the PC.
    void deviceDisconnected(const QString &guid);
    /// \brief That signal is emitted when the HID device has been connected to the PC.
    void deviceConnected(const QString &guid);
};

} // namespace Interface
