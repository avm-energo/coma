#pragma once

#include "dialogcreator.h"

#include <QListWidget>
#include <QPair>
#include <QStackedWidget>
#include <QTimer>
#include <gen/stdfunc.h>

/// \brief Class for managing dialogs.
class DialogManager : public QWidget
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
    explicit DialogManager(QWidget *parent = nullptr);

    /// \brief Returns pointers to UI elements (sidebar and main workspace) in QPair struct.
    QPair<QListWidget *, QStackedWidget *> getUI();
    /// \brief Setup UI using DialogCreator for creating dialogs for current device.
    void setupUI(Device::CurrentDevice *device, const AppConfiguration appCfg, const QSize size);
    /// \brief Removing dialogs, clear and hide UI elements.
    void clearDialogs();

private slots:
    /// \brief Overloaded virtual function for update request.
    void reqUpdate();
    /// \brief Update a dialog in the sidebar.
    void dialogChanged(int newIndex);
};
