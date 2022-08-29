#ifndef DIALOGMANAGER_H
#define DIALOGMANAGER_H

#include <QObject>

//#include "../widgets/alarmwidget.h"
//#include "../widgets/udialog.h"
//#include "../comaversion/comaversion.h"
//#include "../ctti/type_id.hpp"
//#include "../dialogs/fwuploaddialog.h"
//#include "../dialogs/infodialog.h"
//#include "../dialogs/journalsdialog.h"
//#include "../dialogs/timedialog.h"
//#include "../widgets/udialog.h"
//#include "../module/alarmstateall.h"
//#include "../s2/configv.h"

class DialogManager : QObject
{
    Q_OBJECT
public:
    DialogManager(QObject *parent = nullptr);

    /*
    virtual void createAlarm(AlarmWidget *aw);
    virtual void create(QTimer *updateTimer)
    {
        Q_UNUSED(updateTimer);
    };
    void create(UniquePointer<Journals> jour);
    void createCommon();
    virtual void createModule(Modules::Model model)
    {
        Q_UNUSED(model);
    };
    virtual void create(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM)
    {
        Q_UNUSED(typeB);
        Q_UNUSED(typeM);
    };

    QList<UDialog *> dialogs();
    void addDialogToList(UDialog *dlg, const QString &caption = "", const QString &name = "");
    void startTimeTimer();
    void stopTimeTimer();
    void parentTWTabChanged(int index);
    void closeDialogs();
    */
};

#endif // DIALOGMANAGER_H
