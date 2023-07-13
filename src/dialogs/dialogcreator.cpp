#include "dialogcreator.h"

#include "../journals/journaldialog.h"
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
#include "plotdialog.h"
#include "relaydialog.h"
#include "switchjournaldialog.h"
#include "timedialog.h"

DialogCreator::DialogCreator(const ModuleSettings &settings, const Board &board, QWidget *parent)
    : QObject(parent), settings(settings), board(board), mParent(parent)
{
}

/// \brief Creating a list of dialogs based on module settings for a current connected module.
void DialogCreator::createDialogs(const AppConfiguration appCfg)
{
    deleteDialogs();
    createConfigDialogs();
    createCheckDialogs();
    createJournalDialog();
    createSpecificDialogs(appCfg);
    createCommonDialogs(appCfg);
}

/// \brief Adding the created dialog to the list for saving.
void DialogCreator::addDialogToList(UDialog *dlg, const QString &caption, const QString &name)
{
    dlg->setObjectName(name);
    dlg->setCaption(caption);
    mDialogs.append(dlg);
}

/// \brief Removing all dialogs in the dialog list.
void DialogCreator::deleteDialogs()
{
    while (!mDialogs.isEmpty())
    {
        auto dialog = mDialogs.takeFirst();
        dialog->deleteLater();
    }
}

/// \brief Returns list with dialogs.
QList<UDialog *> &DialogCreator::getDialogs()
{
    return mDialogs;
}

/// \brief Returns true if module in box, otherwise false.
bool DialogCreator::isBoxModule(const quint16 &type) const
{
    return (type == Modules::Model::KDV || type == Modules::Model::KTF || type == Modules::Model::KIV);
}

/// \brief Creating config dialogs.
void DialogCreator::createConfigDialogs()
{
    auto &config = settings.getConfigMap();
    for (auto it = config.cbegin(); it != config.cend(); it++)
    {
        QString indexStr = (config.size() > 1) ? ((it.key() == baseConfigIndex) ? "база" : "мезонин") : "";
        addDialogToList(new ConfigDialog(&configV, it.value(), true, mParent), //
            "Конфигурация " + indexStr, "conf" + QString::number(it.key()));
    }
}

/// \brief Creating check dialogs.
void DialogCreator::createCheckDialogs()
{
    auto &sections = settings.getSections();
    auto index = 0;
    for (auto &section : sections)
    {
        auto indexStr = QString::number(index);
        auto checkDialog = new CheckDialog(section, mParent);
        checkDialog->setHighlights(Modules::AlarmType::Critical, settings.getHighlights(Modules::AlarmType::Critical));
        checkDialog->setHighlights(Modules::AlarmType::Warning, settings.getHighlights(Modules::AlarmType::Warning));
        addDialogToList(checkDialog, section.name, "check" + indexStr);
        index++;
    }
}

/// \brief Creating tune dialogs for KIV, KTF and KDV.
void DialogCreator::createBoxTuneDialogs(const Modules::Model boxModel)
{
    if (boxModel == Modules::Model::KIV)
    {
        // TODO: Реанимировать регулировку для КИВ, временно не работает :(
        addDialogToList(new TuneKIVDialog(&configV, mParent), "Регулировка", "tune");
    }
    else
    {
        // TODO: Добавить регулировку для других модулей
    }
}

/// \brief Creating journal dialog.
void DialogCreator::createJournalDialog()
{
    using namespace journals;
    // TODO: Только для USB
    // Делаем проверку и создаём диалог для журналов
    if (board.interfaceType() != Board::InterfaceType::RS485)
        addDialogToList(new JournalDialog(settings, mParent), "Журналы", "jours");
}

/// \brief Creating startup dialog for KIV, KTF and KDV.
void DialogCreator::createStartupDialog(const Modules::Model boxModel)
{
    if (boxModel == Modules::Model::KIV)
        addDialogToList(new StartupKIVDialog(mParent), "Начальные\nзначения", "startup");
    else if (boxModel == Modules::Model::KTF)
        addDialogToList(new StartupKTFDialog(mParent), "Начальные\nзначения", "startup");
    else if (boxModel == Modules::Model::KDV)
        addDialogToList(new StartupKDVDialog(mParent), "Начальные\nзначения", "startup");
}

/// \brief Creating tune dialogs for two-part modules.
void DialogCreator::createTwoPartTuneDialogs(const Modules::BaseBoard &typeb, const Modules::MezzanineBoard &typem)
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
void DialogCreator::createOscAndSwJourDialogs(const Modules::BaseBoard &typeb, const Modules::MezzanineBoard &typem)
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
void DialogCreator::createSpecificDialogs(const AppConfiguration appCfg)
{
    using namespace Modules;

    // Коробочный модуль
    if (isBoxModule(board.baseSerialInfo().type()))
    {
        auto moduleModel = Model(board.type());
        // Добавляем регулировку, если АВМ Настройка
        if (appCfg == AppConfiguration::Debug)
            createBoxTuneDialogs(moduleModel);
        // TODO: Временно выключено для модбаса
        if (board.interfaceType() == Board::InterfaceType::USB)
            createStartupDialog(moduleModel); // Добавляем диалог начальных значений

        // TODO: Fix it
        //        addDialogToList(new PlotDialog(mParent), "Диаграммы", "plot"); // векторные диаграммы нужны для
        //        АВ-ТУК-82 и АВМ-КТФ, а не для всех коробочных модулей
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
        // Добавляем диалог переключений
        if (typeB == BaseBoard::MTB_35)
            addDialogToList(new RelayDialog(4, mParent), "Реле", "relay1");
    }
}

/// \brief Creating common dialogs (all modules).
void DialogCreator::createCommonDialogs(const AppConfiguration appCfg)
{
    if (board.interfaceType() != Board::InterfaceType::Ethernet)
        addDialogToList(new FWUploadDialog(mParent), "Загрузка ВПО", "upload");
    addDialogToList(new TimeDialog(mParent), "Время", "time");
    addDialogToList(new InfoDialog(mParent), "О приборе", "info");

    if (appCfg == AppConfiguration::Debug)
    {
        auto hiddenDialog = new HiddenDialog(mParent);
        hiddenDialog->fill();
        addDialogToList(hiddenDialog, "Секретные операции", "hidden");
    }
}
