#ifndef ALARMCLASS_H
#define ALARMCLASS_H

#define WARNEVENTS 20
#define ALARMEVENTS 20

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../module/module.h"
#include "../widgets/wd_func.h"

#include <QByteArray>
#include <QDialog>
#include <QMap>

class AlarmClass : public QObject
{
    Q_OBJECT

public:
    explicit AlarmClass(QObject *parent = nullptr);
    void setModule(Module *m);

    //    int warnalarmcount = 0, alarmcount = 0;

    //    QList<bool> WarnAlarmEvents;
    //    QList<bool> AvarAlarmEvents;
    // QList<bool> IndicatorEvents;
    //    QList<bool> AlarmEvents;

    //    struct StAlarm
    //    {
    //        int warnCounts;
    //        int avarCounts;
    //        int BdNumbers;
    //        quint32 AdrAlarm;

    //        QList<bool> warns;
    //        QList<bool> avars;
    //    };

    //    QMap<quint16, StAlarm> MapAlarm;

    struct BdAlarm
    {
        quint32 Prib;
        quint32 Warn;
        quint32 Alarm;
    };

signals:
    //    void SetWarnAlarmColor(QList<bool>);
    //    void SetAlarmColor(QList<bool>);
    void setWarnColor(bool);
    void setAlarmColor(bool);
    void SetIndicator(bool);
    //    void AlarmColor(QList<bool>);
    void SetFirstButton();

public slots:
    void UpdateAlarmUSB();
    void UpdateAlarmModBus();
    void UpdateAlarm104();
    void update();

private:
    Module *m_Module;
    bool m_moduleIsSet;
};

#endif // ALARMCLASS_H
