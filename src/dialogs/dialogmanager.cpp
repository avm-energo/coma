#include "dialogmanager.h"

DialogManager::DialogManager(const ModuleSettings &settings, QWidget *parent)
    : UDialog(parent)
    , curDialogIndex(0)
    , mDlgCreator(new DialogCreator(settings, this))
    , mWorkspace(new QStackedWidget(this))
    , mSidebar(new QListWidget(this))
{
    init();
}

void DialogManager::init()
{
    auto sizePolizy = mWorkspace->sizePolicy();
    sizePolizy.setRetainSizeWhenHidden(true);
    mWorkspace->setSizePolicy(sizePolizy);
    sizePolizy = mSidebar->sizePolicy();
    sizePolizy.setRetainSizeWhenHidden(true);
    mSidebar->setSizePolicy(sizePolizy);
    connect(mSidebar.get(), &QListWidget::currentRowChanged, mWorkspace.get(), &QStackedWidget::setCurrentIndex);
    hideUI();
}

void DialogManager::hideUI()
{
    mSidebar->hide();
    mWorkspace->hide();
}

void DialogManager::showUI()
{
    mSidebar->show();
    mWorkspace->show();
}

QPair<QListWidget *, QStackedWidget *> DialogManager::getUI()
{
    return { mSidebar.get(), mWorkspace.get() };
}

/// \brief Changing tab widget in the dialog list.
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
    // Индекс -1 норма, когда дестроятся диалоги
    // else
    //    qWarning() << "Неправильный индекс диалога: " << newIndex;
}

void DialogManager::reqUpdate()
{
    if (updatesEnabled())
    {
        auto currentDialog = mDlgCreator->getDialogs()[curDialogIndex];
        currentDialog->reqUpdate();
    }
}

void DialogManager::setupUI(const AppConfiguration appCfg, const QSize size)
{
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
