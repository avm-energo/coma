#include "dialogmanager.h"

DialogManager::DialogManager(const ModuleSettings &settings, //
    S2DataManager &s2DataManager, S2RequestService &s2ReqService, QWidget *parent)
    : UDialog(parent)
    , m_currentDialogIndex(0)
    , m_dlgCreator(new DialogCreator(settings, Board::GetInstance(), s2DataManager, s2ReqService, this))
    , m_workspace(new QStackedWidget(this))
    , m_sidebar(new QListWidget(this))
    , m_reqTimer(new QTimer(this))
{
    // Init settings for sidebar and main workspace.
    auto sizePoliсy = m_workspace->sizePolicy();
    sizePoliсy.setRetainSizeWhenHidden(true);
    m_workspace->setSizePolicy(sizePoliсy);
    sizePoliсy = m_sidebar->sizePolicy();
    sizePoliсy.setRetainSizeWhenHidden(true);
    m_sidebar->setSizePolicy(sizePoliсy);
    connect(m_sidebar.get(), &QListWidget::currentRowChanged, m_workspace.get(), &QStackedWidget::setCurrentIndex);
    /// TODO: Вынести интервал запроса в настройки
    m_reqTimer->setInterval(1000);
    connect(m_reqTimer.get(), &QTimer::timeout, this, &DialogManager::reqUpdate);
    hideUI();
}

void DialogManager::hideUI()
{
    m_sidebar->hide();
    m_workspace->hide();
}

void DialogManager::showUI()
{
    m_sidebar->show();
    m_workspace->show();
}

QPair<QListWidget *, QStackedWidget *> DialogManager::getUI()
{
    return { m_sidebar.get(), m_workspace.get() };
}

void DialogManager::dialogChanged(int newIndex)
{
    auto &dialogs = m_dlgCreator->getDialogs();
    // Индекс -1 норма, когда удаляются диалоги
    if (newIndex >= 0 && newIndex < dialogs.size())
    {
        auto oldDialog = dialogs[m_currentDialogIndex];
        oldDialog->engine()->setUpdatesEnabled(false);
        auto newDialog = dialogs[newIndex];
        newDialog->engine()->setUpdatesEnabled(true);
        newDialog->setEnabled(true);
        m_currentDialogIndex = newIndex;
    }
}

void DialogManager::reqUpdate()
{
    auto currentDialog = m_dlgCreator->getDialogs()[m_currentDialogIndex];
    currentDialog->reqUpdate();
}

void DialogManager::setupUI(const AppConfiguration appCfg, const QSize size)
{
    Q_ASSERT(m_workspace->count() == 0);
    m_dlgCreator->createDialogs(appCfg);
    for (auto &dialog : m_dlgCreator->getDialogs())
    {
        dialog->engine()->setUpdatesEnabled(false);
        dialog->uponInterfaceSetting();
        dialog->updateConnection(m_dataUpdater->currentConnection());
        auto item = new QListWidgetItem(dialog->getCaption(), m_sidebar.get());
        item->setSizeHint(QSize(0, size.height() / 20));
        item->setTextAlignment(Qt::AlignCenter);
        m_sidebar->addItem(item);
        m_workspace->addWidget(dialog);
    }
    m_currentDialogIndex = 0;
    m_dlgCreator->getDialogs()[m_currentDialogIndex]->engine()->setUpdatesEnabled(true);
    connect(m_workspace.get(), &QStackedWidget::currentChanged, this, &DialogManager::dialogChanged);
    m_sidebar->setMinimumWidth(size.width() / 6);
    m_sidebar->setMaximumWidth(size.width() / 5);
    showUI();
    m_reqTimer->start();
}

void DialogManager::clearDialogs()
{
    m_reqTimer->stop();
    hideUI();
    while (m_workspace->count())
    {
        auto widget = m_workspace->widget(0);
        m_workspace->removeWidget(widget);
        widget->deleteLater();
    }
    m_currentDialogIndex = 0;
    m_sidebar->clear();
    m_dlgCreator->deleteDialogs();
}
