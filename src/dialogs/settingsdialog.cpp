#include "settingsdialog.h"

#include "../gen/settings.h"
#include "../widgets/styleloader.h"
#include "../widgets/wd_func.h"

#include <QDateTime>
#include <QMessageBox>
#include <QMetaEnum>
#include <QSettings>
#include <QTimeZone>
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
    vlyout->addWidget(WDFunc::NewChB2(this, settings::logWidget, "Запись обмена данными в файл"));
    auto pb = new QPushButton("Выключить обновление сигнализации");
    connect(pb, &QCheckBox::clicked, this, &SettingsDialog::disableAlarmUpdate);
    vlyout->addWidget(pb);
    vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Степень усреднения для регулировки", settings::tuneCountWidget, true));
    vlyout->addWidget(WDFunc::NewLBLAndLE(this,
        "Таймаут для HID-порта, мс * 100\n"
        "Необходимо переподкючение для обновления",
        settings::hidTimeout, true));
    auto themeEnum = QMetaEnum::fromType<Name>;
    QStringList values;
    for (int i = 0; i < themeEnum().keyCount(); i++)
    {
        values.push_back(themeEnum().key(i));
    }
    auto *themeCB = WDFunc::NewCB2(this, values);
    int position = StyleLoader::GetInstance().styleNumber();
    themeCB->setCurrentIndex(position);

    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(new QLabel("Тема", this));
    hlyout->addWidget(themeCB);
    vlyout->addLayout(hlyout);
    connect(themeCB, &QComboBox::currentTextChanged, [=](const QString &text) {
        auto answer = QMessageBox::question(
            this, "Предупреждение", "Тема будет изменена\n Приложение может не отвечать некоторое время");
        if (answer == QMessageBox::No)
            return;
        Name key = Name(themeEnum().keyToValue(text.toStdString().c_str()));
        auto &styleLoader = StyleLoader::GetInstance();
        styleLoader.setStyleFile(themes.value(key));
        styleLoader.setAppStyleSheet();
        styleLoader.save();
    });
    QList<QByteArray> zoneList = QTimeZone::availableTimeZoneIds();

    QStringList zonestrList;
    std::copy_if(zoneList.cbegin(), zoneList.cend(), std::back_inserter(zonestrList),
        [](const auto array) { return (array.contains("UTC+")); });

    auto *timezoneCB = WDFunc::NewCB2(this, settings::timezoneWidget, zonestrList);
    QString timezone = QTimeZone::systemTimeZone().displayName(QTimeZone::StandardTime, QTimeZone::OffsetName);
    timezoneCB->setCurrentText(timezone);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(new QLabel("Часовой пояс", this));
    hlyout->addWidget(timezoneCB);
    vlyout->addLayout(hlyout);
    pb = new QPushButton("Готово");
    connect(pb, &QAbstractButton::clicked, this, &SettingsDialog::AcceptSettings);
    vlyout->addWidget(pb);
    setLayout(vlyout);
}

void SettingsDialog::Fill()
{
    auto sets = std::make_unique<QSettings>();
    bool writeUSBLog = sets->value(settings::logKey, "0").toBool();
    WDFunc::SetChBData(this, settings::logWidget, writeUSBLog);
    int N = sets->value(settings::tuneCountKey, "20").toInt();
    WDFunc::SetLEData(this, settings::tuneCountWidget, QString::number(N));
    int timeout = sets->value(settings::hidTimeout, "50").toInt();
    WDFunc::SetLEData(this, settings::hidTimeout, QString::number(timeout));
    QString timezone = QTimeZone::systemTimeZone().displayName(QTimeZone::StandardTime, QTimeZone::OffsetName);
    timezone = sets->value(settings::timezoneKey, timezone).toString();
    WDFunc::SetCBData(this, settings::timezoneWidget, timezone);
}

void SettingsDialog::AcceptSettings()
{
    bool tmpb = false;
    auto sets = std::make_unique<QSettings>();

    WDFunc::ChBData(this, settings::logWidget, tmpb);
    sets->setValue(settings::logKey, (tmpb) ? "1" : "0");
    {
        int N = WDFunc::LEData(this, settings::tuneCountWidget).toInt();
        if ((N < 0) || (N > 100))
        {
            N = 20;
            qWarning() << "Неверное число степени усреднения, установлено по умолчанию 20";
        }
        sets->setValue(settings::tuneCountKey, N);
    }
    {
        int N = WDFunc::LEData(this, settings::hidTimeout).toInt();
        if (N > 100)
        {
            qWarning() << "Слишком большой таймаут";
        }
        sets->setValue(settings::hidTimeout, N);
    }
    QString timezone = WDFunc::CBData(this, settings::timezoneWidget);
    if (!timezone.isEmpty())
        sets->setValue(settings::timezoneKey, timezone);
    close();
}
