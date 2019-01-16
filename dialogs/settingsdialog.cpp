#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QFileDialog>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include "settingsdialog.h"
#include "../config/config.h"
#include "../gen/stdfunc.h"
#include "../gen/modulebsi.h"
#include "../gen/eabstractprotocomchannel.h"
#include "../widgets/wd_func.h"
#include "../gen/maindef.h"

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
    quint32 MTypeB = ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8;
    if (MTypeB == Config::MTB_A1)
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
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBLT(this, "Количество точек усреднения для поверки"), 0);
        hlyout->addWidget(WDFunc::NewSPB(this, "povnumpoints", 1, 65535, 0), 1);
        vlyout->addLayout(hlyout);
    }
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Рабочий каталог программы"), 0);
    hlyout->addWidget(WDFunc::NewLE(this, "pathle"), 1);
    QPushButton *pb = new QPushButton("...");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SetHomeDir()));
    hlyout->addWidget(pb, 0);
    vlyout->addLayout(hlyout);
    if (MTypeB == Config::MTB_80)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(this, "IP-адрес МИП:"));
        hlyout->addWidget(WDFunc::NewLE(this, "miple"));
        vlyout->addLayout(hlyout);
    }
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewChB(this, "writelogchb", "Запись обмена данными в файл"));
    vlyout->addLayout(hlyout);
    pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),this,SLOT(AcceptSettings()));
    vlyout->addWidget(pb);
    setLayout(vlyout);
}

void SettingsDialog::Fill()
{
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    QString DevName = sets->value("PovDevName", "UPTN").toString();
    QString DevSN = sets->value("PovDevSN", "00000001").toString();
    QString DevPrecision = sets->value("PovDevPrecision", "0.05").toString();
    quint32 PovNumPoints = sets->value("PovNumPoints", "60").toUInt();
    QString OrganizationString = sets->value("Organization", "Р&К").toString();
    WDFunc::SetLEData(this,"orgle", OrganizationString);
    WDFunc::SetLEData(this,"pathle",StdFunc::GetHomeDir());
    WDFunc::SetLEData(this,"povdev", DevName);
    WDFunc::SetLEData(this,"povdevsn", DevSN);
    WDFunc::SetLEData(this,"povdevprecision", DevPrecision);
//    QString restring = "^[0-2]{0,1}[0-9]{1,2}{\\.[0-2]{0,1}[0-9]{1,2}}{3}$";
    QString restring = "";
    WDFunc::SetLEData(this,"miple",StdFunc::MIPIP,restring);
#if PROGSIZE != PROGSIZE_EMUL
    WDFunc::SetChBData(this, "writelogchb", EAbstractProtocomChannel::IsWriteUSBLog());
#endif
    WDFunc::SetSPBData(this, "povnumpoints", PovNumPoints);
}

void SettingsDialog::AcceptSettings()
{
    QString DevName, DevSN, DevPrecision, OrganizationString, dir;
    quint32 PovNumPoints;
    bool tmpb;
    WDFunc::LEData(this, "orgle", OrganizationString);
    WDFunc::LEData(this, "pathle", dir);
    StdFunc::SetHomeDir(dir);
    WDFunc::LEData(this, "povdev", DevName);
    WDFunc::LEData(this, "povdevsn", DevSN);
    WDFunc::LEData(this, "povdevprecision", DevPrecision);
    WDFunc::LEData(this, "miple", StdFunc::MIPIP);
    WDFunc::ChBData(this, "writelogchb", tmpb);
#if PROGSIZE != PROGSIZE_EMUL
    EAbstractProtocomChannel::SetWriteUSBLog(tmpb);
#endif
    WDFunc::SPBData(this, "povnumpoints", PovNumPoints);
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    sets->setValue("PovDevName", DevName);
    sets->setValue("PovDevSN", DevSN);
    sets->setValue("PovDevPrecision", DevPrecision);
    sets->setValue("PovNumPoints", QString::number(PovNumPoints, 10));
    sets->setValue("Organization", OrganizationString);
    sets->setValue("MIPIP", StdFunc::MIPIP);
    this->close();
}

void SettingsDialog::SetHomeDir()
{
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString dir = dlg->getExistingDirectory(this, "Домашний каталог", StdFunc::GetHomeDir());
    if (!dir.isEmpty())
        WDFunc::SetLEData(this,"pathle",dir);
}
