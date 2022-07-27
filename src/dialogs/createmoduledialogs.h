#ifndef CREATEMODULEDIALOGS_H
#define CREATEMODULEDIALOGS_H

#include "../module/modulesettings.h"
#include "../s2/configv.h"
#include "../widgets/udialog.h"

#include <QObject>

class CreateModuleDialogs
{
public:
    CreateModuleDialogs(const ModuleSettings &settings);

    QList<UDialog *> CreateDialogs();

private:
    ModuleSettings m_settings;
    QList<UDialog *> m_dialogs;
    ConfigV m_configV;

    void DeleteDialogs();
    void CreateConfigDialogs();
    void CreateCheckDialogs();
    void CreateTuneDialogs();
    void CreateSpecificDialogs();
    void CreateCommonDialogs();
    void addDialogToList(UDialog *dlg, const QString &caption, const QString &name);
};

#endif // CREATEMODULEDIALOGS_H
