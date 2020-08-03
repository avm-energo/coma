#ifndef ALARM84_H
#define ALARM84_H

#include "../alarm/abstractalarm.h"

class Alarm84: public AbstractAlarm
{
     Q_OBJECT

public:
  explicit  Alarm84(QWidget *parent = nullptr);




signals:
     void SetPredAlarmColor(quint8*);
     void SetAlarmColor(quint8*);


public slots:
     void UpdatePredAlarmEvents(IEC104Thread::SponSignals *);
     void UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *);
     void DeviceState();
     void PredAlarmState();
     void AlarmState();
     void UpdateUSB();
     void USBSetAlarms();
     void ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal);
     void ModBusUpdatePredAlarmEvents(ModBus::Coils Signal);

private:
     quint8 PredAlarmEvents[20];
     quint8 AlarmEvents[20];
     QWidget *Wpred;
     QWidget *Walarm;
     EAbstractCheckDialog *CheckB, *CheckM;
     QTimer* BdaTimer, *TimeTimer, *AlarmStateTimer;

     void AlmNewTimers();



};

#endif // ALARM84_H
