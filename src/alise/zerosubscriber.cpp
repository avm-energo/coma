#include "zerosubscriber.h"

#include "../gen/error.h"

#include <QCoreApplication>
#include <QDebug>
#include <QString>
ZeroSubscriber::ZeroSubscriber(zmq::context_t &ctx, int sock_type, QObject *parent)
    : QObject(parent), _ctx(ctx), _worker(_ctx, sock_type)
{
}

void ZeroSubscriber::work()
{
    try
    {
        _worker.connect("inproc://backendSub");
        // zmq::pollitem_t items[] { { static_cast<void *>(_worker), 0, ZMQ_POLLIN, 0 } };
        while (is_active)
        {
            zmq::message_t identity;
            zmq::message_t msg;

            auto id = _worker.recv(identity);
            qDebug() << "Received id bytes: " << id.value();
            qDebug() << "Received id: " << identity.to_string().c_str();
            auto ms = _worker.recv(msg);
            qDebug() << "Received msg bytes: " << ms.value();
            std::string data(msg.to_string());
            qDebug() << "Received msg: " << data.c_str();
            alise::PackedMessage packedMessage;
            packedMessage.ParseFromString(data);
            const auto &messageContent = packedMessage.content();
            if (messageContent.Is<alise::Health>())
            {
                alise::Health protoHealth;
                if (!messageContent.UnpackTo(&protoHealth))
                {
                    qWarning() << Error::WriteError;
                    continue;
                }
                qInfo() << "Receive health:" << protoHealth.code();
                emit healthReceived(protoHealth.code());
            }
            else if (messageContent.Is<google::protobuf::Timestamp>())
            {
                google::protobuf::Timestamp protoTime;
                if (!messageContent.UnpackTo(&protoTime))
                {
                    qWarning() << Error::WriteError;
                    continue;
                }
                timespec unixTime;
                unixTime.tv_sec = protoTime.seconds();
                unixTime.tv_nsec = protoTime.nanos();
                qInfo() << "Receive time:" << unixTime.tv_sec << ":" << unixTime.tv_nsec;
                emit timeReceived(unixTime);
            }
            else if (messageContent.Is<alise::HelloRequest>())
            {
                alise::HelloRequest helloAlise;
                if (!messageContent.UnpackTo(&helloAlise))
                {
                    qWarning() << Error::WriteError;
                    continue;
                }
                qInfo() << "Someone said to Alise:" << helloAlise.message();
                emit helloReceived(QString::fromStdString(identity.to_string()), helloAlise.message());
            }
            else if (messageContent.Is<alise::TimeRequest>())
            {
                alise::TimeRequest payload;
                if (!messageContent.UnpackTo(&payload))
                {
                    qWarning() << Error::WriteError;
                    continue;
                }
                qInfo() << "TimeRequested";
                emit timeRequest();
            }
            else
            {
                qWarning() << Error::WrongType;
            }
            QCoreApplication::processEvents();
        }
        _worker.close();
    } catch (std::exception &e)
    {
        qInfo() << "Exception: " << e.what();
    }
}
