#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QFileDialog>
#include <QPushButton>
#include <QString>
#include "settingsdialog.h"
#include "../config/config.h"
#include "../widgets/s_tqspinbox.h"
#include "../widgets/wd_func.h"
#include "../publicclass.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Настройки");
    SetupUI();
    Fill();
}

void SettingsDialog::SetupUI()
{
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLabel *lbl;
    QLineEdit *le;
    if (pc.ModuleBsi.MTypeB == MTB_A1)
    {
        hlyout = new QHBoxLayout;
        lbl = new QLabel("Наименование организации, эксплуатирующей прибор");
        hlyout->addWidget(lbl);
        le = new QLineEdit;
        le->setObjectName("orgle");
        hlyout->addWidget(le);
        vlyout->addLayout(hlyout);
    }
    hlyout = new QHBoxLayout;
    lbl = new QLabel("Рабочий каталог программы");
    hlyout->addWidget(lbl, 0);
    le = new QLineEdit;
    le->setObjectName("pathle");
    hlyout->addWidget(le, 1);
    QPushButton *pb = new QPushButton("...");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SetHomeDir()));
    hlyout->addWidget(pb, 0);
    vlyout->addLayout(hlyout);
    if (pc.ModuleBsi.MTypeB == MTB_80)
    {
        hlyout = new QHBoxLayout;
        lbl = new QLabel("IP-адрес МИП:");
        hlyout->addWidget(lbl);
        le = new QLineEdit;
        le->setObjectName("miple");
        hlyout->addWidget(le);
        vlyout->addLayout(hlyout);
    }
    pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),this,SLOT(AcceptSettings()));
    vlyout->addWidget(pb);
    setLayout(vlyout);
}

void SettingsDialog::Fill()
{
    if (pc.ModuleBsi.MTypeB == MTB_A1)
        WDFunc::SetLEData(this,"orgle",pc.OrganizationString);
    WDFunc::SetLEData(this,"pathle",pc.HomeDir);
    if (pc.ModuleBsi.MTypeB == MTB_80)
    {
        QString restring = "^[0-2]{0,1}[0-9]{1,2}{\\.[0-2]{0,1}[0-9]{1,2}}{3}$";
        WDFunc::SetLEData(this,"miple",pc.MIPIP,restring);
    }
}

void SettingsDialog::AcceptSettings()
{
    if (pc.ModuleBsi.MTypeB == MTB_A1)
        WDFunc::LEData(this, "orgle", pc.OrganizationString);
    WDFunc::LEData(this, "pathle", pc.HomeDir);
    if (pc.ModuleBsi.MTypeB == MTB_80)
        WDFunc::LEData(this, "miple", pc.MIPIP);
    this->close();
}

void SettingsDialog::SetHomeDir()
{
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString dir = dlg->getExistingDirectory(this, "Домашний каталог", pc.HomeDir);
    if (!dir.isEmpty())
        WDFunc::SetLEData(this,"pathle",dir);
}
