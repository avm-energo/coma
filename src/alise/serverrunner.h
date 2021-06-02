#pragma once
#include "grpc_sync_server.h"

#include <QMutex>
namespace runner
{
constexpr uint32_t timeout = 300000;

class ServerRunner : public QObject
{
    Q_OBJECT
public:
    ServerRunner(QObject *parent = nullptr) : ServerRunner(50051, "0.0.0.0", parent)
    {
    }
    ServerRunner(uint16_t port, std::string addr, QObject *parent = nullptr)
        : QObject(parent), m_port(port), m_addr(addr)
    {
        m_timer->setInterval(timeout);
        m_timer->start();
    }
public slots:
    void runServer();
    void stopServer();
signals:
    void healthChanged(const alise::Health);
    void timeChanged(const alise::UnixTimestamp);
    void requestTime(alise::UnixTimestamp &);
    void requestPowerStatus(alise::PowerStatus &);

private:
    SharedPointer<QTimer> m_timer = SharedPointer<QTimer>(new QTimer(this), [](QObject *obj) { obj->deleteLater(); });
    const uint16_t m_port;
    const std::string m_addr;
    std::unique_ptr<Server> m_server;
    QMutex _mutex;
};
}
