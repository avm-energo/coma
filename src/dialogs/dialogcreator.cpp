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

DialogCreator::DialogCreator(const ModuleSettings &settings, const Board &board, //
    S2DataManager &s2DataManager, S2RequestService &s2ReqService, QWidget *parent)
    : QObject(parent)
    , m_boxModel(Modules::Model(board.type()))
    , m_typeB(Modules::BaseBoard(board.typeB()))
    , m_typeM(Modules::MezzanineBoard(board.typeM()))
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

    // Регулировка доступна только в АВМ-Наладке при связи по USB
    if (appCfg == AppConfiguration::Debug && m_board.interfaceType() == Interface::IfaceType::USB)
        createTuneDialogs();
    // TODO: Временно выключено для модбаса
    if (m_board.interfaceType() == Interface::IfaceType::USB)
        createStartupValuesDialog();

    createOscAndSwJourDialogs();
    createPlotDialog();
    createRelayDialog();
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
    for (auto &[boardType, boardConf] : m_s2manager)
    {
        boardConf.setDefaultConfig();
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

void DialogCreator::createJournalDialog()
{
    using namespace journals;
    addDialogToList(new JournalDialog(m_settings, m_parent), "Журналы", "jours");
}

bool DialogCreator::isS2Available() noexcept
{
    try
    {
        [[maybe_unused]] volatile auto &workConfig = m_s2manager.getCurrentConfiguration().m_workingConfig;
        return true;
    } catch (...)
    {
        return false;
    }
}

void DialogCreator::createTuneDialogs()
{
    using namespace Modules;
    if (!isS2Available())
    {
        qCritical() << "Ошибка создания диалога регулировки";
        return;
    }

    auto &workConfig = m_s2manager.getCurrentConfiguration().m_workingConfig;
    if (m_typeB == BaseBoard::MTB_80)
    {
        if (m_typeM == MezzanineBoard::MTM_81 || m_typeM == MezzanineBoard::MTM_82 || m_typeM == MezzanineBoard::MTM_83)
            addDialogToList(new Tune82Dialog(workConfig, m_typeM, m_parent), "Регулировка", "tune");
        else if (m_typeM == MezzanineBoard::MTM_84)
            addDialogToList(new TuneKIVDialog(workConfig, m_parent), "Регулировка", "tune");
        else if (m_boxModel == Model::KIV)
            addDialogToList(new TuneKIVDialog(workConfig, m_parent), "Регулировка", "tune");
        else
        {
            // TODO: Добавить регулировку для других модулей
        }
    }
}

void DialogCreator::createStartupValuesDialog()
{
    // Добавляем диалог начальных значений
    using namespace Modules;
    switch (m_boxModel)
    {
    case Model::KIV:
        addDialogToList(new StartupKIVDialog(m_parent), "Начальные\nзначения", "startup");
        break;
    case Model::KTF:
        addDialogToList(new StartupKTFDialog(m_parent), "Начальные\nзначения", "startup");
        break;
    case Model::KDV:
        addDialogToList(new StartupKDVDialog(m_parent), "Начальные\nзначения", "startup");
        break;
    default:
        // У АВ-ТУК-84 те же начальные значение, что и у АВМ-КИВ
        if (m_typeB == BaseBoard::MTB_80 && m_typeM == MezzanineBoard::MTM_84)
            addDialogToList(new StartupKIVDialog(m_parent), "Начальные\nзначения", "startup");
        break;
    }
}

void DialogCreator::createOscAndSwJourDialogs()
{
    // Добавляем диалоги осциллограмм и журналов переключений
    using namespace Modules;
    if (m_typeB == BaseBoard::MTB_80)
    {
        if (m_typeM == MezzanineBoard::MTM_81 || m_typeM == MezzanineBoard::MTM_82 || m_typeM == MezzanineBoard::MTM_83)
            addDialogToList(new OscDialog(m_parent), "Осциллограммы", "osc");
    }
    if ((m_typeB == BaseBoard::MTB_80 || m_typeB == BaseBoard::MTB_85))
    {
        if (m_typeM == MezzanineBoard::MTM_85)
        {
            addDialogToList(new SwitchJournalDialog(m_parent), "Журнал переключений", "swjour");
            addDialogToList(new OscDialog(m_parent), "Осциллограммы", "osc");
        }
    }
}

void DialogCreator::createPlotDialog()
{
    // Только для АВ-ТУК-82 и АВМ-КТФ добавляем диалог с векторными диаграммами
    using namespace Modules;
    if ((m_typeB == BaseBoard::MTB_80 && m_typeM == MezzanineBoard::MTM_82) || (m_boxModel == Model::KTF))
        addDialogToList(new PlotDialog(m_parent), "Диаграммы", "plot");
}

void DialogCreator::createRelayDialog()
{
    // Для АВ-ТУК-35 добавляем диалог переключений
    using namespace Modules;
    if (m_typeB == BaseBoard::MTB_35)
        addDialogToList(new RelayDialog(4, m_parent), "Реле", "relay1");
}

void DialogCreator::createCommonDialogs(const AppConfiguration appCfg)
{
    if (m_board.interfaceType() != Interface::IfaceType::Ethernet)
        addDialogToList(new FWUploadDialog(m_parent), "Загрузка ВПО", "upload");
    if (appCfg == AppConfiguration::Debug)
    {
        auto hiddenDialog = new HiddenDialog(m_settings, m_parent);
        hiddenDialog->setModuleName(m_board.moduleName());
        addDialogToList(hiddenDialog, "Секретные операции", "hidden");
    }
    addDialogToList(new TimeDialog(m_parent), "Время", "time");
    addDialogToList(new InfoDialog(m_parent), "О приборе", "info");
}
