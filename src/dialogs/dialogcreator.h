#pragma once

#include "../module/board.h"
#include "../module/modules.h"
#include "../module/modulesettings.h"
#include "../module/s2requestservice.h"
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

/// \brief Class for creating and managing dialogs.
class DialogCreator : public QObject
{
    Q_OBJECT
public:
    DialogCreator(const ModuleSettings &settings, const Board &board, //
        S2DataManager &s2DataManager, S2RequestService &s2ReqService, QWidget *parent = nullptr);
    /// \brief Creating the list with dialogs based on module settings for the current connected device.
    void createDialogs(const AppConfiguration appCfg);
    /// \brief Removing all dialogs in the dialog list.
    void deleteDialogs();
    /// \brief Returns the list with dialogs.
    QList<UDialog *> &getDialogs();

private:
    const ModuleSettings &m_settings;
    const Board &m_board;
    S2DataManager &m_s2manager;
    S2RequestService &m_s2service;
    QWidget *m_parent;
    QList<UDialog *> m_dialogs;

    /// \brief Returns true if module in box, otherwise false.
    bool isBoxModule(const quint16 &type) const;
    /// \brief Adding the created dialog to the list for saving.
    void addDialogToList(UDialog *dlg, const QString &caption, const QString &name);
    /// \brief Creating config dialogs.
    void createConfigDialogs();
    /// \brief Creating check dialogs.
    void createCheckDialogs();
    /// \brief Creating tune dialogs for KIV, KTF and KDV.
    void createBoxTuneDialogs(const Modules::Model boxModel);
    /// \brief Creating journal dialog.
    void createJournalDialog();
    /// \brief Creating startup dialog for KIV, KTF and KDV.
    void createStartupDialog(const Modules::Model boxModel);
    /// \brief Creating tune dialogs for two-part modules.
    void createTwoPartTuneDialogs(const Modules::BaseBoard &typeb, const Modules::MezzanineBoard &typem);
    /// \brief Creating oscillogram and switch journal dialogs.
    void createOscAndSwJourDialogs(const Modules::BaseBoard &typeb, const Modules::MezzanineBoard &typem);
    /// \brief Creating specific dialogs (tune, journal and startup).
    void createSpecificDialogs(const AppConfiguration appCfg);
    /// \brief Creating common dialogs (all modules).
    void createCommonDialogs(const AppConfiguration appCfg);
};
