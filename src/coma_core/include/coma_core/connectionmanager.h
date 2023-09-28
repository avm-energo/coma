#pragma once

#include <QByteArray>
#include <QWidget>
#include <interfaces/types/settingstypes.h>

namespace Core
{

class ConnectionManager : public QObject
{
    Q_OBJECT
private:
    bool m_reconnect;

public:
    explicit ConnectionManager(QObject *parent = nullptr);

    void createConnection(const ConnectStruct &connectionData);

    /// \brief Registering device's notifications for an incoming widget.
    /// \details Current implementation is ready only for Windows.
    bool registerDeviceNotifications(QWidget *widget);
    /// \brief Handling native events from OS.
    /// \details Current implementation is ready only for Windows.
    bool nativeEventHandler(const QByteArray &eventType, void *msg);

signals:
    void usbEvent(const QString &guid, quint32 msgType);

    //    void sendMessage(void *message);
};

} // namespace Interface

using IfaceConnManager = Core::ConnectionManager;
