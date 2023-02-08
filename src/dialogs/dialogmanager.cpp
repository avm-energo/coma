#include "dialogmanager.h"

DialogManager::DialogManager(const ModuleSettings &settings, QWidget *parent)
    : UDialog(parent)
    , curDialogIndex(0)
    , mDlgCreator(new DialogCreator(settings, this))
    , mWorkspace(new QStackedWidget(this))
    , mSidebar(new QListWidget(this))
{
    // Init settings for sidebar and main workspace.
    auto sizePolizy = mWorkspace->sizePolicy();
    sizePolizy.setRetainSizeWhenHidden(true);
    mWorkspace->setSizePolicy(sizePolizy);
    sizePolizy = mSidebar->sizePolicy();
    sizePolizy.setRetainSizeWhenHidden(true);
    mSidebar->setSizePolicy(sizePolizy);
    connect(mSidebar.get(), &QListWidget::currentRowChanged, mWorkspace.get(), &QStackedWidget::setCurrentIndex);
    hideUI();
}

/// \brief Hiding sidebar and main workspace.
void DialogManager::hideUI()
{
    mSidebar->hide();
    mWorkspace->hide();
}

/// \brief Showing sidebar and main workspace.
void DialogManager::showUI()
{
    mSidebar->show();
    mWorkspace->show();
}

/// \brief Returns pointers to UI elements (sidebar and main workspace) in QPair struct.
QPair<QListWidget *, QStackedWidget *> DialogManager::getUI()
{
    return { mSidebar.get(), mWorkspace.get() };
}

/// \brief Changing dialog in the sidebar.
void DialogManager::dialogChanged(int newIndex)
{
    auto &dialogs = mDlgCreator->getDialogs();
    if (newIndex >= 0 && newIndex < dialogs.size())
    {
        auto oldDialog = dialogs[curDialogIndex];
        oldDialog->engine()->setUpdatesEnabled(false);
        auto newDialog = dialogs[newIndex];
        newDialog->engine()->setUpdatesEnabled(true);
        newDialog->setEnabled(true);
        newDialog->reqUpdate();
    }
    // Индекс -1 норма, когда удаляются диалоги
    // else
    //    qWarning() << "Неправильный индекс диалога: " << newIndex;
}

/// \brief Overloaded virtual function for update request.
void DialogManager::reqUpdate()
{
    if (updatesEnabled())
    {
        auto currentDialog = mDlgCreator->getDialogs()[curDialogIndex];
        currentDialog->reqUpdate();
    }
}

/// \brief Setup UI using DialogCreator for creating dialogs for current device.
void DialogManager::setupUI(const AppConfiguration appCfg, const QSize size)
{
    Q_ASSERT(mWorkspace->count() == 0);
    mDlgCreator->createDialogs(appCfg);
    for (auto &dialog : mDlgCreator->getDialogs())
    {
        dialog->engine()->setUpdatesEnabled(false);
        dialog->uponInterfaceSetting();
        auto item = new QListWidgetItem(dialog->getCaption(), mSidebar.get());
        item->setSizeHint(QSize(0, size.height() / 20));
        item->setTextAlignment(Qt::AlignCenter);
        mSidebar->addItem(item);
        mWorkspace->addWidget(dialog);
    }
    curDialogIndex = 0;
    mDlgCreator->getDialogs()[curDialogIndex]->engine()->setUpdatesEnabled(true);
    connect(mWorkspace.get(), &QStackedWidget::currentChanged, this, &DialogManager::dialogChanged);
    mSidebar->setMinimumWidth(size.width() / 6);
    mSidebar->setMaximumWidth(size.width() / 5);
    showUI();
}

/// \brief Removing dialogs, clear and hide UI elements.
void DialogManager::clearDialogs()
{
    hideUI();
    while (mWorkspace->count())
    {
        auto widget = mWorkspace->widget(0);
        mWorkspace->removeWidget(widget);
        widget->deleteLater();
    }
    mSidebar->clear();
    mDlgCreator->deleteDialogs();
}
