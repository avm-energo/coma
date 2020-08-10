#ifndef ALARMCLASS_H
#define ALARMCLASS_H

#define WARNEVENTS 20
#define ALARMEVENTS 20

#define MTYPE_KTF   0xA287
#define MTYPE_KIV   0xA284

#define WARNBSIMASK 0x00005F55
#define AVARBSIMASK 0x000020AA

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

class AlarmClass : public QObject
{
    Q_OBJECT

public:
    explicit  AlarmClass(QObject *parent = nullptr);

    QMap<quint16,int> warnCounts;
    QMap<quint16,int> avarCounts;
    QMap<quint16,int> BdNumbers;

    const quint32 WarnBSIMask = 0x00005F55;
    const quint32 AvarBSIMask = 0x000020AA;

    QList <bool> WarnAlarmEvents;
    QList <bool> AvarAlarmEvents;

   int  warnalarmcount = 0, alarmcount = 0;


    struct BdAlarm
    {
        quint32 Prib;
        quint32 Warn;
        quint32 Alarm;
    };

signals:
    void SetWarnAlarmColor(QList <bool>);
    void SetAlarmColor(QList <bool>);
    void SetFirstButton();


public slots:
     void UpdateAlarmUSB();


};

#endif // ALARMCLASS_H
