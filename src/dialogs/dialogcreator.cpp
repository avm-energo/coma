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
#include "newhiddendialog.h"
#include "plotdialog.h"
#include "relaydialog.h"
#include "switchjournaldialog.h"
#include "timedialog.h"

DialogCreator::DialogCreator(const ModuleSettings &settings, const Board &board, //
    S2DataManager &s2DataManager, S2RequestService &s2ReqService, QWidget *parent)
    : QObject(parent)
    , m_settings(settings)
    , m_board(board)
    , m_s2manager(s2DataManager)
    , m_s2service(s2ReqService)
    , m_parent(parent)
{
}

void DialogCreator::createDialogs(const AppConfiguration appCfg)
{
    deleteDialogs();
    createConfigDialogs();
    createCheckDialogs();
    createJournalDialog();
    createSpecificDialogs(appCfg);
    createCommonDialogs(appCfg);
}

void DialogCreator::addDialogToList(UDialog *dlg, const QString &caption, const QString &name)
{
    constexpr auto defCaption = "Мы забыли название :(";
    if (!name.isEmpty())
        dlg->setObjectName(name);
    if (caption.isEmpty())
        dlg->setCaption(defCaption);
    else
        dlg->setCaption(caption);
    m_dialogs.append(dlg);
}

void DialogCreator::deleteDialogs()
{
    while (!m_dialogs.isEmpty())
    {
        auto dialog = m_dialogs.takeFirst();
        dialog->deleteLater();
    }
}

QList<UDialog *> &DialogCreator::getDialogs()
{
    return m_dialogs;
}

bool DialogCreator::isBoxModule(const quint16 &type) const
{
    return (type == Modules::Model::KDV || type == Modules::Model::KTF || type == Modules::Model::KIV);
}

void DialogCreator::createConfigDialogs()
{
    for (const auto &[boardType, boardConf] : m_s2manager)
    {
        auto confDialog = new ConfigDialog(m_s2service, m_s2manager, boardType, m_parent);
        const auto &confDialogCaption = boardConf.m_tabName;
        const auto confDialogName = "conf" + QString::number(static_cast<int>(boardType));
        addDialogToList(confDialog, confDialogCaption, confDialogName);
    }
}

void DialogCreator::createCheckDialogs()
{
    auto &sections = m_settings.getSections();
    auto index = 0;
    for (auto &section : sections)
    {
        auto indexStr = QString::number(index);
        auto checkDialog = new CheckDialog(section, m_parent);
        checkDialog->setHighlights(
            Modules::AlarmType::Critical, m_settings.getHighlights(Modules::AlarmType::Critical));
        checkDialog->setHighlights(Modules::AlarmType::Warning, m_settings.getHighlights(Modules::AlarmType::Warning));
        addDialogToList(checkDialog, section.name, "check" + indexStr);
        index++;
    }
}

void DialogCreator::createBoxTuneDialogs(const Modules::Model boxModel)
{
    auto &workConfig = m_s2manager.getCurrentConfiguration().m_workingConfig;
    if (boxModel == Modules::Model::KIV)
    {
        // AVM-KIV tune status: currently working
        addDialogToList(new TuneKIVDialog(workConfig, m_parent), "Регулировка", "tune");
    }
    else
    {
        // TODO: Добавить регулировку для других модулей
    }
}

void DialogCreator::createJournalDialog()
{
    using namespace journals;
    addDialogToList(new JournalDialog(m_settings, m_parent), "Журналы", "jours");
}

void DialogCreator::createStartupDialog(const Modules::Model boxModel)
{
    if (boxModel == Modules::Model::KIV)
        addDialogToList(new StartupKIVDialog(m_parent), "Начальные\nзначения", "startup");
    else if (boxModel == Modules::Model::KTF)
        addDialogToList(new StartupKTFDialog(m_parent), "Начальные\nзначения", "startup");
    else if (boxModel == Modules::Model::KDV)
        addDialogToList(new StartupKDVDialog(m_parent), "Начальные\nзначения", "startup");
}

void DialogCreator::createTwoPartTuneDialogs(const Modules::BaseBoard &typeb, const Modules::MezzanineBoard &typem)
{
    using namespace Modules;
    if (typeb == BaseBoard::MTB_80)
    {
        auto &workConfig = m_s2manager.getCurrentConfiguration().m_workingConfig;
        if ((typem == MezzanineBoard::MTM_81) || (typem == MezzanineBoard::MTM_82) || (typem == MezzanineBoard::MTM_83))
            addDialogToList(new Tune82Dialog(workConfig, typem, m_parent), "Регулировка", "tune");
        else if (typem == MezzanineBoard::MTM_84)
        {
            addDialogToList(new TuneKIVDialog(workConfig, m_parent), "Регулировка", "tune");
            addDialogToList(new StartupKIVDialog(m_parent), "Начальные\nзначения", "startup");
        }
    }
    else
    {
        // TODO: Добавить регулировку для других модулей
    }
}

void DialogCreator::createOscAndSwJourDialogs(const Modules::BaseBoard &typeb, const Modules::MezzanineBoard &typem)
{
    using namespace Modules;
    if (typeb == BaseBoard::MTB_80)
    {
        if ((typem == MezzanineBoard::MTM_81) || (typem == MezzanineBoard::MTM_82) || (typem == MezzanineBoard::MTM_83))
        {
            addDialogToList(new OscDialog(m_parent), "Осциллограммы", "osc");
        }
    }
    if ((typeb == BaseBoard::MTB_80 || typeb == BaseBoard::MTB_85) && typem == MezzanineBoard::MTM_85)
    {
        addDialogToList(new SwitchJournalDialog(m_parent), "Журнал переключений", "swjour");
        addDialogToList(new OscDialog(m_parent), "Осциллограммы", "osc");
    }
}

void DialogCreator::createSpecificDialogs(const AppConfiguration appCfg)
{
    using namespace Modules;

    // Коробочный модуль
    if (isBoxModule(m_board.baseSerialInfo().type()))
    {
        auto moduleModel = Model(m_board.type());
        // Добавляем регулировку, если АВМ Настройка
        if (appCfg == AppConfiguration::Debug)
            createBoxTuneDialogs(moduleModel);
        // TODO: Временно выключено для модбаса
        if (m_board.interfaceType() == Interface::IfaceType::USB)
            createStartupDialog(moduleModel); // Добавляем диалог начальных значений

        // TODO: Fix it
        //        addDialogToList(new PlotDialog(mParent), "Диаграммы", "plot"); // векторные диаграммы нужны для
        //        АВ-ТУК-82 и АВМ-КТФ, а не для всех коробочных модулей
    }
    // Модуль состоит из двух плат
    else
    {
        auto typeB = BaseBoard(m_board.typeB());
        auto typeM = MezzanineBoard(m_board.typeM());
        // Добавляем регулировку, если АВМ Настройка
        if (appCfg == AppConfiguration::Debug)
            createTwoPartTuneDialogs(typeB, typeM);
        // Добавляем диалоги осциллограмм и журналов переключений
        createOscAndSwJourDialogs(typeB, typeM);
        // Добавляем диалог переключений
        if (typeB == BaseBoard::MTB_35)
            addDialogToList(new RelayDialog(4, m_parent), "Реле", "relay1");
    }
}

void DialogCreator::createCommonDialogs(const AppConfiguration appCfg)
{
    if (m_board.interfaceType() != Interface::IfaceType::Ethernet)
        addDialogToList(new FWUploadDialog(m_parent), "Загрузка ВПО", "upload");
    addDialogToList(new TimeDialog(m_parent), "Время", "time");
    addDialogToList(new InfoDialog(m_parent), "О приборе", "info");

    if (appCfg == AppConfiguration::Debug)
    {
        auto hiddenDialog = new HiddenDialog(m_parent);
        hiddenDialog->fill();
        addDialogToList(hiddenDialog, "Секретные операции", "hidden");

        // Debug purposes
        auto anotherHiddenDialog = new NewHiddenDialog(m_settings, m_parent);
        anotherHiddenDialog->fill();
        addDialogToList(anotherHiddenDialog, "Секретные операции 2", "hidden2");
    }
}
