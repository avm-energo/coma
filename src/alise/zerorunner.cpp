#include "zerorunner.h"

#include "../gen/datamanager.h"
#include "../gen/stdfunc.h"
#include "timesyncronizer.h"

#include <QDebug>
#include <QMutexLocker>
#include <fstream>
#include <iostream>
#include <thread>
namespace runner
{
bool ZeroRunner::runServer()
{

    frontend_.bind("tcp://*:5555");
    backendSub_.bind("inproc://backendSub");
    backendPub_.bind("inproc://backendPub");
    connect(this, &ZeroRunner::timeReceived, this, &ZeroRunner::publishTime, Qt::DirectConnection);

    auto subscriber = std::unique_ptr<std::thread>(new std::thread([&] {
        auto new_worker = UniquePointer<ZeroSubscriber>(new ZeroSubscriber(ctx_, ZMQ_DEALER));

        connect(new_worker.get(), &ZeroSubscriber::timeReceived, this, &ZeroRunner::timeReceived, Qt::DirectConnection);
        // connect(new_worker.get(), &ZeroSubscriber::timeReceived, this, &ZeroRunner::publishTime,
        // Qt::DirectConnection);
        connect(
            new_worker.get(), &ZeroSubscriber::healthReceived, this, &ZeroRunner::healthReceived, Qt::DirectConnection);
        auto timeSync = UniquePointer<TimeSyncronizer>(new TimeSyncronizer);
        if (timeSync->isNtpSync())
            std::cout << "Ntp enabled";
        connect(new_worker.get(), &ZeroSubscriber::timeReceived, timeSync.get(), &TimeSyncronizer::handleTime);
        new_worker->work();
    }));

    auto publisher = std::unique_ptr<std::thread>(new std::thread([&] {
        auto new_worker = UniquePointer<ZeroPublisher>(new ZeroPublisher(ctx_, ZMQ_DEALER));

        connect(this, &ZeroRunner::publishTime, new_worker.get(), &ZeroPublisher::publishTime, Qt::DirectConnection);
        // connect(this, &ZeroRunner::publishPowerStatus, new_worker.get(), &ZeroPublisher::publishPowerStatus,
        //  Qt::DirectConnection);
        const auto &manager = DataManager::GetInstance();
        connect(
            &manager, &DataManager::timeReceived, new_worker.get(), &ZeroPublisher::publishTime, Qt::DirectConnection);
        connect(&manager, &DataManager::blockReceived, new_worker.get(), &ZeroPublisher::publishBlock,
            Qt::DirectConnection);
        new_worker->work();
    }));

    publisher->detach();
    subscriber->detach();

    //  proxy(frontend_, backendSub_, backendPub_);
    zmq_pollitem_t items[] = {
        { frontend_, 0, ZMQ_POLLIN, 0 },   //
        { backendPub_, 0, ZMQ_POLLIN, 0 }, //
        { backendSub_, 0, ZMQ_POLLIN, 0 }  //
    };
    try
    {
        while (1)
        {
            zmq::message_t message;
            int more; //  Multipart detection

            zmq::poll(&items[0], 2, -1);

            if (items[0].revents & ZMQ_POLLIN)
            {
                while (1)
                {
                    //  Process all parts of the message
                    auto len = frontend_.recv(message, zmq::recv_flags::none);
                    more = frontend_.get(zmq::sockopt::rcvmore);
                    backendSub_.send(message, more ? zmq::send_flags::sndmore : zmq::send_flags::none);

                    if (!more)
                        break; //  Last message part
                }
            }
            if (items[1].revents & ZMQ_POLLIN)
            {
                while (1)
                {
                    //  Process all parts of the message
                    auto len = backendPub_.recv(message);
                    more = backendPub_.get(zmq::sockopt::rcvmore);
                    frontend_.send(message, more ? zmq::send_flags::sndmore : zmq::send_flags::none);

                    if (!more)
                        break; //  Last message part
                }
            }
        }

    } catch (std::exception &e)
    {
        qDebug() << "Exception: " << e.what();
        return false;
    }
    frontend_.close();
    backendPub_.close();
    backendSub_.close();
    ctx_.close();
    return true;
}

void ZeroRunner::stopServer()
{
}

}
