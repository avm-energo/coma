#include "settingsdialog.h"

#include "../widgets/epopup.h"
#include "../widgets/styleloader.h"
#include "../widgets/wd_func.h"

#include <QDateTime>
#include <QGroupBox>
#include <QGuiApplication>
#include <QListWidget>
#include <QMessageBox>
#include <QMetaEnum>
#include <QScreen>
#include <QSettings>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTimeZone>
#include <QVBoxLayout>
#include <QtDebug>
#include <gen/settings.h>

using namespace Settings;

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_settings(ApplicationSettings::GetInstance())
    , m_workspace(new QStackedWidget(this))
    , m_sidebar(new QListWidget(this))
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Настройки");
    setupUI();
    fill();
}

void SettingsDialog::setupUI()
{
    int width = 700, height = 500;
    // Настройки окна (размер, положение)
    auto center = QGuiApplication::primaryScreen()->geometry().center();
    setGeometry(center.x() - width / 2, center.y() - height / 2, width, height);
    setFixedSize(this->size());

    auto mainLayout = new QVBoxLayout;
    auto workspaceLayout = new QHBoxLayout;
    connect(m_sidebar, &QListWidget::currentRowChanged, m_workspace, &QStackedWidget::setCurrentIndex);
    setupGeneralTab();
    setupConnectionTab();
    setupTuneTab();
    workspaceLayout->addWidget(m_sidebar, 1);
    workspaceLayout->addWidget(m_workspace, 3);
    mainLayout->addLayout(workspaceLayout);
    auto saveBtn = new QPushButton("Сохранить настройки");
    connect(saveBtn, &QAbstractButton::clicked, this, &SettingsDialog::acceptSettings);
    mainLayout->addWidget(saveBtn);
    setLayout(mainLayout);
}

QVBoxLayout *SettingsDialog::createTabWidget(const QString &tabName)
{
    auto tabWidget = new QGroupBox(tabName, m_workspace);
    auto tabLayout = new QVBoxLayout;
    tabLayout->setAlignment(Qt::AlignTop);
    tabWidget->setLayout(tabLayout);
    m_workspace->addWidget(tabWidget);
    auto item = new QListWidgetItem(tabName, m_sidebar);
    item->setSizeHint(QSize(0, 30));
    item->setTextAlignment(Qt::AlignCenter);
    m_sidebar->addItem(item);
    return tabLayout;
}

void SettingsDialog::setupGeneralTab()
{
    auto tabLayout = createTabWidget("Общие настройки");
    // Изменение темы
    auto themeComboBox = WDFunc::NewCB2(m_workspace, StyleLoader::availableStyles());
    int position = StyleLoader::GetInstance().styleNumber();
    themeComboBox->setCurrentIndex(position);
    connect(themeComboBox, &QComboBox::currentTextChanged, this, &SettingsDialog::themeChanged);
    auto themeLayout = new QHBoxLayout;
    themeLayout->addWidget(new QLabel("Тема", m_workspace));
    themeLayout->addWidget(themeComboBox);
    tabLayout->addLayout(themeLayout);
    tabLayout->addWidget(WDFunc::newHLine(m_workspace));

    // Изменение таймзоны
    auto zoneList = QTimeZone::availableTimeZoneIds();
    QStringList zonestrList;
    std::copy_if(zoneList.cbegin(), zoneList.cend(), std::back_inserter(zonestrList),
        [](const auto array) { return (array.contains("UTC+")); });
    auto timezoneCB = WDFunc::NewCB2(m_workspace, m_settings.nameof(SettingType::Timezone), zonestrList);
    timezoneCB->setCurrentText(m_settings.defaultValue(SettingType::Timezone).toString());
    auto timezoneLayout = new QHBoxLayout;
    timezoneLayout->addWidget(new QLabel("Часовой пояс", m_workspace));
    timezoneLayout->addWidget(timezoneCB);
    tabLayout->addLayout(timezoneLayout);
    tabLayout->addWidget(WDFunc::newHLine(m_workspace));
}

void SettingsDialog::setupConnectionTab()
{
    auto tabLayout = createTabWidget("Настройки соединения");
    // Логгирование обмена данными по интерфейсу
    auto widgetName = m_settings.nameof(SettingType::LoggingEnabled);
    tabLayout->addWidget(WDFunc::NewChB2(m_workspace, widgetName, "Запись обмена данными в файл"));
    tabLayout->addWidget(WDFunc::newHLine(m_workspace));
    // Обновление сигнализации
    widgetName = m_settings.nameof(SettingType::AlarmsEnabled);
    tabLayout->addWidget(WDFunc::NewChB2(m_workspace, widgetName, "Постоянное обновление сигнализации"));

    // Таймаут по HID
    //    tabLayout->addWidget(WDFunc::NewLBLAndLE(m_workspace,
    //        "Таймаут для HID-порта, мс * 100\n"
    //        "Необходимо переподключение для обновления",
    //        regMap[hidTimeout].name, true));
}

void SettingsDialog::setupTuneTab()
{
    auto tabLayout = createTabWidget("Настройки регулировки");
    auto widgetName = m_settings.nameof(SettingType::TuneCount);
    tabLayout->addWidget(WDFunc::NewLBLAndLE(m_workspace, "Степень усреднения для регулировки", widgetName, true));
    widgetName = m_settings.nameof(SettingType::MipIp);
    tabLayout->addWidget(WDFunc::NewLBLAndLE(m_workspace, "IP устройства МИП-02", widgetName, true));
    widgetName = m_settings.nameof(SettingType::MipPort);
    tabLayout->addWidget(WDFunc::NewLBLAndLE(m_workspace, "Порт устройства МИП-02", widgetName, true));
    widgetName = m_settings.nameof(SettingType::MipBsAddress);
    tabLayout->addWidget(WDFunc::NewLBLAndLE(m_workspace, "Адрес устройства МИП-02", widgetName, true));
}

void SettingsDialog::fill()
{
    WDFunc::SetCBData(this, m_settings.nameof(SettingType::Timezone), m_settings.getTimezone());
    WDFunc::SetChBData(this, m_settings.nameof(SettingType::LoggingEnabled), m_settings.getLoggingState());
    WDFunc::SetChBData(this, m_settings.nameof(SettingType::AlarmsEnabled), m_settings.getAlarmsState());
    WDFunc::SetLEData(this, m_settings.nameof(SettingType::TuneCount), m_settings.getTuneCount());
    WDFunc::SetLEData(this, m_settings.nameof(SettingType::MipIp), m_settings.getMipIp());
    WDFunc::SetLEData(this, m_settings.nameof(SettingType::MipPort), m_settings.getMipPort());
    WDFunc::SetLEData(this, m_settings.nameof(SettingType::MipBsAddress), m_settings.getMipBsAddr());

    // int timeout = sets->value(regMap[hidTimeout].name, regMap[hidTimeout].defValue).toInt();
    // WDFunc::SetLEData(this, regMap[hidTimeout].name, QString::number(timeout));
}

void SettingsDialog::acceptSettings()
{
    bool tmpb = false;
    QString timezone = WDFunc::CBData(this, m_settings.nameof(SettingType::Timezone));
    if (!timezone.isEmpty())
        m_settings.setTimezone(timezone);
    if (WDFunc::ChBData(this, m_settings.nameof(SettingType::LoggingEnabled), tmpb))
        m_settings.setLoggingState(tmpb);
    if (WDFunc::ChBData(this, m_settings.nameof(SettingType::AlarmsEnabled), tmpb))
        m_settings.setAlarmsState(tmpb);
    auto tuneCount = WDFunc::LEData(this, m_settings.nameof(SettingType::TuneCount)).toInt(&tmpb);
    if (tmpb)
        m_settings.setTuneCount(tuneCount);
    m_settings.setMipIp(WDFunc::LEData(this, m_settings.nameof(SettingType::MipIp)));
    m_settings.setMipPort(WDFunc::LEData(this, m_settings.nameof(SettingType::MipPort)));
    m_settings.setMipBsAddr(WDFunc::LEData(this, m_settings.nameof(SettingType::MipBsAddress)));
    close();

    //    {
    //        int N = WDFunc::LEData(this, regMap[hidTimeout].name).toInt();
    //        if (N > 100)
    //        {
    //            qWarning() << "Слишком большой таймаут";
    //        }
    //        sets->setValue(regMap[hidTimeout].name, N);
    //    }
}

void SettingsDialog::themeChanged(const QString &newTheme)
{
    auto answer = QMessageBox::question(this, "Предупреждение", //
        "Тема будет изменена\n Приложение может не отвечать некоторое время");
    if (answer == QMessageBox::Yes)
    {
        auto &styleLoader = StyleLoader::GetInstance();
        styleLoader.setStyle(newTheme);
        styleLoader.setAppStyleSheet();
        styleLoader.save();
    }
}
