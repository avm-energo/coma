#include "dialogmanager.h"

#include "../module/alarmstateall.h"
#include "../module/board.h"
#include "../module/journkdv.h"
#include "../module/journkiv.h"
#include "../module/journktf.h"
#include "../module/modulealarm.h"
#include "../oscillograms/oscdialog.h"
#include "../startup/startupkdvdialog.h"
#include "../startup/startupkivdialog.h"
#include "../startup/startupktfdialog.h"
#include "../tune/82/tune82dialog.h"
#include "../tune/84/tune84dialog.h"
#include "../tune/kiv/tunekivdialog.h"
#include "checkdialog.h"
#include "configdialog.h"
#include "fwuploaddialog.h"
#include "hiddendialog.h"
#include "infodialog.h"
#include "journalsdialog.h"
#include "plotdialog.h"
#include "relaydialog.h"
#include "switchjournaldialog.h"
#include "timedialog.h"

DialogManager::DialogManager(const ModuleSettings &settings, QWidget *parent)
    : QObject(parent), settings(settings), mParent(parent)
{
}

const QList<UDialog *> &DialogManager::createDialogs(const AppConfiguration &appCfg)
{
    deleteDialogs();
    createConfigDialogs();
    createCheckDialogs();
    createSpecificDialogs(appCfg);
    createCommonDialogs(appCfg);
    return mDialogs;
}

// TODO: этим должен заниматься сам AlarmWidget
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

void DialogManager::parentTWTabChanged(int index)
{
    if (index != -1 && !mDialogs.isEmpty())
    {
        for (auto &dialog : mDialogs)
            dialog->engine()->setUpdatesEnabled(false);

        auto dialog = mDialogs.at(index);
        dialog->setEnabled(true);
        dialog->engine()->setUpdatesEnabled();
        dialog->reqUpdate();
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

void DialogManager::createBoxTuneDialogs(const Modules::Model &boxModel)
{
    if (boxModel == Modules::Model::KIV)
        addDialogToList(new TuneKIVDialog(&configV, mParent), "Регулировка", "tune");
    else
    {
        // TODO: Добавить регулировку для других модулей
    }
}

void DialogManager::createJournalAndStartupDialogs(const Modules::Model &boxModel)
{
    UniquePointer<Journals> jours;
    if (boxModel == Modules::Model::KIV)
    {
        jours = UniquePointer<Journals>(new JournKIV(settings.getJours(), this));
        addDialogToList(new StartupKIVDialog(mParent), "Начальные\nзначения", "startup");
    }
    else if (boxModel == Modules::Model::KTF)
    {
        jours = UniquePointer<Journals>(new JournKTF(settings.getJours(), this));
        addDialogToList(new StartupKTFDialog(mParent), "Начальные\nзначения", "startup");
    }
    else if (boxModel == Modules::Model::KDV)
    {
        jours = UniquePointer<Journals>(new JournKDV(settings.getJours(), this));
        addDialogToList(new StartupKDVDialog(mParent), "Начальные\nзначения", "startup");
    }
    // Делаем проверку и создаём диалог для журналов
    if (Board::GetInstance().interfaceType() != Board::InterfaceType::RS485 && jours != nullptr)
        addDialogToList(new JournalDialog(std::move(jours), mParent), "Журналы", "jours");
}

void DialogManager::createTwoPartTuneDialogs(const Modules::BaseBoard &typeb, const Modules::MezzanineBoard &typem)
{
    using namespace Modules;
    if (typeb == BaseBoard::MTB_80)
    {
        if ((typem == MezzanineBoard::MTM_81) || (typem == MezzanineBoard::MTM_82) || (typem == MezzanineBoard::MTM_83))
            addDialogToList(new Tune82Dialog(&configV, typem, mParent), "Регулировка", "tune");
        else if (typem == MezzanineBoard::MTM_84)
        {
            addDialogToList(new TuneKIVDialog(&configV, mParent), "Регулировка", "tune");
            addDialogToList(new StartupKIVDialog(mParent), "Начальные\nзначения", "startup");
        }
    }
    else
    {
        // TODO: Добавить регулировку для других модулей
    }
}

void DialogManager::createOscAndSwJourDialogs(const Modules::BaseBoard &typeb, const Modules::MezzanineBoard &typem)
{
    using namespace Modules;
    if (typeb == BaseBoard::MTB_80)
    {
        if ((typem == MezzanineBoard::MTM_81) || (typem == MezzanineBoard::MTM_82) || (typem == MezzanineBoard::MTM_83))
        {
            addDialogToList(new OscDialog(mParent), "Осциллограммы", "osc");
        }
    }
    if ((typeb == BaseBoard::MTB_80 || typeb == BaseBoard::MTB_85) && typem == MezzanineBoard::MTM_85)
    {
        addDialogToList(new SwitchJournalDialog(mParent), "Журнал переключений", "swjour");
        addDialogToList(new OscDialog(mParent), "Осциллограммы", "osc");
    }
}

void DialogManager::createSpecificDialogs(const AppConfiguration &appCfg)
{
    using namespace Modules;
    auto &board = Board::GetInstance();
    auto isBoxModule = !Modules::BaseBoards.contains(board.typeB());
    // Коробочный модуль
    if (isBoxModule)
    {
        auto moduleModel = Model(board.type());
        // Добавляем регулировку, если АВМ Настройка
        if (appCfg == AppConfiguration::Debug)
            createBoxTuneDialogs(moduleModel);
        // Добавляем диалоги журналов и начальных значений
        createJournalAndStartupDialogs(moduleModel);
        addDialogToList(new PlotDialog(mParent), "Диаграммы", "plot");
        addDialogToList(new TimeDialog(mParent), "Время", "time");
    }
    // Модуль состоит из двух плат
    else
    {
        auto typeB = BaseBoard(board.typeB());
        auto typeM = MezzanineBoard(board.typeM());
        // Добавляем регулировку, если АВМ Настройка
        if (appCfg == AppConfiguration::Debug)
            createTwoPartTuneDialogs(typeB, typeM);
        // Добавляем диалоги осциллограмм и журналов переключений
        createOscAndSwJourDialogs(typeB, typeM);
        // Добавляем диалог реле (???)
        if (typeB == BaseBoard::MTB_35)
            addDialogToList(new RelayDialog(4, mParent), "Реле", "relay1");
        // TODO: если модуль не USIO и !ifaceSettings.settings.isNull()
        // addDialogToList(new TimeDialog(mParent), "Время", "time");
    }
}

void DialogManager::createCommonDialogs(const AppConfiguration &appCfg)
{
    const auto &board = Board::GetInstance();
    if (board.interfaceType() != Board::InterfaceType::RS485)
        addDialogToList(new FWUploadDialog(mParent), "Загрузка ВПО", "upload");
    addDialogToList(new InfoDialog(mParent), "О приборе", "info");

    if (appCfg == AppConfiguration::Debug)
    {
        auto hiddenDialog = new HiddenDialog(mParent);
        hiddenDialog->fill();
        addDialogToList(hiddenDialog, "Секретные операции", "hidden");
    }
}
