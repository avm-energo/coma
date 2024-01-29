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
    , m_settings(UserSettings::GetInstance())
    , m_workspace(new QStackedWidget(this))
    , m_sidebar(new QListWidget(this))
{
    m_settings.native().beginGroup("settings");
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Настройки");
    setupUI();
    fill();
}

SettingsDialog::~SettingsDialog() noexcept
{
    m_settings.native().endGroup();
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
    auto timezoneCB = WDFunc::NewCB2(m_workspace, m_settings.nameof<Timezone>(), zonestrList);
    timezoneCB->setCurrentText(m_settings.defaultValue(Timezone).toString());
    auto timezoneLayout = new QHBoxLayout;
    timezoneLayout->addWidget(new QLabel("Часовой пояс", m_workspace));
    timezoneLayout->addWidget(timezoneCB);
    workspaceLayout->addLayout(timezoneLayout);
}

void SettingsDialog::setupConnectionTab() noexcept
{
    auto setupTabs = createTabWidget(createWorkspaceLayout("Настройки соединения"));
    // Вкладка "Общие"
    auto generalLayout = createTabLayout(setupTabs, "Общие");
    auto widgetName = m_settings.nameof<LoggingEnabled>();
    generalLayout->addWidget(WDFunc::NewChB2(m_workspace, widgetName, "Запись обмена данными в файл"));
    generalLayout->addWidget(WDFunc::newHLine(m_workspace));
    widgetName = m_settings.nameof<AlarmsEnabled>();
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
}

void SettingsDialog::setupTuneTab() noexcept
{
    auto tuneTabs = createTabWidget(createWorkspaceLayout("Настройки регулировки"));
    // Вкладка "Общие"
    auto generalLayout = createTabLayout(tuneTabs, "Общие");
    auto widgetName = m_settings.nameof<TuneCount>();
    generalLayout->addWidget(WDFunc::NewLBLAndLE(m_workspace, "Степень усреднения для регулировки", widgetName, true));
    // Вкладка "МИП-02"
    auto mipLayout = createTabLayout(tuneTabs, "МИП-02");
    widgetName = m_settings.nameof<MipIp>();
    mipLayout->addWidget(WDFunc::NewLBLAndLE(m_workspace, "IP адрес устройства", widgetName, true));
    mipLayout->addWidget(WDFunc::newHLine(m_workspace));
    widgetName = m_settings.nameof<MipPort>();
    mipLayout->addWidget(WDFunc::NewLBLAndLE(m_workspace, "Порт устройства", widgetName, true));
    mipLayout->addWidget(WDFunc::newHLine(m_workspace));
    widgetName = m_settings.nameof<MipBsAddress>();
    mipLayout->addWidget(WDFunc::NewLBLAndLE(m_workspace, "Адрес базовой станции", widgetName, true));
}

void SettingsDialog::fill()
{
    WDFunc::SetCBData(this, m_settings.nameof<Timezone>(), m_settings.get<Timezone>());
    WDFunc::SetChBData(this, m_settings.nameof<LoggingEnabled>(), m_settings.get<LoggingEnabled>());
    WDFunc::SetChBData(this, m_settings.nameof<AlarmsEnabled>(), m_settings.get<AlarmsEnabled>());
    WDFunc::SetLEData(this, m_settings.nameof<TuneCount>(), m_settings.get<TuneCount>());
    WDFunc::SetLEData(this, m_settings.nameof<MipIp>(), m_settings.get<MipIp>());
    WDFunc::SetLEData(this, m_settings.nameof<MipPort>(), m_settings.get<MipPort>());
    WDFunc::SetLEData(this, m_settings.nameof<MipBsAddress>(), m_settings.get<MipBsAddress>());
}

void SettingsDialog::acceptSettings()
{
    bool tmpb = false;
    QString timezone = WDFunc::CBData(this, m_settings.nameof<Timezone>());
    if (!timezone.isEmpty())
        m_settings.set<Timezone>(timezone);
    if (WDFunc::ChBData(this, m_settings.nameof<LoggingEnabled>(), tmpb))
        m_settings.set<LoggingEnabled>(tmpb);
    if (WDFunc::ChBData(this, m_settings.nameof<AlarmsEnabled>(), tmpb))
        m_settings.set<AlarmsEnabled>(tmpb);
    auto tuneCount = WDFunc::LEData(this, m_settings.nameof<TuneCount>()).toInt(&tmpb);
    if (tmpb)
    {
        if ((tuneCount < 0) || (tuneCount > 100))
        {
            tuneCount = m_settings.defaultValue(SettingType::TuneCount).toInt();
            qWarning() << QString("Неверное число степени усреднения, "
                                  "установлено значение по умолчанию %1")
                              .arg(tuneCount);
        }
        m_settings.set<TuneCount>(tuneCount);
    }
    m_settings.set<MipIp>(WDFunc::LEData(this, m_settings.nameof<MipIp>()));
    m_settings.set<MipPort>(WDFunc::LEData(this, m_settings.nameof<MipPort>()));
    m_settings.set<MipBsAddress>(WDFunc::LEData(this, m_settings.nameof<MipBsAddress>()));
    close();
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
