#include "createmoduledialogs.h"

#include "../module/alarmstateall.h"
#include "../module/board.h"
#include "../module/modulealarm.h"
#include "../module/modules.h"
#include "../tune/82/tune82dialog.h"
#include "../tune/84/tune84dialog.h"
#include "../tune/kiv/tunekivdialog.h"
#include "checkdialog.h"
#include "configdialog.h"
#include "fwuploaddialog.h"
#include "infodialog.h"

DialogManager::DialogManager(const ModuleSettings &settings, QWidget *parent)
    : QObject(parent), settings(settings), mParent(parent)
{
}

const QList<UDialog *> &DialogManager::createDialogs(const AppConfiguration &appCfg)
{
    auto isBoxModule = !Modules::BaseBoards.contains(Board::GetInstance().typeB());
    deleteDialogs();
    createConfigDialogs();
    createCheckDialogs();
    // Регулировка
    if (appCfg == AppConfiguration::Debug)
        createTuneDialogs(isBoxModule);

    createSpecificDialogs(isBoxModule);
    createCommonDialogs();
    return mDialogs;
}

void DialogManager::createAlarms(AlarmWidget *alarmWidget)
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
                auto moduleAlarm = new ModuleAlarm(keyIter->second, alarms, alarmWidget);
                alarmWidget->addAlarm(moduleAlarm, title);
            }
        }
    }
}

void DialogManager::addDialogToList(UDialog *dlg, const QString &caption, const QString &name)
{
    dlg->setObjectName(name);
    dlg->setCaption(caption);
    mDialogs.append(dlg);
}

void DialogManager::deleteDialogs()
{
    while (!mDialogs.isEmpty())
    {
        auto dialog = mDialogs.takeFirst();
        dialog->deleteLater();
    }
}

void DialogManager::createConfigDialogs()
{
    auto &config = settings.getConfigMap();
    for (auto it = config.cbegin(); it != config.cend(); it++)
    {
        auto indexStr = QString::number(it.key());
        addDialogToList(new ConfigDialog(&configV, it.value(), true, mParent), //
            "Конфигурация " + indexStr, "conf" + indexStr);
    }
}

void DialogManager::createCheckDialogs()
{
    auto &sections = settings.getSections();
    auto index = 0;
    for (auto &section : sections)
    {
        auto indexStr = QString::number(index);
        addDialogToList(new CheckDialog(section, mParent), section.name, "check" + indexStr);
        index++;
    }
}

void DialogManager::createTuneDialogs(const bool &isBoxModule)
{
    using namespace Modules;
    auto &board = Board::GetInstance();
    // Коробочный модуль
    if (isBoxModule)
    {
        auto moduleModel = Model(board.type());
        if (moduleModel == Model::KIV)
            addDialogToList(new TuneKIVDialog(&configV), "Регулировка", "tune");
        else
        {
            // TODO: Добавить регулировку для других модулей
        }
    }
    // Модуль состоит из двух плат
    else
    {
        auto typeB = BaseBoard(board.typeB());
        auto typeM = MezzanineBoard(board.typeM());
        if (typeB == BaseBoard::MTB_80)
        {
            if ((typeM == MezzanineBoard::MTM_81) || (typeM == MezzanineBoard::MTM_82)
                || (typeM == MezzanineBoard::MTM_83))
                addDialogToList(new Tune82Dialog(&configV, typeM), "Регулировка", "tune");
            else if (typeM == MezzanineBoard::MTM_84)
                addDialogToList(new TuneKIVDialog(&configV), "Регулировка", "tune");
        }
    }
}

void DialogManager::createSpecificDialogs(const bool &isBoxModule)
{
    ;
}

void DialogManager::createCommonDialogs()
{
    const auto &board = Board::GetInstance();
    if (board.interfaceType() != Board::InterfaceType::RS485)
        addDialogToList(new FWUploadDialog(mParent), "Загрузка ВПО", "upload");
    addDialogToList(new InfoDialog(mParent), "О приборе", "info");
}
