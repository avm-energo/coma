#include "settingsdialog.h"

#include "../config/config.h"
#include "../gen/maindef.h"
#include "../gen/modulebsi.h"
#include "../gen/stdfunc.h"
#include "../widgets/wd_func.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
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
    //    quint32 MTypeB = ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8;
    /*    if (MTypeB == Config::MTB_A1)
        { */
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
    //    }
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Рабочий каталог программы"), 0);
    hlyout->addWidget(WDFunc::NewLE(this, "pathle"), 1);
    QPushButton *pb = new QPushButton("...");
    connect(pb, SIGNAL(clicked(bool)), this, SLOT(SetHomeDir()));
    hlyout->addWidget(pb, 0);
    vlyout->addLayout(hlyout);
    /*    if (MTypeB == Config::MTB_80)
        { */
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "IP-адрес МИП:"));
    hlyout->addWidget(WDFunc::NewLE(this, "miple"));
    vlyout->addLayout(hlyout);
    //    }
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewChB(this, "writelogchb", "Запись обмена данными в файл"));
    vlyout->addLayout(hlyout);
    pb = new QPushButton("Готово");
    connect(pb, SIGNAL(clicked()), this, SLOT(AcceptSettings()));
    vlyout->addWidget(pb);
    setLayout(vlyout);
}

void SettingsDialog::Fill()
{
    QSettings *sets = new QSettings("EvelSoft", PROGNAME);
    bool tmpb = sets->value("WriteLog", "0").toBool();
    QString DevName = sets->value("PovDevName", "UPTN").toString();
    QString DevSN = sets->value("PovDevSN", "00000001").toString();
    QString DevPrecision = sets->value("PovDevPrecision", "0.05").toString();
    quint32 PovNumPoints = sets->value("PovNumPoints", "60").toUInt();
    WDFunc::LE_write_data(this, StdFunc::OrganizationString(), "orgle");
    WDFunc::LE_write_data(this, StdFunc::GetHomeDir(), "pathle");
    WDFunc::LE_write_data(this, DevName, "povdev");
    WDFunc::LE_write_data(this, DevSN, "povdevsn");
    WDFunc::LE_write_data(this, DevPrecision, "povdevprecision");
    //    QString restring = "^[0-2]{0,1}[0-9]{1,2}{\\.[0-2]{0,1}[0-9]{1,2}}{3}$";
    QString restring = "";
    WDFunc::LE_write_data(this, StdFunc::ForDeviceIP(), "miple");

    WDFunc::SetSPBData(this, "povnumpoints", PovNumPoints);
    WDFunc::SetChBData(this, "writelogchb", tmpb);
}

void SettingsDialog::AcceptSettings()
{
    QString DevName, DevSN, DevPrecision, tmps, dir;
    quint32 PovNumPoints;
    bool tmpb;
    WDFunc::LE_read_data(this, "orgle", tmps);
    StdFunc::SetOrganizationString(tmps);
    WDFunc::LE_read_data(this, "pathle", dir);
    StdFunc::SetHomeDir(dir);
    WDFunc::LE_read_data(this, "povdev", DevName);
    WDFunc::LE_read_data(this, "povdevsn", DevSN);
    WDFunc::LE_read_data(this, "povdevprecision", DevPrecision);
    WDFunc::LE_read_data(this, "miple", tmps);
    StdFunc::SetDeviceIP(tmps);
    WDFunc::ChBData(this, "writelogchb", tmpb);

    WDFunc::SPBData(this, "povnumpoints", PovNumPoints);
    QSettings *sets = new QSettings("EvelSoft", PROGNAME);
    sets->setValue("PovDevName", DevName);
    sets->setValue("PovDevSN", DevSN);
    sets->setValue("PovDevPrecision", DevPrecision);
    sets->setValue("PovNumPoints", QString::number(PovNumPoints, 10));
    sets->setValue("Organization", StdFunc::OrganizationString());
    sets->setValue("DeviceIP", StdFunc::ForDeviceIP());
    sets->setValue("WriteLog", (tmpb) ? "1" : "0");
    this->close();
}

void SettingsDialog::SetHomeDir()
{
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString dir = dlg->getExistingDirectory(this, "Домашний каталог", StdFunc::GetHomeDir());
    if (!dir.isEmpty())
        WDFunc::LE_write_data(this, dir, "pathle");
}
