#pragma once

#include "../widgets/udialog.h"
#include "dialogcreator.h"

#include <QListWidget>
#include <QPair>
#include <QStackedWidget>
#include <QTimer>

/// \brief Class for managing dialogs.
class DialogManager : public UDialog
{
    Q_OBJECT
private:
    int m_currentDialogIndex;
    UniquePointer<DialogCreator> m_dlgCreator;
    UniquePointer<QStackedWidget> m_workspace;
    UniquePointer<QListWidget> m_sidebar;
    UniquePointer<QTimer> m_reqTimer;

    /// \brief Hiding sidebar and main workspace.
    void hideUI();
    /// \brief Showing sidebar and main workspace.
    void showUI();

public:
    DialogManager(const ModuleSettings &settings, S2DataManager &s2DataManager, //
        S2RequestService &s2ReqService, QWidget *parent = nullptr);
    /// \brief Returns pointers to UI elements (sidebar and main workspace) in QPair struct.
    QPair<QListWidget *, QStackedWidget *> getUI();
    /// \brief Setup UI using DialogCreator for creating dialogs for current device.
    void setupUI(const AppConfiguration appCfg, const QSize size);

public slots:
    /// \brief Overloaded virtual function for update request.
    void reqUpdate() override;
    /// \brief Update a dialog in the sidebar.
    void dialogChanged(int newIndex);
    /// \brief Removing dialogs, clear and hide UI elements.
    void clearDialogs();
};
