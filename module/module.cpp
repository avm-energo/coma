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
    // m_oldTabIndex = m_currentTabIndex = 0;
}

// Module *Module::createModule(QTimer *updateTimer, BaseInterface *iface, AlarmWidget *aw)
Module *Module::createModule(QTimer *updateTimer, AlarmWidget *aw)
{
    using namespace Modules;
    const auto &board = Board::GetInstance();
    Journals *JOUR = nullptr;
    Module *m = new Module;
    //    m->m_iface = iface;
    S2::config = new S2DataTypes::S2ConfigType;
    // ModuleAlarm *criticalAlarm;

    // ModuleAlarm *warningAlarm;
    // m->m_alarmStateAllDialog->UpdateHealth(board.health());
    //    quint16 typeb = Board::GetInstance().typeB();
    quint16 typeb = board.typeB();
    //    aw->setInterface(iface);
    // aw->uponInterfaceSetting();
    AlarmStateAll *alarmStateAll = new AlarmStateAll;

    aw->addAlarm(alarmStateAll);
    if (BaseBoards.contains(typeb)) // there must be two-part module
    {
        //        quint16 typem = Board::GetInstance().typeM();
        quint16 typem = board.typeM();
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
        //        quint16 mtype = Board::GetInstance().type();
        quint16 mtype = board.type();
        switch (mtype)
        {
        case Model::KIV:
        {
            JOUR = new JournKIV(m);
            ConfigKIV *CKIV = new ConfigKIV;
            m->addDialogToList(new ConfKIVDialog(CKIV), "Конфигурирование", "conf1");
            CheckKIVDialog *cdkiv = new CheckKIVDialog;
            m->addDialogToList(cdkiv, "Проверка");
#ifdef AVM_DEBUG
//            TuneKIV *TKIV = new TuneKIV;
            m->addDialogToList(new TuneKIVDialog(CKIV), "Регулировка");
#endif
            m->addDialogToList(new StartupKIVDialog, "Начальные значения");
            aw->addAlarm(new WarnKIV);
            aw->addAlarm(new CritKIV);
            //            connect(m->m_warn, &Warn::updateWarn, cdkiv, &AbstractCheckDialog::SetWarnColor);
            //            connect(m->m_alarm, &Alarm::updateAlarm, cdkiv, &AbstractCheckDialog::SetAlarmColor);
            break;
        }
        case Model::KTF:
        {
            JOUR = new JournKTF(m);
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
            aw->addAlarm(new WarnKTF);
            aw->addAlarm(new CritKTF);
            //            connect(m->m_warn, &Warn::updateWarn, cdktf, &AbstractCheckDialog::SetWarnColor);
            //            connect(m->m_alarm, &Alarm::updateAlarm, cdktf, &AbstractCheckDialog::SetAlarmColor);
            break;
        }
        case Model::KDV:
        {
            JOUR = new JournKDV(m);
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
            aw->addAlarm(new WarnKDV);
            aw->addAlarm(new CritKDV);
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

    //    if (Board::GetInstance().interfaceType() != Board::InterfaceType::RS485)
    if (board.interfaceType() != Board::InterfaceType::RS485)
    {
        Q_ASSERT(JOUR != nullptr);
        m->addDialogToList(new JournalDialog(JOUR), "Журналы");
    }
    else
        delete JOUR;
    //    if (Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
    if (board.interfaceType() != Board::InterfaceType::RS485)
        m->addDialogToList(new FWUploadDialog, "Загрузка ВПО");

    m->addDialogToList(new InfoDialog, "О приборе", "info");

    QList<UDialog *> dlgs = m->dialogs();
    for (auto *d : dlgs)
    {
        connect(updateTimer, &QTimer::timeout, d, &UDialog::reqUpdate);
        d->setUpdatesDisabled();
        d->uponInterfaceSetting();
        //        d->setInterface(m->m_iface);
    }

    //    aw->addAlarm(warningAlarm);
    //    aw->addAlarm(criticalAlarm);

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
    for (UDialog *dlg : m_dialogs)
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

// ModuleAlarm *Module::getAlarm()
//{
//    return m_accAlarm;
//}

// ModuleAlarm *Module::getWarn()
//{
//    return m_warnAlarm;
//}

// AlarmStateAll *Module::getAlarmStateAll()
//{
//    return m_alarmStateAllDialog;
//}

void Module::parentTWTabChanged(int index)
{
    if (index == -1)
        return;
    //    if (index == m_currentTabIndex) // to prevent double function invocation by doubleclicking on tab
    //        return;
    //    m_currentTabIndex = index;

    //    if (m_oldTabIndex >= m_dialogs.size())
    //    {
    //        DBGMSG("BdUIList size");
    //        return;
    //    }
    // UDialog *udlg = m_dialogs.at(m_oldTabIndex);
    for (auto &dialog : m_dialogs)
    {
        dialog->setUpdatesDisabled();
        dialog->setEnabled(false);
    }
    //    UDialog *udlg = qobject_cast<UDialog *>(dlg);
    //    if (udlg)
    // udlg->setUpdatesDisabled();
    //    if (m_currentTabIndex >= m_dialogs.size())
    //    {
    //        DBGMSG("BdUIList size");
    //        return;
    //    }
    UDialog *udlg = m_dialogs.at(index);
    //    udlg = qobject_cast<UDialog *>(dlg);
    //    if (udlg)
    udlg->setEnabled(true);
    udlg->setUpdatesEnabled();
    udlg->reqUpdate();
    // m_oldTabIndex = m_currentTabIndex;
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
