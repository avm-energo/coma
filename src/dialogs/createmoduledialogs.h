#ifndef DIALOGMANAGER_H
#define DIALOGMANAGER_H

#include "../module/modulesettings.h"
#include "../s2/configv.h"
#include "../widgets/alarmwidget.h"
#include "../widgets/udialog.h"

#include <QObject>

enum AppConfiguration : bool
{
    Debug = false,
    Service = true
};

class DialogManager : public QObject
{
    Q_OBJECT
public:
    DialogManager(const ModuleSettings &settings, QWidget *parent = nullptr);

    const QList<UDialog *> &createDialogs(const AppConfiguration &appCfg);
    void createAlarms(AlarmWidget *alarmWidget);
    void deleteDialogs();

private:
    const ModuleSettings &settings;
    QWidget *mParent;
    QList<UDialog *> mDialogs;
    ConfigV configV;

    void addDialogToList(UDialog *dlg, const QString &caption, const QString &name);
    void createConfigDialogs();
    void createCheckDialogs();
    void createTuneDialogs(const bool &isBoxModule);
    void createSpecificDialogs(const bool &isBoxModule);
    void createCommonDialogs();
};

#endif // DIALOGMANAGER_H
