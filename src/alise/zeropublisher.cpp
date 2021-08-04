#include "zeropublisher.h"

#include "protos.pb.h"

#include <QDebug>
#include <functional>
#include <memory>
#include <random>
#include <thread>

ZeroPublisher::ZeroPublisher(zmq::context_t &ctx, int sock_type, QObject *parent)
    : QObject(parent), _ctx(ctx), _worker(_ctx, sock_type)
{
}

void ZeroPublisher::work()
{
    try
    {
        _worker.connect("inproc://backendPub");
        while (is_active)
        {
            QMutexLocker locker(&_mutex);
            checkQueue();
            _waiter.wait(&_mutex);
        }
    } catch (std::exception &e)
    {
        qDebug() << "Exception: " << e.what();
    }
}

template <typename T> void ZeroPublisher::appendToQueue(const std::string &id, const T &paylod)
{
    alise::PackedMessage packedMessage;
    packedMessage.mutable_content()->PackFrom(paylod);
    std::string serialized_update;
    packedMessage.SerializeToString(&serialized_update);
    //  qDebug() << paylod.DebugString().c_str();
    _mutex.lock();
    _queue.push({ id, serialized_update });
    _mutex.unlock();
    _waiter.wakeOne();
}

void ZeroPublisher::publishTime(const timespec time)
{
    google::protobuf::Timestamp protoTime;
    protoTime.set_seconds(time.tv_sec);
    protoTime.set_nanos(time.tv_nsec);
    appendToQueue(admin, protoTime);
}

void ZeroPublisher::publishPowerStatus(const AVTUK_14::Main powerStatus)
{
    alise::PowerStatus protoPower;
    protoPower.set_pwrin(powerStatus.PWRIN);
    appendToQueue(sonica, protoPower);
}

void ZeroPublisher::publishBlock(const DataTypes::BlockStruct blk)
{
    switch (blk.data.size())
    {
    case sizeof(AVTUK_14::Main):
    {
        AVTUK_14::Main powerStatus;
        memcpy(&powerStatus, blk.data.data(), sizeof(powerStatus));
        publishPowerStatus(powerStatus);
    }
    }
}

void ZeroPublisher::publishHello(const QString id, const quint32 code)
{
    alise::HelloReply helloReply;
    helloReply.set_message(code + 1);
    appendToQueue(id.toStdString(), helloReply);
}

void ZeroPublisher::publishNtpStatus(bool status)
{
    alise::NtpStatus ntpStatus;
    ntpStatus.set_isntpenabled(status);
    appendToQueue(admin, ntpStatus);
}

void ZeroPublisher::send(itemType &str)
{
    zmq::message_t identity(str.first);
    zmq::message_t msg(str.second);
    qInfo() << "Send message to: {" << str.first.c_str() << "}, with payload: {" << str.second.c_str() << "}";
    _worker.send(identity, zmq::send_flags::sndmore);
    _worker.send(msg, zmq::send_flags::none);
}

void ZeroPublisher::checkQueue()
{
    while (!_queue.empty())
    {
        send(_queue.back());
        _queue.pop();
    }
}
