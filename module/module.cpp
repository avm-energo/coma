#include "module.h"

#include "../check/checkkivdialog.h"
#include "../config/confkivdialog.h"
#include "../dialogs/infodialog.h"
#include "../dialogs/timedialog.h"
#include "../startup/startupkivdialog.h"
#include "../tune/kiv/tunekivdialog.h"
#include "alarmkiv.h"
#include "warnkiv.h"

Module::Module(QObject *parent) : QObject(parent)
{
    m_Dialogs.clear();
    //    m_CheckDialogs.clear();
    //    m_TuneDialogs.clear();
    //    m_confDialogMez = nullptr;
    //    m_confDialogBase = nullptr;
    //    m_checkDialogMez = nullptr;
    //    m_checkDialogBase = nullptr;
    //    m_tuneDialogMez = nullptr;
    //    m_tuneDialogBase = nullptr;
}

Module *Module::createModule()
{
    Module *m = new Module;
    S2ConfigType *s2Config = new S2ConfigType;
    quint16 typeb = Board::GetInstance().typeB();
    if (Board::GetInstance().getBaseBoardsList().contains(typeb)) // there must be two-part module
    {
        quint16 typem = Board::GetInstance().typeM();
        Q_UNUSED(typem)
        switch (typeb)
        {
        case Board::BaseBoards::MTB_00:
        default:
            break;
        }
    }
    else
    {
        quint16 mtype = Board::GetInstance().type();
        switch (mtype)
        {
        case Board::DeviceModel::KIV:
        {
            ConfigKIV *CKIV = new ConfigKIV(s2Config);
            TuneKIV *TKIV = new TuneKIV(0, s2Config);
            m->addDialogToList(new ConfKIVDialog(s2Config), "conf1");
            CheckKIVDialog *cdkiv = new CheckKIVDialog;
            connect(m, &Module::setWarnColor, cdkiv, &AbstractCheckDialog::SetWarnColor);
            connect(m, &Module::setAlarmColor, cdkiv, &AbstractCheckDialog::SetAlarmColor);
            m->addDialogToList(new CheckKIVDialog);
            m->addDialogToList(new TuneKIVDialog(CKIV, TKIV));
            m->addDialogToList(new StartupKIVDialog);
            WarnKIV = new WarnKIV(Alarm);
        }
        default:
            assert(false);
        }
    }

    m->addDialogToList(new TimeDialog, "time");
    m->addDialogToList(new InfoDialog, "info");
    return m;
}

// AbstractConfDialog *Module::confDialogBase()
//{
//    return m_confDialogBase;
//}

// AbstractConfDialog *Module::confDialogMez()
//{
//    return m_confDialogMez;
//}

// QDialog *Module::tuneDialogBase()
//{
//    return m_tuneDialogBase;
//}

// QDialog *Module::tuneDialogMez()
//{
//    return m_tuneDialogMez;
//}

// AbstractCheckDialog *Module::checkDialogBase()
//{
//    return m_checkDialogBase;
//}

// AbstractCheckDialog *Module::checkDialogMez()
//{
//    return m_checkDialogMez;
//}

QList<QDialog *> Module::dialogs()
{
    QList<QDialog *> list = m_Dialogs;
    return list;
}

QList<QDialog *> Module::confDialogs()
{
    QList<QDialog *> list;
    foreach (QDialog *dlg, m_Dialogs)
    {
        if (dlg->objectName().contains("conf"))
            list.append(dlg);
    }
    return list;
}

void Module::addDialogToList(QDialog *dlg, const QString &name)
{
    dlg->setObjectName(name);
    m_Dialogs.append(dlg);
}

QDialog *Module::infoDialog()
{
    foreach (QDialog *dlg, m_Dialogs)
    {
        if (dlg->objectName().contains("info"))
            return dlg;
    }
    return nullptr;
}
