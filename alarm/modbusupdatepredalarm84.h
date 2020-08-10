#ifndef MODBUSUPDATEPREDALARM84_H
#define MODBUSUPDATEPREDALARM84_H

#include "../alarm/abstractalarm.h"

class ModBusUpdatePredAlarm84 : public AbstractAlarm
{
    Q_OBJECT

 public:
    explicit  ModBusUpdatePredAlarm84(QWidget *parent = nullptr);


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

#endif // MODBUSUPDATEPREDALARM84_H
