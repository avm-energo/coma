#include "dialogmanager.h"

#include "../board.h"
#include "../comaversion/comaversion.h"
#include "../ctti/type_id.hpp"
#include "../dialogs/fwuploaddialog.h"
#include "../dialogs/infodialog.h"
#include "../module/alarmstateall.h"
#include "../modulealarm.h"
#include "../s2/configv.h"

DialogManager::DialogManager(QObject *parent) : QObject(parent)
{
}

void DialogManager::createAlarm(AlarmWidget *aw)
{
    using namespace Modules;

    Q_ASSERT(aw->count() == 0);
    AlarmStateAll *alarmStateAll = new AlarmStateAll;
    alarmStateAll->setupUI(AVM::HthToolTip);
    aw->addAlarm(alarmStateAll);
    /// TODO: Вот это дело надо переписать
    //    if (m_settings)
    //    {
    //        if (settings()->alarms.contains(AlarmType::Warning))
    //        {
    //            auto *alarm = new ModuleAlarm(settings()->alarms.value(AlarmType::Warning), settings()->alarmCount());
    //            aw->addAlarm(alarm, tr("Предупредительная сигнализация"));
    //        }
    //        if (settings()->alarms.contains(AlarmType::Critical))
    //        {
    //            auto *alarm = new ModuleAlarm(settings()->alarms.value(AlarmType::Critical),
    //            settings()->alarmCount()); aw->addAlarm(alarm, tr("Аварийная сигнализация"));
    //        }
    //        if (settings()->alarms.contains(AlarmType::Base))
    //        {
    //            auto *alarm = new ModuleAlarm(
    //                settings()->alarms.value(AlarmType::Base),
    //                settings()->alarms.value(AlarmType::Base).desc.count());
    //            aw->addAlarm(alarm, tr("Базовая сигнализация"));
    //        }
    //        if (settings()->alarms.contains(AlarmType::Mezz))
    //        {
    //            auto *alarm = new ModuleAlarm(
    //                settings()->alarms.value(AlarmType::Mezz),
    //                settings()->alarms.value(AlarmType::Mezz).desc.count());
    //            aw->addAlarm(alarm, tr("Мезонинная сигнализация"));
    //        }
    //    }
}

QList<UDialog *> DialogManager::dialogs()
{
    QList<UDialog *> list = m_dialogs;
    return list;
}

void DialogManager::addDialogToList(UDialog *dlg, const QString &caption, const QString &name)
{
    dlg->setObjectName(name);
    dlg->setCaption(caption);
    m_dialogs.append(dlg);
}

void DialogManager::parentTWTabChanged(int index)
{
    if (index == -1 || m_dialogs.isEmpty())
        return;

    for (auto &dialog : m_dialogs)
    {
        dialog->engine()->setUpdatesEnabled(false);
    }

    UDialog *udlg = m_dialogs.at(index);

    udlg->setEnabled(true);
    udlg->engine()->setUpdatesEnabled();
    udlg->reqUpdate();
}

void DialogManager::closeDialogs()
{
    while (!m_dialogs.isEmpty())
        m_dialogs.takeFirst()->close();
}

void DialogManager::create(UniquePointer<Journals> jour)
{
    if (Board::GetInstance().interfaceType() != Board::InterfaceType::RS485)
    {
        Q_ASSERT(jour != nullptr);
        addDialogToList(new JournalDialog(std::move(jour)), "Журналы");
    }
}

void DialogManager::createCommon()
{
    const auto &board = Board::GetInstance();
    if (board.interfaceType() != Board::InterfaceType::RS485)
        addDialogToList(new FWUploadDialog, "Загрузка ВПО");

    addDialogToList(new InfoDialog, "О приборе", "info");
}
