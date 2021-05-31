#include "../gen/stdfunc.h"
#include "grpc_proto.grpc.pb.h"

#include <QObject>
#include <QThread>
#include <QTimer>
#include <grpcpp/grpcpp.h>

namespace runner
{

using alise::Health;
using alise::HelloReply;
using alise::HelloRequest;
using alise::Reply;
using alise::Synchronizer;
using alise::UnixTimestamp;
using google::protobuf::Empty;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
// Logic and data behind the server's behavior.
class SynchronizerServiceImpl final : public Synchronizer::Service
{
public:
    SynchronizerServiceImpl(QTimer *timer) : Synchronizer::Service(), m_timer(timer)
    {
    }

private:
    Status SayHello(ServerContext *context, const HelloRequest *request, HelloReply *reply) override;
    Status GetHealth(ServerContext *context, const Empty *request, Health *health) override;
    Status SetHealth(ServerContext *context, const Health *request, Reply *response) override;
    Status SetTime(ServerContext *context, const UnixTimestamp *request, Reply *response) override;
    Status GetTime(ServerContext *context, const Empty *request, UnixTimestamp *response) override;

private:
    QTimer *m_timer;
};

constexpr uint32_t timeout = 300000;

class ServerRunner : public QObject
{
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

private:
    SharedPointer<QTimer> m_timer = SharedPointer<QTimer>(new QTimer(this), [](QObject *obj) { obj->deleteLater(); });
    const uint16_t m_port;
    const std::string m_addr;
    std::unique_ptr<Server> m_server;
};

}

class ServerController : public QObject
{
public:
    ServerController(QObject *parent = nullptr) : ServerController(50051, "0.0.0.0", parent)
    {
    }
    ServerController(uint16_t port, std::string addr, QObject *parent = nullptr)
        : QObject(parent), worker(new runner::ServerRunner(port, addr))
    {
        worker->moveToThread(&workerThread);
        connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
        connect(&workerThread, &QThread::started, worker, &runner::ServerRunner::runServer);
    }

    ~ServerController()
    {
        workerThread.quit();
        workerThread.wait();
    }
    void launch()
    {
        workerThread.start();
    }
    void shutdown()
    {
        QMetaObject::invokeMethod(worker, &runner::ServerRunner::stopServer, Qt::DirectConnection);
    }
signals:

private:
    runner::ServerRunner *worker;
    QThread workerThread;
};
