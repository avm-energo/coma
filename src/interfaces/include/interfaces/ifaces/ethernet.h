#pragma once

#include <interfaces/ifaces/baseinterface.h>
#include <interfaces/types/serial_settings.h>

#include <QTcpSocket>

class Ethernet final : public BaseInterface
{
    Q_OBJECT
private:
    IEC104Settings *m_settings;
    QTcpSocket *m_socket;

public:
    explicit Ethernet(IEC104Settings *settings, QObject *parent = nullptr);

private:
    bool connect() override;
    void disconnect() override;
    QByteArray read(bool &status) override;
    bool write(const QByteArray &data) override;

private slots:
    void handleSocketState(const QAbstractSocket::SocketState state);
    void handleSocketError(const QAbstractSocket::SocketError err);
};
