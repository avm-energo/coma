#pragma once

#include "../module/board.h"
#include "../module/modules.h"
#include "../module/modulesettings.h"
#include "../s2/s2datamanager.h"
#include "../widgets/alarmwidget.h"
#include "../widgets/udialog.h"

#include <QObject>

/// \brief Enumeration for application configuration.
enum class AppConfiguration : bool
{
    Debug = false,
    Service = true
};

/// \brief Class for creating dialogs.
class DialogCreator : public QObject
{
    Q_OBJECT
public:
    DialogCreator(const ModuleSettings &settings, const Board &board, //
        S2DataManager &s2DataManager, QWidget *parent = nullptr);
    void createDialogs(const AppConfiguration appCfg);
    void deleteDialogs();
    QList<UDialog *> &getDialogs();

private:
    const ModuleSettings &settings;
    const Board &board;
    S2DataManager &s2manager;
    QWidget *mParent;
    QList<UDialog *> mDialogs;

    bool isBoxModule(const quint16 &type) const;
    void addDialogToList(UDialog *dlg, const QString &caption, const QString &name);
    void createConfigDialogs();
    void createCheckDialogs();
    void createBoxTuneDialogs(const Modules::Model boxModel);
    void createJournalDialog();
    void createStartupDialog(const Modules::Model boxModel);
    void createTwoPartTuneDialogs(const Modules::BaseBoard &typeb, const Modules::MezzanineBoard &typem);
    void createOscAndSwJourDialogs(const Modules::BaseBoard &typeb, const Modules::MezzanineBoard &typem);
    void createSpecificDialogs(const AppConfiguration appCfg);
    void createCommonDialogs(const AppConfiguration appCfg);
};
