#include "settingsdialog.h"

#include "../widgets/styleloader.h"
#include "../widgets/wd_func.h"

#include <QDateTime>
#include <QMessageBox>
#include <QMetaEnum>
#include <QSettings>
#include <QTimeZone>
#include <QVBoxLayout>
#include <QtDebug>
#include <gen/settings.h>

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
    using namespace settings;
    auto vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewChB2(this, regMap[logKey].name, "Запись обмена данными в файл"));
    auto pb = new QPushButton("Выключить обновление сигнализации");
    connect(pb, &QCheckBox::clicked, this, &SettingsDialog::disableAlarmUpdate);
    vlyout->addWidget(pb);
    vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Степень усреднения для регулировки", regMap[tuneCountKey].name, true));
    vlyout->addWidget(WDFunc::NewLBLAndLE(this,
        "Таймаут для HID-порта, мс * 100\n"
        "Необходимо переподключение для обновления",
        regMap[hidTimeout].name, true));
    auto themeEnum = QMetaEnum::fromType<Name>;
    QStringList values;
    for (int i = 0; i < themeEnum().keyCount(); i++)
    {
        values.push_back(themeEnum().key(i));
    }
    auto themeCB = WDFunc::NewCB2(this, values);
    int position = StyleLoader::GetInstance().styleNumber();
    themeCB->setCurrentIndex(position);

    auto hlyout = new QHBoxLayout;
    hlyout->addWidget(new QLabel("Тема", this));
    hlyout->addWidget(themeCB);
    vlyout->addLayout(hlyout);
    connect(themeCB, &QComboBox::currentTextChanged, [=](const QString &text) {
        auto answer = QMessageBox::question(
            this, "Предупреждение", "Тема будет изменена\n Приложение может не отвечать некоторое время");
        if (answer == QMessageBox::Yes)
        {
            auto key = Name(themeEnum().keyToValue(text.toStdString().c_str()));
            auto &styleLoader = StyleLoader::GetInstance();
            styleLoader.setStyleFile(themes.value(key));
            styleLoader.setAppStyleSheet();
            styleLoader.save();
        }
    });

    auto zoneList = QTimeZone::availableTimeZoneIds();
    QStringList zonestrList;
    std::copy_if(zoneList.cbegin(), zoneList.cend(), std::back_inserter(zonestrList),
        [](const auto array) { return (array.contains("UTC+")); });

    auto timezoneCB = WDFunc::NewCB2(this, regMap[timezoneKey].name, zonestrList);
    auto timezone = QTimeZone::systemTimeZone().displayName(QTimeZone::StandardTime, QTimeZone::OffsetName);
    timezoneCB->setCurrentText(timezone);
    hlyout = new QHBoxLayout;
    hlyout->addWidget(new QLabel("Часовой пояс", this));
    hlyout->addWidget(timezoneCB);
    vlyout->addLayout(hlyout);
    vlyout->addWidget(WDFunc::newHLine(this));
    vlyout->addWidget(WDFunc::NewLBLAndLE(this, "IP устройства МИП-02", regMap[MIPIP].name, true));
    vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Адрес устройства МИП-02", regMap[MIPAddress].name, true));
    pb = new QPushButton("Готово");
    connect(pb, &QAbstractButton::clicked, this, &SettingsDialog::AcceptSettings);
    vlyout->addWidget(pb);
    setLayout(vlyout);
}

void SettingsDialog::Fill()
{
    using namespace settings;
    auto sets = std::make_unique<QSettings>();
    bool writeUSBLog = sets->value(regMap[logKey].name, regMap[logKey].defValue).toBool();
    WDFunc::SetChBData(this, regMap[logKey].name, writeUSBLog);
    int N = sets->value(regMap[tuneCountKey].name, regMap[tuneCountKey].defValue).toInt();
    WDFunc::SetLEData(this, regMap[tuneCountKey].name, QString::number(N));
    int timeout = sets->value(regMap[hidTimeout].name, regMap[hidTimeout].defValue).toInt();
    WDFunc::SetLEData(this, regMap[hidTimeout].name, QString::number(timeout));
    QString timezone = QTimeZone::systemTimeZone().displayName(QTimeZone::StandardTime, QTimeZone::OffsetName);
    timezone = sets->value(regMap[timezoneKey].name, regMap[timezoneKey].defValue).toString();
    WDFunc::SetCBData(this, regMap[timezoneKey].name, timezone);
    QString mipip = sets->value(regMap[MIPIP].name, regMap[MIPIP].defValue).toString();
    QString mipaddress = sets->value(regMap[MIPAddress].name, regMap[MIPAddress].defValue).toString();
    WDFunc::SetLEData(this, regMap[MIPAddress].name, mipaddress);
    WDFunc::SetLEData(this, regMap[MIPIP].name, mipip);
}

void SettingsDialog::AcceptSettings()
{
    using namespace settings;
    bool tmpb = false;
    auto sets = std::make_unique<QSettings>();

    WDFunc::ChBData(this, regMap[logKey].name, tmpb);
    sets->setValue(regMap[logKey].name, (tmpb) ? "1" : "0");
    {
        int N = WDFunc::LEData(this, regMap[tuneCountKey].name).toInt();
        if ((N < 0) || (N > 100))
        {
            N = 20;
            qWarning() << "Неверное число степени усреднения, установлено по умолчанию 20";
        }
        sets->setValue(regMap[tuneCountKey].name, N);
    }
    {
        int N = WDFunc::LEData(this, regMap[hidTimeout].name).toInt();
        if (N > 100)
        {
            qWarning() << "Слишком большой таймаут";
        }
        sets->setValue(regMap[hidTimeout].name, N);
    }
    QString timezone = WDFunc::CBData(this, regMap[timezoneKey].name);
    if (!timezone.isEmpty())
        sets->setValue(regMap[timezoneKey].name, timezone);
    sets->setValue(regMap[MIPIP].name, WDFunc::LEData(this, regMap[MIPIP].name));
    sets->setValue(regMap[MIPAddress].name, WDFunc::LEData(this, regMap[MIPAddress].name));
    close();
}
