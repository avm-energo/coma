#include "settingsdialog.h"

#include "../widgets/wd_func.h"

#include <QFileDialog>
#include <QSettings>
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
    hlyout->addWidget(WDFunc::NewChB(this, "writelogchb", "Запись обмена данными в файл"));
    vlyout->addLayout(hlyout);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb, SIGNAL(clicked()), this, SLOT(AcceptSettings()));
    vlyout->addWidget(pb);
    setLayout(vlyout);
}

void SettingsDialog::Fill()
{
    QSettings *sets = new QSettings("EvelSoft", PROGNAME);
    bool writeUSBLog = sets->value("WriteLog", "0").toBool();
    WDFunc::SetChBData(this, "writelogchb", writeUSBLog);
}

void SettingsDialog::AcceptSettings()
{
    bool tmpb;
    QSettings *sets = new QSettings("EvelSoft", PROGNAME);

    WDFunc::ChBData(this, "writelogchb", tmpb);
    sets->setValue("WriteLog", (tmpb) ? "1" : "0");
    this->close();
}
