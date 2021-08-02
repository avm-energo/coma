#pragma once
#include "../gen/datatypes.h"
#include "../gen/modules.h"
#include "protos.pb.h"

#include <QMutex>
#include <QObject>
#include <QWaitCondition>
#include <vector>
#include <zmq.hpp>

// Q_DECLARE_METATYPE(alise::Health_Code);

class ZeroSubscriber : public QObject
{
    Q_OBJECT
public:
    using healthType = alise::Health_Code;
    // using alise::HelloReply;
    explicit ZeroSubscriber(zmq::context_t &ctx, int sock_type, QObject *parent = nullptr);
    void work();
    void stop()
    {
        is_active = false;
    }
signals:
    void timeReceived(timespec);
    void timeRequest();
    void healthReceived(healthType);
    void helloReceived(const QString, quint32);

private:
    zmq::context_t &_ctx;
    zmq::socket_t _worker;
    QMutex _mutex;
    QWaitCondition _waiter;
    bool is_active = true;
};

Q_DECLARE_METATYPE(ZeroSubscriber::healthType)
