#include "settingsdialog.h"

#include "../widgets/wd_func.h"

#include <QFileDialog>
#include <QSettings>
#include <QVBoxLayout>
#include <QtDebug>

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
    vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Степень усреднения для регулировки", "N", true));
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
    int N = sets->value("N", "10").toInt();
    WDFunc::SetLEData(this, "N", QString::number(N));
}

void SettingsDialog::AcceptSettings()
{
    bool tmpb;
    QSettings *sets = new QSettings("EvelSoft", PROGNAME);

    WDFunc::ChBData(this, "writelogchb", tmpb);
    sets->setValue("WriteLog", (tmpb) ? "1" : "0");
    int N = WDFunc::LEData(this, "N").toInt();
    if ((N < 0) || (N > 100))
    {
        N = 20;
        qWarning() << "Неверное число степени усреднения, установлено по умолчанию 20";
    }
    sets->setValue("N", N);
    this->close();
}
