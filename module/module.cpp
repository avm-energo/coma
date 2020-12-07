#include "module.h"

#include "../check/checkkdvdialog.h"
#include "../check/checkkdvharmonicdialog.h"
#include "../check/checkkdvvibrdialog.h"
#include "../check/checkkivdialog.h"
#include "../check/checkktfdialog.h"
#include "../check/checkktfharmonicdialog.h"
#include "../config/confkdvdialog.h"
#include "../config/confkivdialog.h"
#include "../config/confktfdialog.h"
#include "../dialogs/fwuploaddialog.h"
#include "../dialogs/infodialog.h"
#include "../dialogs/journalsdialog.h"
#include "../dialogs/timedialog.h"
#include "../startup/startupkdvdialog.h"
#include "../startup/startupkivdialog.h"
#include "../startup/startupktfdialog.h"
#include "../widgets/udialog.h"
#ifdef AVM_DEBUG
//#include "../tune/kiv/tunekdvdialog.h"
#include "../tune/kiv/tunekivdialog.h"
//#include "../tune/kiv/tunektfdialog.h"
#endif
#include "../gen/s2.h"
#include "alarmkdv.h"
#include "alarmkiv.h"
#include "alarmktf.h"
#include "journkdv.h"
#include "journkiv.h"
#include "journktf.h"
#include "modules.h"
Module::Module(QObject *parent) : QObject(parent)
{
    m_dialogs.clear();
    m_oldTabIndex = m_currentTabIndex = 0;
}

Module *Module::createModule(QTimer *updateTimer, BaseInterface *iface, AlarmWidget *aw)
{
    using namespace Modules;
    const auto &board = Board::GetInstance();
    Journals *JOUR;
    Module *m = new Module;
    m->m_iface = iface;
    S2::config = new S2DataTypes::S2ConfigType;
    m->m_alarmStateAllDialog = new AlarmStateAll;
    m->m_alarmStateAllDialog->UpdateHealth(board.health());
    quint16 typeb = Board::GetInstance().typeB();
    if (BaseBoards.contains(typeb)) // there must be two-part module
    {
        quint16 typem = Board::GetInstance().typeM();
        Q_UNUSED(typem)
        switch (typeb)
        {
        case BaseBoards::MTB_00:
            /*
                str = (checkMDialog == nullptr) ? "Текущие параметры" : "Текущие параметры\nБазовая";
                if (checkBDialog != nullptr)
                {
                    checkBDialog->setMinimumHeight(500);
                    MainTW->addTab(checkBDialog, str);
                    CheckIndex = MainTW->indexOf(checkBDialog);
                }
                str = (checkBDialog == nullptr) ? "Текущие параметры" : "Текущие параметры\nМезонин";
                if (checkMDialog != nullptr)
                    MainTW->addTab(checkMDialog, str);
            */
        default:
            break;
        }
    }
    else
    {
        quint16 mtype = Board::GetInstance().type();
        switch (mtype)
        {
        case Model::KIV:
        {
            JOUR = new JournKIV;
            ConfigKIV *CKIV = new ConfigKIV;
            m->addDialogToList(new ConfKIVDialog(CKIV), "Конфигурирование", "conf1");
            CheckKIVDialog *cdkiv = new CheckKIVDialog;
            m->addDialogToList(cdkiv, "Проверка");
#ifdef AVM_DEBUG
            TuneKIV *TKIV = new TuneKIV;
            m->addDialogToList(new TuneKIVDialog(CKIV, TKIV), "Регулировка");
#endif
            m->addDialogToList(new StartupKIVDialog, "Начальные значения");
            m->m_warnAlarm = new WarnKIV;
            m->m_accAlarm = new CritKIV;
            //            connect(m->m_warn, &Warn::updateWarn, cdkiv, &AbstractCheckDialog::SetWarnColor);
            //            connect(m->m_alarm, &Alarm::updateAlarm, cdkiv, &AbstractCheckDialog::SetAlarmColor);
            break;
        }
        case Model::KTF:
        {
            JOUR = new JournKTF;
            ConfigKTF *CKTF = new ConfigKTF;
            m->addDialogToList(new ConfKTFDialog(CKTF), "Конфигурирование", "conf1");
            CheckKTFDialog *cdktf = new CheckKTFDialog;
            m->addDialogToList(cdktf);
#ifdef AVM_DEBUG
            //            TuneKTF *TKTF = new TuneKTF(0, s2Config);
//                        m->addDialogToList(new TuneKTFDialog(CKTF, TKTF));
#endif
            m->addDialogToList(new StartupKTFDialog, "Старение изоляции");
            m->addDialogToList(new CheckKTFHarmonicDialog, "Гармоники");
            m->m_warnAlarm = new WarnKTF;
            m->m_accAlarm = new CritKTF;
            //            connect(m->m_warn, &Warn::updateWarn, cdktf, &AbstractCheckDialog::SetWarnColor);
            //            connect(m->m_alarm, &Alarm::updateAlarm, cdktf, &AbstractCheckDialog::SetAlarmColor);
            break;
        }
        case Model::KDV:
        {
            JOUR = new JournKDV;
            ConfigKDV *CKDV = new ConfigKDV;
            m->addDialogToList(new ConfKDVDialog(CKDV), "Конфигурирование", "conf1");
            CheckKDVDialog *cdkdv = new CheckKDVDialog;
            m->addDialogToList(cdkdv);
#ifdef AVM_DEBUG
//            TuneKDV *TKDV = new TuneKDV;
//            m->addDialogToList(new TuneKDVDialog(CKDV, TKDV));
#endif
            m->addDialogToList(new StartupKDVDialog, "Старение изоляции");
            m->addDialogToList(new CheckKDVHarmonicDialog, "Гармоники");
            m->addDialogToList(new CheckKDVVibrDialog, "Вибрации");
            //            VibrDialog = new CheckDialogVibrKDV(BoardTypes::BT_BASE);
            //            connect(BdaTimer, &QTimer::timeout, VibrDialog, &AbstractCheckDialog::USBUpdate);
            m->m_warnAlarm = new WarnKDV;
            m->m_accAlarm = new CritKDV;
            //            connect(m->m_warn, &Warn::updateWarn, cdkdv, &AbstractCheckDialog::SetWarnColor);
            //            connect(m->m_alarm, &Alarm::updateAlarm, cdkdv, &AbstractCheckDialog::SetAlarmColor);
            break;
        }
        default:
            assert(false);
        }
    }

    TimeDialog *tdlg = new TimeDialog;
    m->addDialogToList(tdlg, "Время", "time");

    if (Board::GetInstance().interfaceType() != Board::InterfaceType::RS485)
    {
        m->addDialogToList(new JournalDialog(JOUR), "Журналы");
    }
    if (Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
        m->addDialogToList(new FWUploadDialog, "Загрузка ВПО");

    m->addDialogToList(new InfoDialog, "О приборе", "info");

    QList<UDialog *> dlgs = m->dialogs();
    for (auto *d : dlgs)
    {
        connect(updateTimer, &QTimer::timeout, d, &UDialog::reqUpdate);
        d->setUpdatesDisabled();
        d->setInterface(m->m_iface);
    }
    connect(aw, &AlarmWidget::AlarmButtonPressed, m->m_alarmStateAllDialog, &QDialog::show);
    connect(aw, &AlarmWidget::ModuleWarnButtonPressed, m->m_warnAlarm, &QDialog::show);
    connect(aw, &AlarmWidget::ModuleAlarmButtonPressed, m->m_accAlarm, &QDialog::show);
    connect(m->m_warnAlarm, &::BaseAlarm::updateAlarm, aw, &AlarmWidget::updateWarn);
    connect(m->m_accAlarm, &BaseAlarm::updateAlarm, aw, &AlarmWidget::updateAlarm);
    connect(m->m_alarmStateAllDialog, &AlarmStateAll::BSIUpdated, aw, &AlarmWidget::updateMain);
    return m;
}

QList<UDialog *> Module::dialogs()
{
    QList<UDialog *> list = m_dialogs;
    return list;
}

QList<UDialog *> Module::confDialogs()
{
    QList<UDialog *> list;
    foreach (UDialog *dlg, m_dialogs)
    {
        if (dlg->objectName().contains("conf"))
            list.append(dlg);
    }
    return list;
}

void Module::addDialogToList(UDialog *dlg, const QString &caption, const QString &name)
{
    dlg->setObjectName(name);
    dlg->setCaption(caption);
    m_dialogs.append(dlg);
}

BaseAlarm *Module::getAlarm()
{
    return m_accAlarm;
}

BaseAlarm *Module::getWarn()
{
    return m_warnAlarm;
}

AlarmStateAll *Module::getAlarmStateAll()
{
    return m_alarmStateAllDialog;
}

void Module::parentTWTabClicked(int index)
{
    if (index == m_currentTabIndex) // to prevent double function invocation by doubleclicking on tab
        return;
    m_currentTabIndex = index;

    if (m_oldTabIndex >= m_dialogs.size())
    {
        DBGMSG("BdUIList size");
        return;
    }
    UDialog *udlg = m_dialogs.at(m_oldTabIndex);
    //    UDialog *udlg = qobject_cast<UDialog *>(dlg);
    //    if (udlg)
    udlg->setUpdatesDisabled();
    if (m_currentTabIndex >= m_dialogs.size())
    {
        DBGMSG("BdUIList size");
        return;
    }
    udlg = m_dialogs.at(m_currentTabIndex);
    //    udlg = qobject_cast<UDialog *>(dlg);
    //    if (udlg)
    udlg->setUpdatesEnabled();
    udlg->reqUpdate();
    m_oldTabIndex = m_currentTabIndex;
}

// void Module::setDefConf()
//{
//    foreach (QDialog *dlg, m_dialogs)
//    {
//        if (dlg->objectName().contains("conf"))
//        {
//            static_cast<AbstractConfDialog *>(dlg)->SetDefConf();
//            static_cast<AbstractConfDialog *>(dlg)->Fill();
//        }
//    }
//}

void Module::closeDialogs()
{
    for (auto &i : m_dialogs)
        i->close();
}
