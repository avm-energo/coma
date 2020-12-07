#ifndef MODULEFABRIC_H
#define MODULEFABRIC_H

#include "../widgets/alarmstateallwidget.h"
#include "../widgets/alarmwidget.h"
#include "../widgets/udialog.h"
#include "alarm.h"

class Module : public QObject
{
    Q_OBJECT
public:
    enum DialogTypes
    {
        CHECK = 0,
        CONF = 1,
        TUNE = 2
    };

    explicit Module(QObject *parent = nullptr);

    static Module *createModule(QTimer *updateTimer, BaseInterface *iface, AlarmWidget *aw);
    QList<UDialog *> dialogs();
    QList<UDialog *> confDialogs();
    void addDialogToList(UDialog *dlg, const QString &caption = "", const QString &name = "");
    BaseAlarm *getAlarm();
    BaseAlarm *getWarn();
    AlarmStateAll *getAlarmStateAll();
    void startTimeTimer();
    void stopTimeTimer();
    void parentTWTabChanged(int index);
    //    void setDefConf();
    void closeDialogs();

signals:

public slots:

private:
    QList<UDialog *> m_dialogs;
    BaseAlarm *m_accAlarm;
    AlarmStateAll *m_alarmStateAllDialog;
    BaseAlarm *m_warnAlarm;
    int m_currentTabIndex, m_oldTabIndex;
    BaseInterface *m_iface;
};

#endif // MODULEFABRIC_H
