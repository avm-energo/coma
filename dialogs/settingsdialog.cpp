#include "settingsdialog.h"

#include "../config/config.h"
#include "../gen/stdfunc.h"
#include "../widgets/wd_func.h"

#include <QFileDialog>
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
    WDFunc::SetChBData(this, "writelogchb", sets->value("WriteLog", "0").toBool());
}

void SettingsDialog::AcceptSettings()
{
    bool tmpb;
    QSettings *sets = new QSettings("EvelSoft", PROGNAME);

    WDFunc::ChBData(this, "writelogchb", tmpb);
    sets->setValue("WriteLog", (tmpb) ? "1" : "0");
    this->close();
}
