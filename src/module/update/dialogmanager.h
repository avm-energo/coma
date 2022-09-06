#ifndef DIALOGMANAGER_H
#define DIALOGMANAGER_H

#include "../dialogs/journalsdialog.h"
#include "../widgets/alarmwidget.h"
#include "../widgets/udialog.h"

#include <QObject>

class DialogManager : QObject
{
    Q_OBJECT
private:
    QList<UDialog *> m_dialogs;

public:
    DialogManager(QObject *parent = nullptr);

    virtual void create(QTimer *updateTimer) = 0;
    virtual void create(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM) = 0;
    virtual void createModule(Modules::Model model) = 0;

    virtual void createAlarm(AlarmWidget *aw);
    void create(UniquePointer<Journals> jour);
    void createCommon();

    QList<UDialog *> dialogs();
    void addDialogToList(UDialog *dlg, const QString &caption = "", const QString &name = "");
    void startTimeTimer();
    void stopTimeTimer();
    void parentTWTabChanged(int index);
    void closeDialogs();
};

#endif // DIALOGMANAGER_H
