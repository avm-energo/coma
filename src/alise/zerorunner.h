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
    void runServer();
    void stopServer();
signals:
    // void publishPowerStatus(AVTUK_14::Main powerStatus);
    void publishNtpStatus(bool);

    void timeRequest();

    void healthReceived(ZeroSubscriber::healthType);
    void timeReceived(timespec);

private:
    void proxy(zmq::socket_t &front, zmq::socket_t &sub, zmq::socket_t &pub);
    void polling();

    QMutex _mutex;

    zmq::context_t ctx_;
    zmq::socket_t frontend_;
    zmq::socket_t backendSub_;
    zmq::socket_t backendPub_;
};
}
