#pragma once

#include <QByteArray>
#include <QWidget>

namespace Interface
{

class ConnectionManager : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionManager(QObject *parent = nullptr);

    bool registerForDeviceNotification(QWidget *widget);
    bool nativeEventHandler(const QByteArray &eventType, void *message);

signals:
    void sendMessage(void *message);
};

} // namespace Interface

using IfaceConnManager = Interface::ConnectionManager;
