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

class AlarmWidget : public QDialog
{
    Q_OBJECT
public:
   explicit  AlarmWidget(QWidget *parent = nullptr);

    QMap<quint16,int> warnCounts;
    QMap<quint16,int> avarCounts;
    QMap<quint16,int> BdNumbers;

    struct BdAlarm
    {
        quint32 Prib;
        quint32 Warn;
        quint32 Alarm;
    };


  quint8 WarnEvents[20];
  quint8 AvarAlarmEvents[20];

  quint8 WarnEventsKTF[15];
  quint8 AvarAlarmEventsKTF[15];

  const quint32 WarnBSIMask = 0x00005F55;
  const quint32 AvarBSIMask = 0x000020AA;

signals:
    void SetWarnAlarmColor(quint8*);
    void SetAlarmColor(quint8*);
    void AlarmButtonPressed();
    void ModuleWarnButtonPressed();
    void ModuleAlarmButtonPressed();

public slots:

     void UpdateUSB();
     void USBSetAlarms();

     void UpdateWarningEvents(IEC104Thread::SponSignals *);
     void UpdateStateWarningEvents(IEC104Thread::SponSignals *);

     void ModbusUpdateStateWarningEvents(ModBus::Coils Signal);
     void ModBusUpdateWarningEvents(ModBus::Coils Signal);




};

#endif // ALARMWIDGET_H
