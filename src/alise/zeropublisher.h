#pragma once
#include "../gen/datatypes.h"
#include "../gen/modules.h"

#include <QMutex>
#include <QObject>
#include <QWaitCondition>
#include <queue>
#include <vector>
#include <zmq.hpp>
Q_DECLARE_METATYPE(AVTUK_14::DiscretePowerSignals);
Q_DECLARE_METATYPE(AVTUK_14::Indication);

#define LOG_PROTOBUF

class ZeroPublisher : public QObject
{
    Q_OBJECT
public:
    explicit ZeroPublisher(zmq::context_t &ctx, int sock_type, QObject *parent = nullptr);

    void work();
    void stop()
    {
        is_active = false;
    }

signals:
public slots:
    void publishTime(const timespec time);
    void publishPowerStatus(const AVTUK_14::DiscretePowerSignals powerStatus);
    void publishBlock(const DataTypes::BlockStruct blk);

private:
    zmq::context_t &_ctx;
    zmq::socket_t _worker;
    QMutex _mutex;
    QWaitCondition _waiter;
    bool is_active = true;
    std::queue<std::string> _queue;

    void send(std::string &str)
    {
        zmq::message_t identity(std::string("sb"));
        zmq::message_t msg(str);
        _worker.send(identity, zmq::send_flags::sndmore);
        _worker.send(msg, zmq::send_flags::none);
    }
    void checkQueue()
    {
        while (!_queue.empty())
        {
            send(_queue.back());
            _queue.pop();
        }
    }
};
