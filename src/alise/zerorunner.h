#pragma once
#include "protos.pb.h"
#include "zeropublisher.h"
#include "zerosubscriber.h"

#include <QMutex>
#include <QObject>
#include <zmq.hpp>
namespace runner
{
constexpr uint32_t timeout = 300000;

class ZeroRunner : public QObject
{
    Q_OBJECT
public:
    ZeroRunner(QObject *parent = nullptr)
        : QObject(parent)
        , ctx_(1)
        , frontend_(ctx_, ZMQ_ROUTER)
        , backendSub_(ctx_, ZMQ_DEALER)
        , backendPub_(ctx_, ZMQ_DEALER)
    {
    }
    enum
    {
        kMaxThread = 1
    };
public slots:
    bool runServer();
    void stopServer();
signals:
    void publishTime(timespec time);
    void publishPowerStatus(AVTUK_14::Main powerStatus);
    void timeReceived(timespec);
    void healthReceived(ZeroSubscriber::healthType);

private:
    void proxy(zmq::socket_t &front, zmq::socket_t &sub, zmq::socket_t &pub);

    QMutex _mutex;

    zmq::context_t ctx_;
    zmq::socket_t frontend_;
    zmq::socket_t backendSub_;
    zmq::socket_t backendPub_;
};
}
