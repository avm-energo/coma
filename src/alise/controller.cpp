#include "controller.h"

Controller::Controller(QObject *parent) noexcept : Controller(50051, "0.0.0.0", parent)
{
}

Controller::Controller(uint16_t port, std::string addr, QObject *parent) noexcept
    : QObject(parent), worker(new runner::ServerRunner(port, addr)), m_stmBroker({})
{
    worker->moveToThread(&workerThread);
    connect(
        worker, &runner::ServerRunner::healthChanged, this,
        [](auto health) { std::cout << health.SerializeAsString(); }, Qt::DirectConnection);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater, Qt::QueuedConnection);
    connect(&workerThread, &QThread::started, worker, &runner::ServerRunner::runServer, Qt::QueuedConnection);
    connect(
        worker, &runner::ServerRunner::healthChanged, &m_stmBroker,
        [&](const auto health) { m_stmBroker.setIndication(health.code()); }, Qt::QueuedConnection);
    connect(
        worker, &runner::ServerRunner::requestTime, &m_stmBroker,
        [&](alise::UnixTimestamp &timestamp) { m_stmBroker.getTime(timestamp); }, Qt::DirectConnection);
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
    QMetaObject::invokeMethod(worker, &runner::ServerRunner::stopServer, Qt::DirectConnection);
}
