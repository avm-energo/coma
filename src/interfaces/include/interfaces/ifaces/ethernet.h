#pragma once

#include <QTcpSocket>
#include <interfaces/ifaces/baseinterface.h>
#include <interfaces/types/settingstypes.h>

class Ethernet final : public BaseInterface
{
    Q_OBJECT
private:
    IEC104Settings m_settings;
    QTcpSocket *m_socket;

public:
    explicit Ethernet(const IEC104Settings &settings, QObject *parent = nullptr);

private:
    bool connect() override;
    void disconnect() override;
    QByteArray read(bool *status = nullptr) override;
    bool write(const QByteArray &data) override;

private slots:
    void handleSocketState(const QAbstractSocket::SocketState state);
    void handleSocketError(const QAbstractSocket::SocketError err);
};
