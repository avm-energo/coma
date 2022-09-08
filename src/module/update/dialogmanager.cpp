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

void DialogManager::create(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM)
{
    using namespace Modules;
    if (Board::isUSIO(typeB, typeM))
        return createUSIO(typeB, typeM);

    /// TODO: Переписать :)
    //    if (settings())
    //    {
    //        if (!settings()->configSettings.general.isEmpty())
    //        {
    //            addDialogToList(
    //                new ConfigDialog(&configV, settings()->configSettings.general), "Конфигурирование", "conf1");
    //        }
    //        if (settings()->ifaceSettings.settings.isValid())
    //        {
    //            for (auto &&item : m_gsettings.check.items)
    //            {
    //                addDialogToList(
    //                    new CheckDialog(item, m_gsettings.check.categories), item.header, "check:" + item.header);
    //            }
    //        }
    //    }
    //    if ((typeB == BaseBoard::MTB_80) && (typeM == MezzanineBoard::MTM_84))
    //    {
    //        qDebug("Here is KIV");
    //        addDialogToList(new Tune84Dialog(&configV), "Регулировка");
    //        assert(m_gsettings.check.items.size() == 1);
    //        auto &&item = m_gsettings.check.items.front();
    //        auto check = new CheckDialog(item, m_gsettings.check.categories);
    //        check->setHighlights(CheckDialog::Warning, settings()->highlightWarn);
    //        check->setHighlights(CheckDialog::Critical, settings()->highlightCrit);
    //        addDialogToList(check, item.header, "check:" + item.header);
    //        addDialogToList(new StartupKIVDialog, "Начальные\nзначения");
    //    }
    //    if ((typeB == BaseBoard::MTB_86) && (typeM == MezzanineBoard::MTM_00))
    //    {
    //        qDebug("Here is AVTUK-8600");
    //    }
    //    if (typeB == BaseBoard::MTB_80)
    //    {
    //        if ((typeM == MezzanineBoard::MTM_81) || (typeM == MezzanineBoard::MTM_82) || (typeM ==
    //        MezzanineBoard::MTM_83))
    //        {
    //            addDialogToList(new OscDialog, "Осциллограммы");
    //            addDialogToList(new Tune82Dialog(&configV, typeM), "Регулировка");
    //        }
    //    }
    //    if ((typeB == BaseBoard::MTB_80) && (typeM == MezzanineBoard::MTM_85))
    //    {
    //        qDebug("Here is AVTUK-8085");
    //        addDialogToList(new SwitchJournalDialog, "Журнал переключений");
    //        addDialogToList(new OscDialog, "Осциллограммы");
    //    }
    //    if ((typeB == BaseBoard::MTB_85) && (typeM == MezzanineBoard::MTM_85))
    //    {
    //        qDebug("Here is AVTUK-8585");
    //        addDialogToList(new SwitchJournalDialog, "Журнал переключений");
    //        addDialogToList(new OscDialog, "Осциллограммы");
    //    }
    //    if (!settings()->ifaceSettings.settings.isNull())
    //    {
    //        TimeDialog *tdlg = new TimeDialog;
    //        addDialogToList(tdlg, "Время", "time");
    //    }
}

void DialogManager::createUSIO(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM)
{
    using namespace Modules;
    QString message("Here is AVTUK-%1%2");
    qDebug() << message.arg(typeB, 0, 16).arg(typeM, 0, 16);
    /// TODO: переписываем
    //    if (!settings()->configSettings.general.empty())
    //    {
    //        addDialogToList(new ConfigDialog(&configV, settings()->configSettings.general),
    //            "Конфигурирование"
    //            "\nобщая",
    //            "confGeneral");
    //    }
    //    if (!settings()->configSettings.base.empty())
    //    {
    //        addDialogToList(new ConfigDialog(&configV, settings()->configSettings.base, false),
    //            "Конфигурирование"
    //            "\nбазовая",
    //            "confBase");
    //    }
    //    if (!settings()->configSettings.mezz.empty())
    //    {
    //        addDialogToList(new ConfigDialog(&configV, settings()->configSettings.mezz, false),
    //            "Конфигурирование"
    //            "\nмезонинная",
    //            "confMezz");
    //    }

    //    if ((!settings()->ifaceSettings.settings.isValid())
    //        && (Board::GetInstance().interfaceType() != Board::InterfaceType::Emulator))
    //        return;

    //    const auto &item = m_gsettings.check.items.at(0);
    //    if (typeB == BaseBoard::MTB_35)
    //    {
    //        addDialogToList(new RelayDialog(4), "Реле", "relay1");
    //    }
    //    addDialogToList(new CheckDialog(item, m_gsettings.check.categories), item.header, "check:" + item.header);
    //    if (m_gsettings.check.items.size() == 2)
    //    {
    //        const auto &itemIn = m_gsettings.check.items.at(1);
    //        addDialogToList(new CheckDialog(itemIn, m_gsettings.check.categories), itemIn.header, "check:" +
    //        itemIn.header);
    //    }
}

void DialogManager::create(QTimer *updateTimer)
{
    using namespace Modules;
    // BaseInterface::iface()->setSettings(settings()->ifaceSettings);
    auto &board = Board::GetInstance();
    quint16 typeb = board.typeB();
    if (BaseBoards.contains(typeb)) // there must be two-part module
    {
        quint16 typem = board.typeM();
        create(static_cast<BaseBoard>(typeb), static_cast<MezzanineBoard>(typem));
    }
    else
    {
        quint16 mtype = board.type();
        createModule(Modules::Model(mtype));
    }
    createCommon();

    /// TODO: Добавить definition
    //    HiddenDialog *hiddenDialog = new HiddenDialog;
    //    hiddenDialog->fill();
    //    addDialogToList(hiddenDialog, "Секретные операции", "hidden");

    QList<UDialog *> dlgs = dialogs();
    for (auto *d : dlgs)
    {
        connect(updateTimer, &QTimer::timeout, d, &UDialog::reqUpdate);
        d->uponInterfaceSetting();
    }
}
