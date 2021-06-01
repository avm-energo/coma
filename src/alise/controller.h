#pragma once
#include "serverrunner.h"
#include "stmbroker.h"
#include "timesyncronizer.h"

#include <QObject>
#include <QThread>

class Controller : public QObject
{
public:
    explicit Controller(QObject *parent = nullptr) noexcept;
    explicit Controller(uint16_t port, std::string addr, QObject *parent = nullptr) noexcept;

    ~Controller();

    bool launch();
    void shutdown();
signals:

private:
    runner::ServerRunner *worker;
    StmBroker m_stmBroker;
    QThread workerThread;
};
