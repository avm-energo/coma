#include "dialogmanager.h"

DialogManager::DialogManager(QObject *parent) : QObject(parent)
{
}

/*
void NewModule::createAlarm(AlarmWidget *aw)
{
    using namespace Modules;

    Q_ASSERT(aw->count() == 0);
    AlarmStateAll *alarmStateAll = new AlarmStateAll;
    alarmStateAll->setupUI(AVM::HthToolTip);
    aw->addAlarm(alarmStateAll);
    if (m_settings)
    {
        if (settings()->alarms.contains(AlarmType::Warning))
        {
            auto *alarm = new ModuleAlarm(settings()->alarms.value(AlarmType::Warning), settings()->alarmCount());
            aw->addAlarm(alarm, tr("Предупредительная сигнализация"));
        }
        if (settings()->alarms.contains(AlarmType::Critical))
        {
            auto *alarm = new ModuleAlarm(settings()->alarms.value(AlarmType::Critical), settings()->alarmCount());
            aw->addAlarm(alarm, tr("Аварийная сигнализация"));
        }
        if (settings()->alarms.contains(AlarmType::Base))
        {
            auto *alarm = new ModuleAlarm(
                settings()->alarms.value(AlarmType::Base), settings()->alarms.value(AlarmType::Base).desc.count());
            aw->addAlarm(alarm, tr("Базовая сигнализация"));
        }
        if (settings()->alarms.contains(AlarmType::Mezz))
        {
            auto *alarm = new ModuleAlarm(
                settings()->alarms.value(AlarmType::Mezz), settings()->alarms.value(AlarmType::Mezz).desc.count());
            aw->addAlarm(alarm, tr("Мезонинная сигнализация"));
        }
    }
}

QList<UDialog *> NewModule::dialogs()
{
    QList<UDialog *> list = m_dialogs;
    return list;
}

void NewModule::addDialogToList(UDialog *dlg, const QString &caption, const QString &name)
{
    dlg->setObjectName(name);
    dlg->setCaption(caption);
    m_dialogs.append(dlg);
}

void NewModule::parentTWTabChanged(int index)
{
    if (index == -1 || m_dialogs.isEmpty())
        return;

    for (auto &dialog : m_dialogs)
    {
        //        if (dialog->updatesEnabled())
        dialog->engine()->setUpdatesEnabled(false);
    }

    UDialog *udlg = m_dialogs.at(index);

    udlg->setEnabled(true);
    udlg->engine()->setUpdatesEnabled();
    udlg->reqUpdate();
}

void NewModule::closeDialogs()
{
    while (!m_dialogs.isEmpty())
        m_dialogs.takeFirst()->close();
}

void NewModule::create(UniquePointer<Journals> jour)
{
    if (Board::GetInstance().interfaceType() != Board::InterfaceType::RS485)
    {
        Q_ASSERT(jour != nullptr);
        addDialogToList(new JournalDialog(std::move(jour)), "Журналы");
    }
}

void NewModule::createCommon()
{
    const auto &board = Board::GetInstance();
    if (board.interfaceType() != Board::InterfaceType::RS485)
        addDialogToList(new FWUploadDialog, "Загрузка ВПО");

    addDialogToList(new InfoDialog, "О приборе", "info");
}
*/
