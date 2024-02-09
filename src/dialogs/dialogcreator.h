#pragma once

#include "../module/board.h"
#include "../module/modules.h"
#include "../module/modulesettings.h"
#include "../module/s2requestservice.h"
#include "../widgets/udialog.h"

#include <QObject>
#include <s2/s2datamanager.h>

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
private:
    Modules::Model m_boxModel;
    Modules::BaseBoard m_typeB;
    Modules::MezzanineBoard m_typeM;
    const ModuleSettings &m_settings;
    const Board &m_board;
    S2DataManager &m_s2manager;
    S2RequestService &m_s2service;
    QWidget *m_parent;
    QList<UDialog *> m_dialogs;

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
    /// \brief Adding the created dialog to the list for saving.
    void addDialogToList(UDialog *dlg, const QString &caption, const QString &name);
    /// \brief Updating creator's internal data.
    void updateTypes();
    /// \brief Creating config dialogs.
    void createConfigDialogs();
    /// \brief Creating check dialogs.
    void createCheckDialogs();
    /// \brief Creating a journal dialog.
    void createJournalDialog();
    /// \brief Checking the availability of the S2 configuration.
    bool isS2Available() noexcept;
    /// \brief Creating a tune dialog for some devices.
    void createTuneDialogs();
    /// \brief Creating a startup dialog for some devices.
    void createStartupValuesDialog();
    /// \brief Creating oscillogram dialogs and switch journal dialogs.
    void createOscAndSwJourDialogs();
    /// \brief Creating a plot dialog.
    void createPlotDialog();
    /// \brief Creating a relay dialog.
    void createRelayDialog();
    /// \brief Creating common dialogs (all modules).
    void createCommonDialogs(const AppConfiguration appCfg);
};
