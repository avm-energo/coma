#include "settingsdialog.h"

#include "../gen/colors.h"
#include "../gen/eabstractprotocomchannel.h"
#include "../widgets/epushbutton.h"
#include "../widgets/wd_func.h"
#include "config.h"
#include <avm-gen/settings.h>
#include <avm-gen/stdfunc.h>

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
    setStyleSheet(yellowDlgSS);
    setWindowTitle("Настройки");
    SetupUI();
    Fill();
}

void SettingsDialog::SetupUI()
{
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout;
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
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Рабочий каталог программы"), 0);
    hlyout->addWidget(WDFunc::NewLE(this, "pathle"), 1);
    EPushButton *pb = new EPushButton("...");
    connect(pb, SIGNAL(clicked(bool)), this, SLOT(SetHomeDir()));
    hlyout->addWidget(pb, 0);
    vlyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewChB(this, "writelogchb", "Запись обмена данными в файл"));
    vlyout->addLayout(hlyout);
    pb = new EPushButton("Готово");
    connect(pb, SIGNAL(clicked()), this, SLOT(AcceptSettings()));
    vlyout->addWidget(pb);
    setLayout(vlyout);
}

void SettingsDialog::Fill()
{
    QString DevName = Settings::get("PovDevName", "ПКС-1");
    QString DevSN = Settings::get("PovDevSN", "00000001");
    QString DevPrecision = Settings::get("PovDevPrecision", "0.05");
    quint32 PovNumPoints = Settings::get("PovNumPoints", "60");
    WDFunc::SetLEData(this, "orgle", Settings::get("OrganizationString", "Р&К"));
    WDFunc::SetLEData(this, "pathle", Settings::workDir());
    WDFunc::SetLEData(this, "povdev", DevName);
    WDFunc::SetLEData(this, "povdevsn", DevSN);
    WDFunc::SetLEData(this, "povdevprecision", DevPrecision);
    //    QString restring = "^[0-2]{0,1}[0-9]{1,2}{\\.[0-2]{0,1}[0-9]{1,2}}{3}$";
    QString restring = "";
    WDFunc::SetLEData(this, "miple", Settings::get("MIPIP", "172.16.11.12"), restring);
    WDFunc::SetChBData(this, "writelogchb", EAbstractProtocomChannel::IsWriteUSBLog());
    WDFunc::SetSPBData(this, "povnumpoints", PovNumPoints);
}

void SettingsDialog::AcceptSettings()
{
    QString DevName, DevSN, DevPrecision, tmps, dir;
    quint32 PovNumPoints;
    bool tmpb;
    WDFunc::LEData(this, "orgle", tmps);
    Settings::set("OrganizationString", tmps);
    WDFunc::LEData(this, "pathle", dir);
    Settings::setWorkDir(dir);
    WDFunc::LEData(this, "povdev", DevName);
    WDFunc::LEData(this, "povdevsn", DevSN);
    WDFunc::LEData(this, "povdevprecision", DevPrecision);
    WDFunc::LEData(this, "miple", tmps);
    Settings::set("MIPIP", tmps);
    WDFunc::ChBData(this, "writelogchb", tmpb);
    EAbstractProtocomChannel::SetWriteUSBLog(tmpb);
    WDFunc::SPBData(this, "povnumpoints", PovNumPoints);
    Settings::set("PovDevName", DevName);
    Settings::set("PovDevSN", DevSN);
    Settings::set("PovDevPrecision", DevPrecision);
    Settings::set("PovNumPoints", QString::number(PovNumPoints, 10));
    this->close();
}

void SettingsDialog::SetHomeDir()
{
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString dir = dlg->getExistingDirectory(this, "Домашний каталог", Settings::workDir());
    if (!dir.isEmpty())
        WDFunc::SetLEData(this, "pathle", dir);
}
