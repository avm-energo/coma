#include "dialogmanager.h"

#include "../module/board.h"
#include "../module/journkdv.h"
#include "../module/journkiv.h"
#include "../module/journktf.h"
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

/// \brief Creating a list of dialogs based on module settings for a current connected module.
const QList<UDialog *> &DialogManager::createDialogs(const AppConfiguration &appCfg)
{
    deleteDialogs();
    createConfigDialogs();
    createCheckDialogs();
    createSpecificDialogs(appCfg);
    createCommonDialogs(appCfg);
    return mDialogs;
}

/// \brief Adding the created dialog to the list for saving.
void DialogManager::addDialogToList(UDialog *dlg, const QString &caption, const QString &name)
{
    dlg->setObjectName(name);
    dlg->setCaption(caption);
    mDialogs.append(dlg);
}

/// \brief Removing all dialogs in the dialog list.
void DialogManager::deleteDialogs()
{
    while (!mDialogs.isEmpty())
    {
        auto dialog = mDialogs.takeFirst();
        dialog->deleteLater();
    }
}

/// \brief Changing tab widget in the dialog list.
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

/// \brief Returns true if module in box, otherwise false.
bool DialogManager::isBoxModule(const quint16 &type) const
{
    return (type == Modules::Model::KDV || type == Modules::Model::KTF || type == Modules::Model::KIV);
}

/// \brief Creating config dialogs.
void DialogManager::createConfigDialogs()
{
    auto &config = settings.getConfigMap();
    for (auto it = config.cbegin(); it != config.cend(); it++)
    {
        QString indexStr = (config.size() > 1) ? ((it.key() == 1) ? "база" : "мезонин") : "";
        addDialogToList(new ConfigDialog(&configV, it.value(), true, mParent), //
            "Конфигурация " + indexStr, "conf" + QString::number(it.key()));
    }
}

/// \brief Creating check dialogs.
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

/// \brief Creating tune dialogs for KIV, KTF and KDV.
void DialogManager::createBoxTuneDialogs(const Modules::Model &boxModel)
{
    if (boxModel == Modules::Model::KIV)
        addDialogToList(new TuneKIVDialog(&configV, mParent), "Регулировка", "tune");
    else
    {
        // TODO: Добавить регулировку для других модулей
    }
}

/// \brief Creating journal and startup dialogs for KIV, KTF and KDV.
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

/// \brief Creating tune dialogs for two-part modules.
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

/// \brief Creating oscillogram and switch journal dialogs.
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

/// \brief Creating specific dialogs (tune, journal and startup).
void DialogManager::createSpecificDialogs(const AppConfiguration &appCfg)
{
    using namespace Modules;
    auto &board = Board::GetInstance();
    // Коробочный модуль
    if (isBoxModule(board.baseSerialInfo().type()))
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
    }
}

/// \brief Creating common dialogs (all modules).
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
