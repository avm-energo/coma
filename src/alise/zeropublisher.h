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

constexpr char booter[] = "sb";
constexpr char sonica[] = "sc";

// template <typename T> void appendToQueue(std::string &&id, const T &paylod);

class ZeroPublisher : public QObject
{
    //   template <typename T> friend void appendToQueue(std::string &&id, const T &paylod);

    Q_OBJECT
public:
    using itemType = std::pair<std::string, std::string>;
    using queueType = std::queue<itemType>;

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
    void publishHello(const std::string id, const uint32_t code);

private:
    zmq::context_t &_ctx;
    zmq::socket_t _worker;
    QMutex _mutex;
    QWaitCondition _waiter;
    bool is_active = true;
    queueType _queue;

    void send(itemType &str);
    void checkQueue();

    template <typename T> void appendToQueue(const std::string &id, const T &paylod);
};
