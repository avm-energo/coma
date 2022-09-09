#include "createmoduledialogs.h"

#include "../module/alarmstateall.h"
#include "../module/board.h"
#include "../module/modulealarm.h"
#include "configdialog.h"
#include "fwuploaddialog.h"
#include "infodialog.h"

CreateModuleDialogs::CreateModuleDialogs(const ModuleSettings &settings, QObject *parent)
    : QObject(parent), settings(settings)
{
}

const QList<UDialog *> &CreateModuleDialogs::createDialogs()
{
    deleteDialogs();
    createConfigDialogs();
    //
    createCommonDialogs();
    return dialogs;
}

void CreateModuleDialogs::createAlarms(AlarmWidget *alarmWidget)
{
    /// TODO: bool -> enum
    static const QHash<ModuleTypes::AlarmKey, QString> alarmSettings {
        { { true, Modules::AlarmType::Critical }, "Базовая аварийная сигнализация" },            //
        { { true, Modules::AlarmType::Warning }, "Базовая предупредительная сигнализация" },     //
        { { true, Modules::AlarmType::Info }, "Базовая информационная сигнализация" },           //
        { { false, Modules::AlarmType::Critical }, "Мезонинная аварийная сигнализация" },        //
        { { false, Modules::AlarmType::Warning }, "Мезонинная предупредительная сигнализация" }, //
        { { false, Modules::AlarmType::Info }, "Мезонинная информационная сигнализация" }        //
    };

    auto alarmStateAll = new AlarmStateAll(alarmWidget);
    alarmStateAll->setupUI(AVM::HthToolTip);
    alarmWidget->addAlarm(alarmStateAll);
    const auto &alarmMap = settings.getAlarms();
    if (!alarmMap.empty())
    {
        for (auto keyIter = alarmMap.keyBegin(); keyIter != alarmMap.keyEnd(); keyIter++)
        {
            auto &title = alarmSettings.value(*keyIter);
            if (!title.isEmpty())
            {
                auto &alarms = alarmMap.value(*keyIter);
                auto moduleAlarm = new ModuleAlarm(keyIter->second, alarms);
                alarmWidget->addAlarm(moduleAlarm, title);
            }
        }
        //        if (settings()->alarms.contains(AlarmType::Warning))
        //        {
        //            auto *alarm = new ModuleAlarm(settings()->alarms.value(AlarmType::Warning),
        //            settings()->alarmCount()); alarmWidget->addAlarm(alarm, tr("Предупредительная сигнализация"));
        //        }
        //        if (settings()->alarms.contains(AlarmType::Critical))
        //        {
        //            auto *alarm = new ModuleAlarm(settings()->alarms.value(AlarmType::Critical),
        //            settings()->alarmCount()); alarmWidget->addAlarm(alarm, tr("Аварийная сигнализация"));
        //        }
        //        if (settings()->alarms.contains(AlarmType::Base))
        //        {
        //            auto *alarm = new ModuleAlarm(
        //                settings()->alarms.value(AlarmType::Base),
        //                settings()->alarms.value(AlarmType::Base).desc.count());
        //            alarmWidget->addAlarm(alarm, tr("Базовая сигнализация"));
        //        }
        //        if (settings()->alarms.contains(AlarmType::Mezz))
        //        {
        //            auto *alarm = new ModuleAlarm(
        //                settings()->alarms.value(AlarmType::Mezz),
        //                settings()->alarms.value(AlarmType::Mezz).desc.count());
        //            alarmWidget->addAlarm(alarm, tr("Мезонинная сигнализация"));
        //        }
    }
}

void CreateModuleDialogs::addDialogToList(UDialog *dlg, const QString &caption, const QString &name)
{
    dlg->setObjectName(name);
    dlg->setCaption(caption);
    dialogs.append(dlg);
}

void CreateModuleDialogs::deleteDialogs()
{
    while (!dialogs.isEmpty())
    {
        auto dialog = dialogs.takeFirst();
        dialog->deleteLater();
    }
}

void CreateModuleDialogs::createConfigDialogs()
{
    auto config = settings.getConfigMap();
    for (auto it = config.cbegin(); it != config.cend(); ++it)
    {
        QString indexStr = QString::number(it.key());
        addDialogToList(new ConfigDialog(&configV, it.value()), "Конфигурация " + indexStr, "conf" + indexStr);
    }
}

void CreateModuleDialogs::createCheckDialogs()
{
    ;
}

void CreateModuleDialogs::createTuneDialogs()
{
    ;
}

void CreateModuleDialogs::createSpecificDialogs()
{
    ;
}

void CreateModuleDialogs::createCommonDialogs()
{
    const auto &board = Board::GetInstance();
    if (board.interfaceType() != Board::InterfaceType::RS485)
        addDialogToList(new FWUploadDialog, "Загрузка ВПО", "upload");
    addDialogToList(new InfoDialog, "О приборе", "info");
}
