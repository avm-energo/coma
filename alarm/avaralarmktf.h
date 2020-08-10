#ifndef AVARALARMKTF_H
#define AVARALARMKTF_H

#include "../alarm/abstractalarm.h"

class AvarAlarmKTF : public AbstractAlarm
{
    Q_OBJECT

 public:
    explicit  AvarAlarmKTF(AlarmClass *alarm,QWidget *parent = nullptr);


 public slots:

    void AlarmState();
    void PredAlarmState();
    void AvarState();

    void Update(QList<bool> states);

    void UpdatePredAlarmEvents(IEC104Thread::SponSignals *);
    void UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *);

    void ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal);
    void ModBusUpdatePredAlarmEvents(ModBus::Coils Signal);

 private:

};

#endif // AVARALARMKTF_H
