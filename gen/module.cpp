#include "module.h"

//#include "../dialogs/timedialog.h"
//#include "../dialogs/infodialog.h"
#include "../check/checkdialogkiv.h"
#include "../config/confdialogkiv.h"
#include "../tune/kiv/tunekivdialog.h"

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

Module *Module::createModule(quint32 mtype)
{
    Module *m = new Module;
    S2ConfigType *s2Config = new S2ConfigType;

    switch (mtype)
    {
    case Board::DeviceModel::KIV:
    {
        ConfigKIV *CKIV = new ConfigKIV(s2Config);
        TuneKIV *TKIV = new TuneKIV(0, s2Config);
        m->addDialogToList(new ConfDialogKIV(s2Config), "conf1");
        CheckDialogKIV *cdkiv = new CheckDialogKIV;
        connect(m, &Module::setWarnColor, cdkiv, &AbstractCheckDialog::SetWarnColor);
        connect(m, &Module::setAlarmColor, cdkiv, &AbstractCheckDialog::SetAlarmColor);
        m->addDialogToList(new CheckDialogKIV);
        m->addDialogToList(new TuneKIVDialog(CKIV, TKIV));
        WarnAlarmKIVDialog = new WarnAlarmKIV(Alarm);
    }
    default:
        assert(false);
    }

    //    m->addDialogToList(new TimeDialog);
    //    m->addDialogToList(new InfoDialog);
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
    QList<QDialog *> list;
    foreach (QDialog *dlg, m_Dialogs)
        list.append(dlg);
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
