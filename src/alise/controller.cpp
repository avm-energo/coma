#include "controller.h"

#include "../gen/datamanager.h"
#include "../gen/stdfunc.h"

#include <iostream>
Controller::Controller(QObject *parent) noexcept : Controller("0.0.0.0", parent)
{
}

Controller::Controller(std::string addr, QObject *parent) noexcept
    : QObject(parent), worker(new runner::ZeroRunner(this)), m_stmBroker({})
{
    connect(worker, &runner::ZeroRunner::healthReceived, &m_stmBroker, &StmBroker::setIndication, Qt::DirectConnection);
    connect(worker, &runner::ZeroRunner::timeReceived, &m_stmBroker, &StmBroker::setTime, Qt::DirectConnection);
    connect(worker, &runner::ZeroRunner::timeReceived, &timeSync, &TimeSyncronizer::handleTime, Qt::DirectConnection);
    connect(worker, &runner::ZeroRunner::timeRequest, &m_stmBroker, &StmBroker::getTime, Qt::DirectConnection);
    const auto &manager = DataManager::GetInstance();
    connect(&manager, &DataManager::blockReceived, &recovery, &Recovery::receiveBlock, Qt::DirectConnection);

    // NOTE avtuk will be rebooted
    connect(&recovery, &Recovery::rebootReq, &m_stmBroker, &StmBroker::rebootMyself, Qt::DirectConnection);

    connect(&timeSync, &TimeSyncronizer::ntpStatusChanged, worker, &runner::ZeroRunner::publishNtpStatus/*,
        Qt::DirectConnection*/);
}

Controller::~Controller()
{
}

bool Controller::launch()
{
    if (!m_stmBroker.connectToStm())
    {
        delete worker;
        return false;
    }
    const auto &manager = DataManager::GetInstance();
    auto connectionTimeSync = std::shared_ptr<QMetaObject::Connection>(new QMetaObject::Connection);

    *connectionTimeSync = connect(
        &manager, &DataManager::timeReceived, &timeSync,
        [=, &syncer = timeSync](const timespec &time) {
            QObject::disconnect(*connectionTimeSync);
            syncer.handleTime(time);
        },
        Qt::DirectConnection);
    m_stmBroker.getTime();
    worker->runServer();

    return true;
}

void Controller::shutdown()
{
    QMetaObject::invokeMethod(worker, &runner::ZeroRunner::stopServer, Qt::DirectConnection);
}

void Controller::syncTime(const timespec &)
{
}
