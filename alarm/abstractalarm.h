#ifndef ABSTRACTALARM_H
#define ABSTRACTALARM_H

#define NotUA 3011
#define NotUB 3012
#define NotUC 3013
#define NotIA 3014
#define NotIB 3015
#define NotIC 3016
#define BegDat 3017
#define LowUA 3018
#define LowUB 3019
#define LowUC 3020
#define SignTA 3021
#define SignTB 3022
#define SignTC 3023
#define AlarmTA 3024
#define AlarmTB 3025
#define AlarmTC 3026
#define SignCA 3027
#define SignCB 3028
#define SignCC 3029
#define AlarmCA 3030
#define AlarmCB 3031
#define AlarmCC 3032
#define PaspDat 3033
#define SignInb 3034
#define AlarmInb 3035

#define NOTUA 5011
#define NOTUB 5012
#define NOTUC 5013
#define NOTIA 5014
#define NOTIB 5015
#define NOTIC 5016
#define OvcA 5017
#define OvcB 5018
#define OvcC 5019
#define MachON 5020
#define SignT 5021
#define SignDat 5022
#define AlarmT 5023
#define SignIStart 5024
#define AlarmIStart 5025

#include <QDialog>
#include <QByteArray>
#include "../iec104/iec104.h"
#include "../iec104/ethernet.h"
#include "../modbus/modbus.h"
#include "../gen/modulebsi.h"
#include "alarmclass.h"




class AbstractAlarm : public QWidget
{
        Q_OBJECT
public:
    AbstractAlarm(QWidget *parent = nullptr);

    AlarmClass *Alarm;

    const quint32 PredBSIMask = 0x00005F55;
    const quint32 AvarBSIMask = 0x000020AA;
\
   // QList <bool> WarnAlarmEvents;
  //  QList <bool> AvarAlarmEvents;

    quint8 PredAlarmEvents[20];
    quint8 AvarAlarmEvents[20];

    quint8 PredAlarmEventsKTF[15];
    quint8 AvarAlarmEventsKTF[15];

    struct Bd11
    {
        quint32 dev;
        quint32 predAlarm;
        quint32 alarm;
    };

    struct Bd16
    {
        quint32 Prib;
        quint32 Warn;
        quint32 Alarm;
    };

    QWidget *Wpred;
    QWidget *Walarm;


public slots:

    virtual void AlarmState()=0;
    virtual  void WarnAlarmState()=0;
    virtual void AvarState()=0;

    virtual void Update(QList<bool>)=0;


private:

};

#endif // ABSTRACTALARM_H
