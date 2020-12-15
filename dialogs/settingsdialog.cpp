#include "settingsdialog.h"

#include "../widgets/styleloader.h"
#include "../widgets/wd_func.h"

#include <QFileDialog>
#include <QMetaEnum>
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
    using namespace Style;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewChB2(this, "writelogchb", "Запись обмена данными в файл"));
    vlyout->addLayout(hlyout);
    vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Степень усреднения для регулировки", "reqcount", true));

    auto themeEnum = QMetaEnum::fromType<Name>;
    QStringList values;
    for (int i = 0; i < themeEnum().keyCount(); i++)
    {
        values.push_back(themeEnum().key(i));
    }
    auto *themeCB = WDFunc::NewCB2(this, values);
    int position = StyleLoader::GetInstance().styleNumber();
    themeCB->setCurrentIndex(position);
    const QString style = StyleLoader::GetInstance().styleFile();
    vlyout->addWidget(themeCB);
    connect(themeCB, &QComboBox::currentTextChanged, [=](const QString &text) {
        Name key = Name(themeEnum().keyToValue(text.toStdString().c_str()));
        auto &styleLoader = StyleLoader::GetInstance();
        styleLoader.setStyleFile(themes.value(key));
        styleLoader.setAppStyleSheet();
        styleLoader.save();
    });

    QPushButton *pb = new QPushButton("Готово");
    connect(pb, &QAbstractButton::clicked, this, &SettingsDialog::AcceptSettings);
    vlyout->addWidget(pb);
    setLayout(vlyout);
}

void SettingsDialog::Fill()
{
    QSettings *sets = new QSettings("EvelSoft", PROGNAME);
    bool writeUSBLog = sets->value("WriteLog", "0").toBool();
    WDFunc::SetChBData(this, "writelogchb", writeUSBLog);
    int N = sets->value("TuneRequestCount", "20").toInt();
    WDFunc::SetLEData(this, "reqcount", QString::number(N));
}

void SettingsDialog::AcceptSettings()
{
    bool tmpb;
    QSettings *sets = new QSettings("EvelSoft", PROGNAME);

    WDFunc::ChBData(this, "writelogchb", tmpb);
    sets->setValue("WriteLog", (tmpb) ? "1" : "0");
    int N = WDFunc::LEData(this, "reqcount").toInt();
    if ((N < 0) || (N > 100))
    {
        N = 20;
        qWarning() << "Неверное число степени усреднения, установлено по умолчанию 20";
    }
    sets->setValue("TuneRequestCount", N);
    this->close();
}
