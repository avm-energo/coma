#include "controller.h"

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
    connect(worker, &runner::ZeroRunner::healthReceived, &m_stmBroker, &StmBroker::setIndication, Qt::DirectConnection);
    connect(worker, &runner::ZeroRunner::timeReceived, &m_stmBroker, &StmBroker::setTime, Qt::DirectConnection);
}

Controller::~Controller()
{
    workerThread.quit();
    workerThread.wait();
}

bool Controller::launch()
{
    if (!m_stmBroker.connectToStm())
        return false;
    workerThread.start();
    return true;
}

void Controller::shutdown()
{
    QMetaObject::invokeMethod(worker, &runner::ZeroRunner::stopServer, Qt::DirectConnection);
}
