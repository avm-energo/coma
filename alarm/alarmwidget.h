#ifndef ALARMWIDGET_H
#define ALARMWIDGET_H

#include <QDialog>
#include <QByteArray>
#include <QMap>
#include "../iec104/iec104.h"
#include "../iec104/ethernet.h"
#include "../modbus/modbus.h"
#include "../gen/modulebsi.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/commands.h"
#include "../widgets/wd_func.h"
#include "../alarm/alarmclass.h"

class AlarmWidget : public QWidget
{
    Q_OBJECT
public:
   explicit  AlarmWidget(AlarmClass *alarm, QWidget *parent = nullptr);


signals:
    void SetWarnAlarmColor(QList <bool>);
    void SetAlarmColor(QList <bool>);
    void AlarmButtonPressed();
    void ModuleWarnButtonPressed();
    void ModuleAlarmButtonPressed();

public slots:


     void UpdateFirstUSB();
     void UpdateSecondUSB(QList<bool>);
     void UpdateThirdUSB(QList<bool>);

     void USBSetAlarms();

     void UpdateWarningEvents(IEC104Thread::SponSignals *);
     void UpdateStateWarningEvents(IEC104Thread::SponSignals *);

     void ModbusUpdateStateWarningEvents(ModBus::Coils Signal);
     void ModBusUpdateWarningEvents(ModBus::Coils Signal);

private:
    AlarmClass *Alarm;

};

#endif // ALARMWIDGET_H
