#pragma once
#include "../gen/modules.h"
#include "grpc_proto.grpc.pb.h"
#include "timesyncronizer.h"

#include <QObject>
#include <QTimer>
class Protocom;

class StmBroker : public QObject
{
    //    Q_OBJECT
public:
    StmBroker(QObject *parent = nullptr);
    bool connectToStm();

public /*slots*/:
    void checkPowerUnit();
    void setIndication(alise::Health_Code code);

    void setTime(alise::UnixTimestamp time);
    void getTime(alise::UnixTimestamp &time);

private:
    AVTUK_14::Indication transform(alise::Health_Code code) const;
    timespec transform(alise::UnixTimestamp timestamp) const
    {
        return timespec { timestamp.sec(), timestamp.nsec() };
    }
    // Controller m_controller;
    Protocom *m_interface;
    QTimer m_timer;
    QTimer m_testTimer;
    TimeSyncronizer timeSync;
};
