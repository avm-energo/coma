#pragma once

#include "../module/modules.h"
#include "../module/modulesettings.h"
#include "../s2/configv.h"
#include "../widgets/alarmwidget.h"
#include "../widgets/udialog.h"

#include <QObject>

/// \brief Enumeration for application configuration.
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
    void deleteDialogs();
    void parentTWTabChanged(int index);

private:
    const ModuleSettings &settings;
    QWidget *mParent;
    QList<UDialog *> mDialogs;
    ConfigV configV;

    void addDialogToList(UDialog *dlg, const QString &caption, const QString &name);
    void createConfigDialogs();
    void createCheckDialogs();
    void createBoxTuneDialogs(const Modules::Model &boxModel);
    void createJournalAndStartupDialogs(const Modules::Model &boxModel);
    void createTwoPartTuneDialogs(const Modules::BaseBoard &typeb, const Modules::MezzanineBoard &typem);
    void createOscAndSwJourDialogs(const Modules::BaseBoard &typeb, const Modules::MezzanineBoard &typem);
    void createSpecificDialogs(const AppConfiguration &appCfg);
    void createCommonDialogs(const AppConfiguration &appCfg);
};
