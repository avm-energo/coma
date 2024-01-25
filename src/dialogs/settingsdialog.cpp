#include "settingsdialog.h"

#include "../widgets/epopup.h"
#include "../widgets/styleloader.h"
#include "../widgets/wd_func.h"

#include <QGroupBox>
#include <QGuiApplication>
#include <QListWidget>
#include <QMessageBox>
#include <QScreen>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTimeZone>
#include <QVBoxLayout>
#include <QtDebug>

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

QVBoxLayout *SettingsDialog::createWorkspaceLayout(const QString &tabName)
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

QTabWidget *SettingsDialog::createTabWidget(QVBoxLayout *layout)
{
    auto tabWidget = new QTabWidget(m_workspace);
    layout->addWidget(tabWidget);
    return tabWidget;
}

QVBoxLayout *SettingsDialog::createTabLayout(QTabWidget *destination, const QString &tabName)
{
    auto tab = new QWidget(destination);
    auto tabLayout = new QVBoxLayout;
    tabLayout->setAlignment(Qt::AlignTop);
    tab->setLayout(tabLayout);
    destination->addTab(tab, tabName);
    return tabLayout;
}

void SettingsDialog::setupGeneralTab() noexcept
{
    auto workspaceLayout = createWorkspaceLayout("Общие настройки");
    // Изменение темы
    auto themeComboBox = WDFunc::NewCB2(m_workspace, StyleLoader::availableStyles());
    int position = StyleLoader::GetInstance().styleNumber();
    themeComboBox->setCurrentIndex(position);
    connect(themeComboBox, &QComboBox::currentTextChanged, this, &SettingsDialog::themeChanged);
    auto themeLayout = new QHBoxLayout;
    themeLayout->addWidget(new QLabel("Тема", m_workspace));
    themeLayout->addWidget(themeComboBox);
    workspaceLayout->addLayout(themeLayout);
    workspaceLayout->addWidget(WDFunc::newHLine(m_workspace));

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
    workspaceLayout->addLayout(timezoneLayout);
    workspaceLayout->addWidget(WDFunc::newHLine(m_workspace));
}

void SettingsDialog::setupConnectionTab() noexcept
{
    auto setupTabs = createTabWidget(createWorkspaceLayout("Настройки соединения"));
    // Вкладка "Общие"
    auto generalLayout = createTabLayout(setupTabs, "Общие");
    auto widgetName = m_settings.nameof(SettingType::LoggingEnabled);
    generalLayout->addWidget(WDFunc::NewChB2(m_workspace, widgetName, "Запись обмена данными в файл"));
    generalLayout->addWidget(WDFunc::newHLine(m_workspace));
    widgetName = m_settings.nameof(SettingType::AlarmsEnabled);
    generalLayout->addWidget(WDFunc::NewChB2(m_workspace, widgetName, "Постоянное обновление сигнализации"));

    // Вкладка "USB HID"
    auto protocomLayout = createTabLayout(setupTabs, "USB HID");
    /// TODO
    Q_UNUSED(protocomLayout);

    // Вкладка "Modbus"
    auto modbusLayout = createTabLayout(setupTabs, "Modbus");
    /// TODO
    Q_UNUSED(modbusLayout);

    // Вкладка "МЭК 61850-104"
    auto iec104Layout = createTabLayout(setupTabs, "МЭК 61850-104");
    /// TODO
    Q_UNUSED(iec104Layout);

    // Таймаут по HID
    //    tabLayout->addWidget(WDFunc::NewLBLAndLE(m_workspace,
    //        "Таймаут для HID-порта, мс * 100\n"
    //        "Необходимо переподключение для обновления",
    //        regMap[hidTimeout].name, true));
}

void SettingsDialog::setupTuneTab() noexcept
{
    auto tuneTabs = createTabWidget(createWorkspaceLayout("Настройки регулировки"));
    // Вкладка "Общие"
    auto generalLayout = createTabLayout(tuneTabs, "Общие");
    auto widgetName = m_settings.nameof(SettingType::TuneCount);
    generalLayout->addWidget(WDFunc::NewLBLAndLE(m_workspace, "Степень усреднения для регулировки", widgetName, true));
    // Вкладка "МИП-02"
    auto mipLayout = createTabLayout(tuneTabs, "МИП-02");
    widgetName = m_settings.nameof(SettingType::MipIp);
    mipLayout->addWidget(WDFunc::NewLBLAndLE(m_workspace, "IP адрес устройства", widgetName, true));
    mipLayout->addWidget(WDFunc::newHLine(m_workspace));
    widgetName = m_settings.nameof(SettingType::MipPort);
    mipLayout->addWidget(WDFunc::NewLBLAndLE(m_workspace, "Порт устройства", widgetName, true));
    mipLayout->addWidget(WDFunc::newHLine(m_workspace));
    widgetName = m_settings.nameof(SettingType::MipBsAddress);
    mipLayout->addWidget(WDFunc::NewLBLAndLE(m_workspace, "Адрес базовой станции", widgetName, true));
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
