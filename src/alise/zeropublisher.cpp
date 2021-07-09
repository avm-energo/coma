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

void ZeroPublisher::publishTime(const timespec time)
{

    google::protobuf::Timestamp protoTime;
    protoTime.set_seconds(time.tv_sec);
    protoTime.set_nanos(time.tv_nsec);
    alise::PackedMessage packedMessage;
    packedMessage.mutable_content()->PackFrom(protoTime);
    std::string serialized_update;
    packedMessage.SerializeToString(&serialized_update);
    qDebug() << protoTime.DebugString().c_str();
    _mutex.lock();
    _queue.push(serialized_update);
    _mutex.unlock();
    _waiter.wakeOne();
}

void ZeroPublisher::publishPowerStatus(const AVTUK_14::DiscretePowerSignals powerStatus)
{
    alise::PowerStatus protoPower;
    protoPower.set_pwrin(powerStatus.PWRIN);
    alise::PackedMessage packedMessage;
    packedMessage.mutable_content()->PackFrom(protoPower);
    std::string serialized_update;
    packedMessage.SerializeToString(&serialized_update);
    qDebug() << protoPower.DebugString().c_str();
    _mutex.lock();
    _queue.push(serialized_update);
    _mutex.unlock();
    _waiter.wakeOne();
}

void ZeroPublisher::publishBlock(const DataTypes::BlockStruct blk)
{
    switch (blk.data.size())
    {
    case sizeof(AVTUK_14::DiscretePowerSignals):
    {
        AVTUK_14::DiscretePowerSignals powerStatus;
        memcpy(&powerStatus.PWRIN, blk.data.data(), sizeof(powerStatus.PWRIN));
        publishPowerStatus(powerStatus);
    }
    }
}
