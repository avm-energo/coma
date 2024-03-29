#include "dialogcreator.h"

#include "../journals/journaldialog.h"
#include "../oscillograms/oscdialog.h"
#include "../oscillograms/osckivdialog.h"
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

#include <device/current_device.h>

using namespace Device;

DialogCreator::DialogCreator(Device::CurrentDevice *device, QWidget *parent)
    : QObject(parent)
    , m_device(device)
    , m_parent(parent)
    , m_boxModel(static_cast<Model>(m_device->getDeviceType()))
    , m_typeB(static_cast<BaseBoard>(m_device->getBaseType()))
    , m_typeM(static_cast<MezzanineBoard>(m_device->getMezzType()))
{
    Q_ASSERT(m_device != nullptr);
}

void DialogCreator::createDialogs(const AppConfiguration appCfg)
{
    auto ifaceType = m_device->async()->getInterfaceType();
    deleteDialogs();
    createConfigDialogs();
    createCheckDialogs();
    createJournalDialog();

    // Регулировка доступна только в АВМ-Наладке при связи по USB
    if (appCfg == AppConfiguration::Debug && ifaceType == Interface::IfaceType::USB)
        createTuneDialogs();
    // TODO: Временно выключено для модбаса
    if (ifaceType == Interface::IfaceType::USB)
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

void DialogCreator::createConfigDialogs()
{
    for (auto &[boardType, boardConf] : *m_device->getS2Datamanager())
    {
        boardConf.setDefaultConfig();
        auto confDialog = new ConfigDialog(m_device, boardType, m_parent);
        const auto &confDialogCaption = boardConf.m_tabName;
        const auto confDialogName = "conf" + QString::number(static_cast<int>(boardType));
        addDialogToList(confDialog, confDialogCaption, confDialogName);
    }
}

void DialogCreator::createCheckDialogs()
{
    auto &sections = m_device->getConfigStorage()->getDeviceSettings().getSections();
    auto index = 0;
    for (auto &section : sections)
    {
        auto indexStr = QString::number(index);
        auto checkDialog = new CheckDialog(section, m_device, m_parent);
        addDialogToList(checkDialog, section.name, "check" + indexStr);
        index++;
    }
}

void DialogCreator::createJournalDialog()
{
    using namespace journals;
    addDialogToList(new JournalDialog(m_device, m_parent), "Журналы", "jours");
}

bool DialogCreator::isS2Available() noexcept
{
    try
    {
        [[maybe_unused]] volatile auto s2manager = m_device->getS2Datamanager();
        [[maybe_unused]] volatile auto &workConfig = s2manager->getCurrentConfiguration().m_workingConfig;
        return true;
    } catch (...)
    {
        return false;
    }
}

void DialogCreator::createTuneDialogs()
{
    if (!isS2Available())
    {
        qCritical() << "Ошибка создания диалога регулировки";
        return;
    }

    GeneralTuneDialog *tuneDlg = nullptr;
    switch (m_boxModel)
    {
    case Model::KIV:
        tuneDlg = new TuneKIVDialog(m_device, m_parent);
        break;
    case Model::KTF:
        /// TODO: Регулировка КТФ
        break;
    case Model::KDV:
        /// TODO: Регулировка КДВ
        break;
    default:
        if (m_typeB == BaseBoard::MTB_80)
        {
            if (m_typeM == Device::MezzanineBoard::MTM_81 || m_typeM == MezzanineBoard::MTM_82
                || m_typeM == MezzanineBoard::MTM_83)
                tuneDlg = new Tune82Dialog(m_device, m_parent);
            else if (m_typeM == MezzanineBoard::MTM_84)
                tuneDlg = new TuneKIVDialog(m_device, m_parent);
        }
        break;
    }
    if (tuneDlg != nullptr)
        addDialogToList(tuneDlg, "Регулировка", "tune");
}

void DialogCreator::createStartupValuesDialog()
{
    // Добавляем диалог начальных значений
    AbstractStartupDialog *startupDlg = nullptr;
    switch (m_boxModel)
    {
    case Model::KIV:
        startupDlg = new StartupKIVDialog(m_device, m_parent);
        break;
    case Model::KTF:
        startupDlg = new StartupKTFDialog(m_device, m_parent);
        break;
    case Model::KDV:
        startupDlg = new StartupKTFDialog(m_device, m_parent);
        break;
    default:
        // У АВ-ТУК-84 те же начальные значение, что и у АВМ-КИВ
        if (m_typeB == BaseBoard::MTB_80 && m_typeM == MezzanineBoard::MTM_84)
            startupDlg = new StartupKIVDialog(m_device, m_parent);
        break;
    }
    if (startupDlg != nullptr)
        addDialogToList(startupDlg, "Начальные\nзначения", "startup");
}

void DialogCreator::createOscAndSwJourDialogs()
{
    // Добавляем диалоги осциллограмм и журналов переключений
    if (m_typeB == BaseBoard::MTB_80)
    {
        if (m_typeM == MezzanineBoard::MTM_81 || m_typeM == MezzanineBoard::MTM_82 || m_typeM == MezzanineBoard::MTM_83)
            addDialogToList(new OscDialog(m_device, m_parent), "Осциллограммы", "osc");
    }
    else if ((m_typeB == BaseBoard::MTB_80 || m_typeB == BaseBoard::MTB_85) && (m_typeM == MezzanineBoard::MTM_85))
    {
        addDialogToList(new SwitchJournalDialog(m_device, m_parent), "Журнал переключений", "swjour");
        addDialogToList(new OscDialog(m_device, m_parent), "Осциллограммы", "osc");
    }
    else if (m_boxModel == Model::KIV)
    {
        addDialogToList(new OscKivDialog(m_device, m_parent), "Осциллограммы", "osc");
    }
}

void DialogCreator::createPlotDialog()
{
    // Только для АВ-ТУК-82 и АВМ-КТФ добавляем диалог с векторными диаграммами
    if ((m_typeB == BaseBoard::MTB_80 && m_typeM == MezzanineBoard::MTM_82) || (m_boxModel == Model::KTF))
        addDialogToList(new PlotDialog(m_device, m_parent), "Диаграммы", "plot");
}

void DialogCreator::createRelayDialog()
{
    // Для АВ-ТУК-35 добавляем диалог переключений
    if (m_typeB == BaseBoard::MTB_35)
        addDialogToList(new RelayDialog(4, m_device, m_parent), "Реле", "relay1");
}

void DialogCreator::createCommonDialogs(const AppConfiguration appCfg)
{
    auto ifaceType = m_device->async()->getInterfaceType();
    if (ifaceType != Interface::IfaceType::Ethernet)
        addDialogToList(new FWUploadDialog(m_device, m_parent), "Загрузка ВПО", "upload");
    if (appCfg == AppConfiguration::Debug)
    {
        auto hiddenDialog = new HiddenDialog(m_device, m_parent);
        hiddenDialog->setModuleName(m_device->getDeviceName());
        addDialogToList(hiddenDialog, "Секретные операции", "hidden");
    }
    addDialogToList(new TimeDialog(m_device, m_parent), "Время", "time");
    addDialogToList(new InfoDialog(m_device, m_parent), "О приборе", "info");
}
