#ifndef USBSETALARM84_H
#define USBSETALARM84_H

#include "../alarm/abstractalarm.h"

class USBSetAlarm84 : public AbstractAlarm
{
    Q_OBJECT

public:
    explicit  USBSetAlarm84(QWidget *parent = nullptr);


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

#endif // USBSETALARM84_H
