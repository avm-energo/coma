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
#include "../gen/publicclass.h"
#include "../widgets/wd_func.h"

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
/*    quint32 MTypeB = pc.ModuleBsi.MTypeB << 8;
    if (MTypeB == MTB_A1)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBLT(this, "Наименование организации, эксплуатирующей прибор"));
        hlyout->addWidget(WDFunc::NewLE(this, "orgle"));
        vlyout->addLayout(hlyout);
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBLT(this, "Наименование средства поверки"));
        hlyout->addWidget(WDFunc::NewLE(this, "povdev"));
        vlyout->addLayout(hlyout);
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBLT(this, "Заводской номер средства поверки"));
        hlyout->addWidget(WDFunc::NewLE(this, "povdevsn"));
        vlyout->addLayout(hlyout);
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBLT(this, "Класс точности средства поверки"));
        hlyout->addWidget(WDFunc::NewLE(this, "povdevprecision"));
        vlyout->addLayout(hlyout);
    } */
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Рабочий каталог программы"), 0);
    hlyout->addWidget(WDFunc::NewLE(this, "pathle"), 1);
    QPushButton *pb = new QPushButton("...");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SetHomeDir()));
    hlyout->addWidget(pb, 0);
    vlyout->addLayout(hlyout);
    if (pc.ModuleBsi.MTypeB == MTB_80)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(this, "IP-адрес МИП:"));
        hlyout->addWidget(WDFunc::NewLE(this, "miple"));
        vlyout->addLayout(hlyout);
    }
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewChB(this, "writelogchb", "Запись обмена данными в файл"));
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBLT(this, "Количество точек усреднения для поверки"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this, "povnumpoints", 1, 65535, 0), 1);
    vlyout->addLayout(hlyout);
    pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),this,SLOT(AcceptSettings()));
    vlyout->addWidget(pb);
    setLayout(vlyout);
}

void SettingsDialog::Fill()
{
//    WDFunc::SetLEData(this,"orgle",pc.OrganizationString);
    WDFunc::SetLEData(this,"pathle",pc.HomeDir);
/*    WDFunc::SetLEData(this,"povdev",pc.PovDev.DevName);
    WDFunc::SetLEData(this,"povdevsn",pc.PovDev.DevSN);
    WDFunc::SetLEData(this,"povdevprecision",pc.PovDev.DevPrecision); */
    QString restring = "^[0-2]{0,1}[0-9]{1,2}{\\.[0-2]{0,1}[0-9]{1,2}}{3}$";
    WDFunc::SetLEData(this,"miple",pc.MIPIP,restring);
    WDFunc::SetChBData(this, "writelogchb", pc.WriteUSBLog);
//    WDFunc::SetSPBData(this, "povnumpoints", pc.PovNumPoints);
}

void SettingsDialog::AcceptSettings()
{
//    WDFunc::LEData(this, "orgle", pc.OrganizationString);
    WDFunc::LEData(this, "pathle", pc.HomeDir);
/*    WDFunc::LEData(this, "povdev", pc.PovDev.DevName);
    WDFunc::LEData(this, "povdevsn", pc.PovDev.DevSN);
    WDFunc::LEData(this, "povdevprecision", pc.PovDev.DevPrecision); */
    WDFunc::LEData(this, "miple", pc.MIPIP);
    WDFunc::ChBData(this, "writelogchb", pc.WriteUSBLog);
//    WDFunc::SPBData(this, "povnumpoints", pc.PovNumPoints);
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
