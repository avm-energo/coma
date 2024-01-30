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
    int width = 700, height = 550;
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
    constexpr auto reconnectIntervalTooltip
        = "<p><font size=\"4\">Интервал между попытками переподключиться к устройству при сбое соединения.</font></p>";

    // Вкладка "Общие"
    auto generalLayout = createTabLayout(setupTabs, "Общие");
    auto widgetName = m_settings.nameof<LoggingEnabled>();
    generalLayout->addWidget(WDFunc::NewChB2(m_workspace, widgetName, "Запись обмена данными в файл"));
    generalLayout->addWidget(WDFunc::newHLine(m_workspace));
    widgetName = m_settings.nameof<AlarmsEnabled>();
    generalLayout->addWidget(WDFunc::NewChB2(m_workspace, widgetName, "Постоянное обновление сигнализации"));
    generalLayout->addWidget(WDFunc::newHLine(m_workspace));
    widgetName = m_settings.nameof<SilentInterval>();
    auto widget = WDFunc::NewLBLAndLE(m_workspace, "Время \"тихого\" переподключения, мс", widgetName, true);
    widget->setToolTip("<p><font size=\"4\">Для выполнения некоторых операций (загрузка конфигурации или обновление "
                       "ВПО) требуется перезапуск устройства. Для таких операций предусмотрена возможность \"тихого\""
                       "переподключения, при котором не отображается диалог переподключения к устройству, а само "
                       "переподключение выполняется в фоновом режиме. Если указанного времени не хватит для "
                       "переподключения, диалог о переподключении к устройству появится в любом случае.</font></p>");
    generalLayout->addWidget(widget);
    generalLayout->addWidget(WDFunc::newHLine(m_workspace));
    widgetName = m_settings.nameof<TimeoutCount>();
    widget = WDFunc::NewLBLAndLE(m_workspace, "Кол-во таймаутов для переподключения", widgetName, true);
    widget->setToolTip("<p><font size=\"4\">Количество подряд полученных таймаутов на посылаемые устройству "
                       "запросы, после которых будет предпринята попытка переподключиться к устройству по "
                       "тем же настройкам и интерфейсу для восстановления связи.</font></p>");
    generalLayout->addWidget(widget);
    generalLayout->addWidget(WDFunc::newHLine(m_workspace));
    widgetName = m_settings.nameof<ErrorCount>();
    widget = WDFunc::NewLBLAndLE(m_workspace, "Кол-во ошибок для переподключения", widgetName, true);
    widget->setToolTip("<p><font size=\"4\">Количество подряд полученных ошибок от интерфейса связи с "
                       "устройством, после которых будет предпринята попытка переподключения к устройству.</font></p>");
    generalLayout->addWidget(widget);

    // Вкладка "USB HID"
    auto protocomLayout = createTabLayout(setupTabs, "USB HID");
    widgetName = m_settings.nameof<ProtocomTimeout>();
    protocomLayout->addWidget(WDFunc::NewLBLAndLE(m_workspace, "Таймаут отправки запроса, мс", widgetName, true));
    protocomLayout->addWidget(WDFunc::newHLine(m_workspace));
    widgetName = m_settings.nameof<ProtocomReconnect>();
    widget = WDFunc::NewLBLAndLE(m_workspace, "Интервал переподключения, мс", widgetName, true);
    widget->setToolTip(reconnectIntervalTooltip);
    protocomLayout->addWidget(widget);

    // Вкладка "Modbus"
    auto modbusLayout = createTabLayout(setupTabs, "Modbus");
    widgetName = m_settings.nameof<ModbusTimeout>();
    modbusLayout->addWidget(WDFunc::NewLBLAndLE(m_workspace, "Таймаут отправки запроса, мс", widgetName, true));
    modbusLayout->addWidget(WDFunc::newHLine(m_workspace));
    widgetName = m_settings.nameof<ModbusReconnect>();
    widget = WDFunc::NewLBLAndLE(m_workspace, "Интервал переподключения, мс", widgetName, true);
    widget->setToolTip(reconnectIntervalTooltip);
    modbusLayout->addWidget(widget);

    // Вкладка "МЭК 61850-104"
    auto iec104Layout = createTabLayout(setupTabs, "МЭК 61850-104");
    widgetName = m_settings.nameof<Iec104Timeout>();
    iec104Layout->addWidget(WDFunc::NewLBLAndLE(m_workspace, "Таймаут отправки запроса, мс", widgetName, true));
    iec104Layout->addWidget(WDFunc::newHLine(m_workspace));
    widgetName = m_settings.nameof<Iec104Reconnect>();
    widget = WDFunc::NewLBLAndLE(m_workspace, "Интервал переподключения, мс", widgetName, true);
    widget->setToolTip(reconnectIntervalTooltip);
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(WDFunc::newHLine(m_workspace));
    widget = WDFunc::NewLBLAndLE(m_workspace, "t0, с", m_settings.nameof<Iec104T0>(), true);
    widget->setToolTip("<p><font size=\"4\">Тайм-аут при установке соединения.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(WDFunc::newHLine(m_workspace));
    widget = WDFunc::NewLBLAndLE(m_workspace, "t1, с", m_settings.nameof<Iec104T1>(), true);
    widget->setToolTip("<p><font size=\"4\">Тайм-аут при посылке или тестировании APDU.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(WDFunc::newHLine(m_workspace));
    widget = WDFunc::NewLBLAndLE(m_workspace, "t2, с", m_settings.nameof<Iec104T2>(), true);
    widget->setToolTip("<p><font size=\"4\">Тайм-аут для отправки подтверждения о принятых APDU "
                       "в случае отсутствия сообщения с данными.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(WDFunc::newHLine(m_workspace));
    widget = WDFunc::NewLBLAndLE(m_workspace, "t3, с", m_settings.nameof<Iec104T3>(), true);
    widget->setToolTip("<p><font size=\"4\">Тайм-аут для посылки блоков тестирования "
                       "в случае долгого простоя соединения.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(WDFunc::newHLine(m_workspace));
    widget = WDFunc::NewLBLAndLE(m_workspace, "k", m_settings.nameof<Iec104K>(), true);
    widget->setToolTip("<p><font size=\"4\">Максимальное число неподтверждённых "
                       "APDU, отправляемых станцией.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(WDFunc::newHLine(m_workspace));
    widget = WDFunc::NewLBLAndLE(m_workspace, "w", m_settings.nameof<Iec104W>(), true);
    widget->setToolTip("<p><font size=\"4\">Количество APDU информационного формата (I-посылок), "
                       "после приёма которых требуется отправить подтверждение.</font></p>");
    iec104Layout->addWidget(widget);
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
    WDFunc::SetLEData(this, m_settings.nameof<SilentInterval>(), m_settings.get<SilentInterval>());
    WDFunc::SetLEData(this, m_settings.nameof<TimeoutCount>(), m_settings.get<TimeoutCount>());
    WDFunc::SetLEData(this, m_settings.nameof<ErrorCount>(), m_settings.get<ErrorCount>());
    WDFunc::SetLEData(this, m_settings.nameof<TuneCount>(), m_settings.get<TuneCount>());
    WDFunc::SetLEData(this, m_settings.nameof<MipIp>(), m_settings.get<MipIp>());
    WDFunc::SetLEData(this, m_settings.nameof<MipPort>(), m_settings.get<MipPort>());
    WDFunc::SetLEData(this, m_settings.nameof<MipBsAddress>(), m_settings.get<MipBsAddress>());
    WDFunc::SetLEData(this, m_settings.nameof<ProtocomTimeout>(), m_settings.get<ProtocomTimeout>());
    WDFunc::SetLEData(this, m_settings.nameof<ProtocomReconnect>(), m_settings.get<ProtocomReconnect>());
    WDFunc::SetLEData(this, m_settings.nameof<ModbusTimeout>(), m_settings.get<ModbusTimeout>());
    WDFunc::SetLEData(this, m_settings.nameof<ModbusReconnect>(), m_settings.get<ModbusReconnect>());
    WDFunc::SetLEData(this, m_settings.nameof<Iec104Timeout>(), m_settings.get<Iec104Timeout>());
    WDFunc::SetLEData(this, m_settings.nameof<Iec104Reconnect>(), m_settings.get<Iec104Reconnect>());
    WDFunc::SetLEData(this, m_settings.nameof<Iec104T0>(), m_settings.get<Iec104T0>());
    WDFunc::SetLEData(this, m_settings.nameof<Iec104T1>(), m_settings.get<Iec104T1>());
    WDFunc::SetLEData(this, m_settings.nameof<Iec104T2>(), m_settings.get<Iec104T2>());
    WDFunc::SetLEData(this, m_settings.nameof<Iec104T3>(), m_settings.get<Iec104T3>());
    WDFunc::SetLEData(this, m_settings.nameof<Iec104K>(), m_settings.get<Iec104K>());
    WDFunc::SetLEData(this, m_settings.nameof<Iec104W>(), m_settings.get<Iec104W>());
}

void SettingsDialog::acceptSettings()
{
    bool tmpb = false;
    QString timezone = WDFunc::CBData(this, m_settings.nameof<Timezone>());
    if (!timezone.isEmpty())
        m_settings.set<Timezone>(timezone);

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

    if (WDFunc::ChBData(this, m_settings.nameof<LoggingEnabled>(), tmpb))
        m_settings.set<LoggingEnabled>(tmpb);
    if (WDFunc::ChBData(this, m_settings.nameof<AlarmsEnabled>(), tmpb))
        m_settings.set<AlarmsEnabled>(tmpb);
    m_settings.set<SilentInterval>(WDFunc::LEData(this, m_settings.nameof<SilentInterval>()));
    m_settings.set<TimeoutCount>(WDFunc::LEData(this, m_settings.nameof<TimeoutCount>()));
    m_settings.set<ErrorCount>(WDFunc::LEData(this, m_settings.nameof<ErrorCount>()));

    m_settings.set<ProtocomTimeout>(WDFunc::LEData(this, m_settings.nameof<ProtocomTimeout>()));
    m_settings.set<ProtocomReconnect>(WDFunc::LEData(this, m_settings.nameof<ProtocomReconnect>()));
    m_settings.set<ModbusTimeout>(WDFunc::LEData(this, m_settings.nameof<ModbusTimeout>()));
    m_settings.set<ModbusReconnect>(WDFunc::LEData(this, m_settings.nameof<ModbusReconnect>()));
    m_settings.set<Iec104Timeout>(WDFunc::LEData(this, m_settings.nameof<Iec104Timeout>()));
    m_settings.set<Iec104Reconnect>(WDFunc::LEData(this, m_settings.nameof<Iec104Reconnect>()));
    m_settings.set<Iec104T0>(WDFunc::LEData(this, m_settings.nameof<Iec104T0>()));
    m_settings.set<Iec104T1>(WDFunc::LEData(this, m_settings.nameof<Iec104T1>()));
    m_settings.set<Iec104T2>(WDFunc::LEData(this, m_settings.nameof<Iec104T2>()));
    m_settings.set<Iec104T3>(WDFunc::LEData(this, m_settings.nameof<Iec104T3>()));
    m_settings.set<Iec104K>(WDFunc::LEData(this, m_settings.nameof<Iec104K>()));
    m_settings.set<Iec104W>(WDFunc::LEData(this, m_settings.nameof<Iec104W>()));
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
