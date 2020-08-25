#ifndef ALARMCLASS_H
#define ALARMCLASS_H

#define WARNEVENTS 20
#define ALARMEVENTS 20

#define MTYPE_KTF 0xA287
#define MTYPE_KIV 0xA284

#define WARNBSIMASK 0x00005F55
#define AVARBSIMASK 0x000020AA

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../iec104/ethernet.h"
#include "../iec104/iec104.h"
#include "../modbus/modbus.h"
#include "../widgets/wd_func.h"

#include <QByteArray>
#include <QDialog>
#include <QMap>

class AlarmClass : public QObject
{
    Q_OBJECT

public:
    explicit AlarmClass(QObject *parent = nullptr);

    int warnalarmcount = 0, alarmcount = 0;

    const quint32 WarnBSIMask = 0x00005F55;
    const quint32 AvarBSIMask = 0x000020AA;

    QList<bool> WarnAlarmEvents;
    QList<bool> AvarAlarmEvents;
    QList<bool> AlarmEvents;

    struct StAlarm
    {
        int warnCounts;
        int avarCounts;
        int BdNumbers;
        quint32 AdrAlarm;

        QList<bool> warns;
        QList<bool> avars;
    };

    QMap<quint16, StAlarm> MapAlarm;

    struct BdAlarm
    {
        quint32 Prib;
        quint32 Warn;
        quint32 Alarm;
    };

signals:
    void SetWarnAlarmColor(QList<bool>);
    void SetAlarmColor(QList<bool>);
    void AlarmColor(QList<bool>);
    void SetFirstButton();

public slots:
    void UpdateAlarmUSB();
    void UpdateAlarmModBus(ModBus::Coils Signal);
    void UpdateAlarm104(IEC104Thread::SponSignals *Signal);
};

#endif // ALARMCLASS_H
