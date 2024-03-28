#pragma once

#include "../widgets/udialog.h"

#include <QObject>
#include <device/device_list.h>

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
    Device::CurrentDevice *m_device;
    Device::Model m_boxModel;
    Device::BaseBoard m_typeB;
    Device::MezzanineBoard m_typeM;
    QWidget *m_parent;
    QList<UDialog *> m_dialogs;

public:
    explicit DialogCreator(Device::CurrentDevice *device, QWidget *parent = nullptr);

    /// \brief Creating the list with dialogs based on module settings for the current connected device.
    void createDialogs(const AppConfiguration appCfg);
    /// \brief Removing all dialogs in the dialog list.
    void deleteDialogs();
    /// \brief Returns the list with dialogs.
    QList<UDialog *> &getDialogs();

private:
    /// \brief Adding the created dialog to the list for saving.
    void addDialogToList(UDialog *dlg, const QString &caption, const QString &name);
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
