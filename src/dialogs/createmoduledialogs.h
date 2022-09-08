#ifndef CREATEMODULEDIALOGS_H
#define CREATEMODULEDIALOGS_H

#include "../module/modulesettings.h"
#include "../s2/configv.h"
#include "../widgets/alarmwidget.h"
#include "../widgets/udialog.h"

#include <QObject>

class CreateModuleDialogs : public QObject
{
    Q_OBJECT
public:
    CreateModuleDialogs(const ModuleSettings &settings, QObject *parent = nullptr);

    const QList<UDialog *> &createDialogs();
    void createAlarms(AlarmWidget *alarmWidget);

private:
    const ModuleSettings &settings;
    QList<UDialog *> dialogs;
    ConfigV configV;

    void addDialogToList(UDialog *dlg, const QString &caption, const QString &name);
    void deleteDialogs();
    void createConfigDialogs();
    void createCheckDialogs();
    void createTuneDialogs();
    void createSpecificDialogs();
    void createCommonDialogs();
};

#endif // CREATEMODULEDIALOGS_H
