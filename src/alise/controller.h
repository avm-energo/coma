#pragma once
#include "recovery.h"
#include "stmbroker.h"
#include "timesyncronizer.h"
#include "zerorunner.h"

#include <QObject>
#include <QThread>

class Controller : public QObject
{
public:
    explicit Controller(QObject *parent = nullptr) noexcept;
    explicit Controller(std::string addr, QObject *parent = nullptr) noexcept;

    ~Controller() override;

    bool launch();
    void shutdown();
signals:

private:
    runner::ZeroRunner *worker;
    StmBroker m_stmBroker;
    TimeSyncronizer timeSync;
    QThread workerThread;
    Recovery recovery;
};
