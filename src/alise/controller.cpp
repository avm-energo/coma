#include "controller.h"

#include "../gen/datamanager.h"
#include "../gen/stdfunc.h"

#include <iostream>
Controller::Controller(QObject *parent) noexcept : Controller("0.0.0.0", parent)
{
}

Controller::Controller(std::string addr, QObject *parent) noexcept
    : QObject(parent), worker(new runner::ZeroRunner()), m_stmBroker({})
{

    worker->moveToThread(&workerThread);

    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater, Qt::QueuedConnection);
    connect(&workerThread, &QThread::started, worker, &runner::ZeroRunner::runServer, Qt::QueuedConnection);
    connect(worker, &runner::ZeroRunner::healthReceived, &m_stmBroker,
        &StmBroker::setIndication /*, Qt::DirectConnection*/);
    connect(worker, &runner::ZeroRunner::timeReceived, &m_stmBroker, &StmBroker::setTime /*, Qt::DirectConnection*/);
    connect(worker, &runner::ZeroRunner::timeRequest, &m_stmBroker, &StmBroker::getTime /*, Qt::DirectConnection*/);
    const auto &manager = DataManager::GetInstance();
    connect(&manager, &DataManager::blockReceived, &recovery, &Recovery::receiveBlock /*, Qt::DirectConnection*/);

    // NOTE avtuk will be rebooted
    connect(&recovery, &Recovery::rebootReq, &m_stmBroker, &StmBroker::rebootMyself);

    connect(&timeSync, &TimeSyncronizer::ntpStatusChanged, worker, &runner::ZeroRunner::publishNtpStatus,
        Qt::DirectConnection);
}

Controller::~Controller()
{
    workerThread.quit();
    workerThread.wait();
}

bool Controller::launch()
{
    if (!m_stmBroker.connectToStm())
    {
        delete worker;
        return false;
    }
    workerThread.start();
    return true;
}

void Controller::shutdown()
{
    QMetaObject::invokeMethod(worker, &runner::ZeroRunner::stopServer, Qt::DirectConnection);
}
