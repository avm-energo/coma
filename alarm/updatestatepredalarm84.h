#ifndef UPDATESTATEPREDALARM84_H
#define UPDATESTATEPREDALARM84_H

#include "../alarm/abstractalarm.h"

class UpdateStatePredAlarm84 : public AbstractAlarm
{
    Q_OBJECT

public:
    explicit  UpdateStatePredAlarm84(QWidget *parent = nullptr);


signals:
    void SetPredAlarmColor(quint8*);
    void SetAlarmColor(quint8*);


public slots:

    void AlarmState();
    void PredAlarmState();
    void AvarState();

    void UpdateUSB();
    void USBSetAlarms();

    void UpdatePredAlarmEvents(IEC104Thread::SponSignals *);
    void UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *);

    void ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal);
    void ModBusUpdatePredAlarmEvents(ModBus::Coils Signal);

private:

    QWidget *Wpred;
    QWidget *Walarm;
};

#endif // UPDATESTATEPREDALARM84_H
