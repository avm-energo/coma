#ifndef MODULEFABRIC_H
#define MODULEFABRIC_H

//#include "../check/abstractcheckdialog.h"
//#include "../config/abstractconfdialog.h"
#include "../gen/board.h"
#include "../gen/udialog.h"
#include "alarm.h"
#include "warn.h"

#include <QObject>

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

    static Module *createModule(QTimer *updateTimer);
    QList<UDialog *> dialogs();
    QList<UDialog *> confDialogs();
    void addDialogToList(UDialog *dlg, const QString &caption = "", const QString &name = "");
    Alarm *getAlarm();
    Warn *getWarn();
    void startTimeTimer();
    void stopTimeTimer();
    void parentTWTabClicked(int index);
    void setDefConf();
    void closeDialogs();

signals:

public slots:

private:
    QList<UDialog *> m_Dialogs;
    Alarm *m_Alarm;
    Warn *m_Warn;
    int m_currentTabIndex, m_oldTabIndex;
};

#endif // MODULEFABRIC_H
