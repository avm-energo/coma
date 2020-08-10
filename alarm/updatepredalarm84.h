#ifndef UPDATEPREDALARM84_H
#define UPDATEPREDALARM84_H

#include "../alarm/abstractalarm.h"

class UpdatePredAlarm84 : public AbstractAlarm

{
    Q_OBJECT

public:
    explicit  UpdatePredAlarm84(QWidget *parent = nullptr);


signals:
    void SetPredAlarmColor(quint8*);
    void SetAlarmColor(quint8*);


public slots:

    void AlarmState();
    void PredAlarmState();
    void AvarState();

    void UpdateUSB();
    void Update();

    void UpdatePredAlarmEvents(IEC104Thread::SponSignals *);
    void UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *);

    void ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal);
    void ModBusUpdatePredAlarmEvents(ModBus::Coils Signal);

private:


};

#endif // UPDATEPREDALARM84_H
