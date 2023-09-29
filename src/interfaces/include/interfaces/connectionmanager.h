#pragma once

#include <QByteArray>
#include <QWidget>
#include <interfaces/connectioncontext.h>
#include <interfaces/types/settingstypes.h>

namespace Interface
{

class ConnectionManager : public QObject
{
    Q_OBJECT
private:
    ConnectionContext m_context;
    bool m_reconnect;
    QWidget m_parent;

public:
    explicit ConnectionManager(QWidget *parent = nullptr);
    void createConnection(const ConnectStruct &connectionData);
    void reconnectConnection();
    void breakConnection();

    /// \brief Registering device's notifications for an incoming widget.
    /// \details Current implementation is ready only for Windows.
    bool registerDeviceNotifications(QWidget *widget);
    /// \brief Handling native events from OS.
    /// \details Current implementation is ready only for Windows.
    bool nativeEventHandler(const QByteArray &eventType, void *msg);

signals:
    void connectSuccesfull();
    void connectFailed();
    void reconnect();
    void disconnectError();

private slots:
    void portErrorHadler(const BasePort::PortErrors error);
};

} // namespace Interface

using IfaceConnManager = Interface::ConnectionManager;
