#include <dialogs/settingsdialog.h>
#include <settings/user_settings.h>
#include <widgets/cbfunc.h>
#include <widgets/chbfunc.h>
#include <widgets/epopup.h>
#include <widgets/graphfunc.h>
#include <widgets/lblfunc.h>
#include <widgets/lefunc.h>
#include <widgets/passwordlineedit.h>
#include <widgets/pbfunc.h>
#include <widgets/styleloader.h>

#include <QCryptographicHash>
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

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_workspace(new QStackedWidget(this))
    , m_sidebar(new QListWidget(this))
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Настройки");
    setupUI();
    fill();
}

SettingsDialog::~SettingsDialog() noexcept
{
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
    auto themeComboBox = CBFunc::New(m_workspace, StyleLoader::availableStyles());
    themeComboBox->setCurrentText(StyleLoader::styleName());
    connect(themeComboBox, &QComboBox::currentTextChanged, this, &SettingsDialog::themeChanged);
    auto themeLayout = new QHBoxLayout;
    themeLayout->addWidget(new QLabel("Тема", m_workspace));
    themeLayout->addWidget(themeComboBox);
    workspaceLayout->addLayout(themeLayout);
    workspaceLayout->addWidget(GraphFunc::newHLine(m_workspace));

    // Изменение таймзоны
    auto zoneList = QTimeZone::availableTimeZoneIds();
    QStringList zonestrList;
    std::copy_if(zoneList.cbegin(), zoneList.cend(), std::back_inserter(zonestrList),
        [](const auto array) { return (array.contains("UTC+")); });
    auto timezoneCB = CBFunc::New(m_workspace, UserSettings::getName(UserSettings::SettingName::Timezone), zonestrList);
    timezoneCB->setCurrentText(UserSettings::get(UserSettings::SettingName::Timezone));
    auto timezoneLayout = new QHBoxLayout;
    timezoneLayout->addWidget(new QLabel("Часовой пояс", m_workspace));
    timezoneLayout->addWidget(timezoneCB);
    workspaceLayout->addLayout(timezoneLayout);
    workspaceLayout->addWidget(GraphFunc::newHLine(m_workspace));

    // Изменение пароля
    auto passwordGB = new QGroupBox("Пароль", m_workspace);
    auto passwordLayout = new QGridLayout;
    auto passwordLE = UserSettings::getName(UserSettings::SettingName::PasswordHash);
    passwordLayout->addWidget(LBLFunc::New(passwordGB, "Обновить пароль:"), 1, 1, 1, 1);
    passwordLayout->addWidget(LEFunc::NewPsw(passwordGB, passwordLE, QLineEdit::Password), 1, 2, 1, 1);
    auto resetBtn = new QPushButton("Установить пароль по умолчанию", passwordGB);
    connect(resetBtn, &QAbstractButton::clicked, this, &SettingsDialog::resetPassword);
    passwordLayout->addWidget(resetBtn, 2, 1, 1, 2);
    passwordGB->setLayout(passwordLayout);
    workspaceLayout->addWidget(passwordGB);
}

void SettingsDialog::setupConnectionTab() noexcept
{
    auto setupTabs = createTabWidget(createWorkspaceLayout("Настройки соединения"));
    constexpr auto reconnectIntervalTooltip
        = "<p><font size=\"4\">Интервал между попытками переподключиться к устройству при сбое соединения.</font></p>";

    // Вкладка "Общие"
    auto generalLayout = createTabLayout(setupTabs, "Общие");
    auto widgetName = UserSettings::getName(UserSettings::SettingName::LoggingEnabled);
    generalLayout->addWidget(ChBFunc::NewChB(m_workspace, widgetName, "Запись обмена данными в файл"));
    generalLayout->addWidget(GraphFunc::newHLine(m_workspace));
    widgetName = UserSettings::getName(UserSettings::SettingName::AlarmsInterval);
    generalLayout->addWidget(LEFunc::NewLBLAndLE(m_workspace, "Интервал запроса сигнализации, мс", widgetName, true));
    generalLayout->addWidget(GraphFunc::newHLine(m_workspace));
    widgetName = UserSettings::getName(UserSettings::SettingName::AlarmsEnabled);
    generalLayout->addWidget(ChBFunc::NewChB(m_workspace, widgetName, "Обновление сигнализации"));
    generalLayout->addWidget(GraphFunc::newHLine(m_workspace));
    widgetName = UserSettings::getName(UserSettings::SettingName::SilentInterval);
    auto widget = LEFunc::NewLBLAndLE(m_workspace, "Время \"тихого\" переподключения, мс", widgetName, true);
    widget->setToolTip("<p><font size=\"4\">Для выполнения некоторых операций (загрузка конфигурации или обновление "
                       "ВПО) требуется перезапуск устройства. Для таких операций предусмотрена возможность \"тихого\""
                       "переподключения, при котором не отображается диалог переподключения к устройству, а само "
                       "переподключение выполняется в фоновом режиме. Если указанного времени не хватит для "
                       "переподключения, диалог о переподключении к устройству появится в любом случае.</font></p>");
    generalLayout->addWidget(widget);
    generalLayout->addWidget(GraphFunc::newHLine(m_workspace));
    widgetName = UserSettings::getName(UserSettings::SettingName::TimeoutCount);
    widget = LEFunc::NewLBLAndLE(m_workspace, "Кол-во таймаутов для переподключения", widgetName, true);
    widget->setToolTip("<p><font size=\"4\">Количество подряд полученных таймаутов на посылаемые устройству "
                       "запросы, после которых будет предпринята попытка переподключиться к устройству по "
                       "тем же настройкам и интерфейсу для восстановления связи.</font></p>");
    generalLayout->addWidget(widget);
    generalLayout->addWidget(GraphFunc::newHLine(m_workspace));
    widgetName = UserSettings::getName(UserSettings::SettingName::ErrorCount);
    widget = LEFunc::NewLBLAndLE(m_workspace, "Кол-во ошибок для переподключения", widgetName, true);
    widget->setToolTip("<p><font size=\"4\">Количество подряд полученных ошибок от интерфейса связи с "
                       "устройством, после которых будет предпринята попытка переподключения к устройству.</font></p>");
    generalLayout->addWidget(widget);

    // Вкладка "USB HID"
    auto protocomLayout = createTabLayout(setupTabs, "USB HID");
    widgetName = UserSettings::getName(UserSettings::SettingName::ProtocomTimeout);
    protocomLayout->addWidget(LEFunc::NewLBLAndLE(m_workspace, "Таймаут отправки запроса, мс", widgetName, true));
    protocomLayout->addWidget(GraphFunc::newHLine(m_workspace));
    widgetName = UserSettings::getName(UserSettings::SettingName::ProtocomReconnect);
    widget = LEFunc::NewLBLAndLE(m_workspace, "Интервал переподключения, мс", widgetName, true);
    widget->setToolTip(reconnectIntervalTooltip);
    protocomLayout->addWidget(widget);

    // Вкладка "Modbus"
    auto modbusLayout = createTabLayout(setupTabs, "Modbus");
    widgetName = UserSettings::getName(UserSettings::SettingName::ModbusTimeout);
    modbusLayout->addWidget(LEFunc::NewLBLAndLE(m_workspace, "Таймаут отправки запроса, мс", widgetName, true));
    modbusLayout->addWidget(GraphFunc::newHLine(m_workspace));
    widgetName = UserSettings::getName(UserSettings::SettingName::ModbusReconnect);
    widget = LEFunc::NewLBLAndLE(m_workspace, "Интервал переподключения, мс", widgetName, true);
    widget->setToolTip(reconnectIntervalTooltip);
    modbusLayout->addWidget(widget);

    // Вкладка "МЭК 61850-104"
    auto iec104Layout = createTabLayout(setupTabs, "МЭК 61850-104");
    widgetName = UserSettings::getName(UserSettings::SettingName::Iec104Timeout);
    iec104Layout->addWidget(LEFunc::NewLBLAndLE(m_workspace, "Таймаут отправки запроса, мс", widgetName, true));
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widgetName = UserSettings::getName(UserSettings::SettingName::Iec104Reconnect);
    widget = LEFunc::NewLBLAndLE(m_workspace, "Интервал переподключения, мс", widgetName, true);
    widget->setToolTip(reconnectIntervalTooltip);
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget
        = LEFunc::NewLBLAndLE(m_workspace, "t0, с", UserSettings::getName(UserSettings::SettingName::Iec104T0), true);
    widget->setToolTip("<p><font size=\"4\">Тайм-аут при установке соединения.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget
        = LEFunc::NewLBLAndLE(m_workspace, "t1, с", UserSettings::getName(UserSettings::SettingName::Iec104T1), true);
    widget->setToolTip("<p><font size=\"4\">Тайм-аут при посылке или тестировании APDU.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget
        = LEFunc::NewLBLAndLE(m_workspace, "t2, с", UserSettings::getName(UserSettings::SettingName::Iec104T2), true);
    widget->setToolTip("<p><font size=\"4\">Тайм-аут для отправки подтверждения о принятых APDU "
                       "в случае отсутствия сообщения с данными.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget
        = LEFunc::NewLBLAndLE(m_workspace, "t3, с", UserSettings::getName(UserSettings::SettingName::Iec104T3), true);
    widget->setToolTip("<p><font size=\"4\">Тайм-аут для посылки блоков тестирования "
                       "в случае долгого простоя соединения.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBLAndLE(m_workspace, "k", UserSettings::getName(UserSettings::SettingName::Iec104K), true);
    widget->setToolTip("<p><font size=\"4\">Максимальное число неподтверждённых "
                       "APDU, отправляемых станцией.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBLAndLE(m_workspace, "w", UserSettings::getName(UserSettings::SettingName::Iec104W), true);
    widget->setToolTip("<p><font size=\"4\">Количество APDU информационного формата (I-посылок), "
                       "после приёма которых требуется отправить подтверждение.</font></p>");
    iec104Layout->addWidget(widget);
}

void SettingsDialog::setupTuneTab() noexcept
{
    auto tuneTabs = createTabWidget(createWorkspaceLayout("Настройки регулировки"));
    // Вкладка "Общие"
    auto generalLayout = createTabLayout(tuneTabs, "Общие");
    auto widgetName = UserSettings::getName(UserSettings::SettingName::TuneCount);
    generalLayout->addWidget(LEFunc::NewLBLAndLE(m_workspace, "Степень усреднения для регулировки", widgetName, true));
    // Вкладка "МИП-02"
    auto mipLayout = createTabLayout(tuneTabs, "МИП-02");
    widgetName = UserSettings::getName(UserSettings::SettingName::MipIp);
    mipLayout->addWidget(LEFunc::NewLBLAndLE(m_workspace, "IP адрес устройства", widgetName, true));
    mipLayout->addWidget(GraphFunc::newHLine(m_workspace));
    widgetName = UserSettings::getName(UserSettings::SettingName::MipPort);
    mipLayout->addWidget(LEFunc::NewLBLAndLE(m_workspace, "Порт устройства", widgetName, true));
    mipLayout->addWidget(GraphFunc::newHLine(m_workspace));
    widgetName = UserSettings::getName(UserSettings::SettingName::MipBsAddress);
    mipLayout->addWidget(LEFunc::NewLBLAndLE(m_workspace, "Адрес базовой станции", widgetName, true));
}

void SettingsDialog::fill()
{
    CBFunc::SetCBData(this, UserSettings::getName(UserSettings::SettingName::Timezone),
        UserSettings::get(UserSettings::SettingName::Timezone));
    ChBFunc::SetChBData(this, UserSettings::getName(UserSettings::SettingName::LoggingEnabled),
        UserSettings::get(UserSettings::SettingName::LoggingEnabled));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::AlarmsInterval),
        UserSettings::get(UserSettings::SettingName::AlarmsInterval));
    ChBFunc::SetChBData(this, UserSettings::getName(UserSettings::SettingName::AlarmsEnabled),
        UserSettings::get(UserSettings::SettingName::AlarmsEnabled));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::SilentInterval),
        UserSettings::get(UserSettings::SettingName::SilentInterval));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::TimeoutCount),
        UserSettings::get(UserSettings::SettingName::TimeoutCount));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::ErrorCount),
        UserSettings::get(UserSettings::SettingName::ErrorCount));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::TuneCount),
        UserSettings::get(UserSettings::SettingName::TuneCount));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::MipIp),
        UserSettings::get(UserSettings::SettingName::MipIp));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::MipPort),
        UserSettings::get(UserSettings::SettingName::MipPort));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::MipBsAddress),
        UserSettings::get(UserSettings::SettingName::MipBsAddress));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::ProtocomTimeout),
        UserSettings::get(UserSettings::SettingName::ProtocomTimeout));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::ProtocomReconnect),
        UserSettings::get(UserSettings::SettingName::ProtocomReconnect));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::ModbusTimeout),
        UserSettings::get(UserSettings::SettingName::ModbusTimeout));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::ModbusReconnect),
        UserSettings::get(UserSettings::SettingName::ModbusReconnect));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::Iec104Timeout),
        UserSettings::get(UserSettings::SettingName::Iec104Timeout));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::Iec104Reconnect),
        UserSettings::get(UserSettings::SettingName::Iec104Reconnect));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::Iec104T0),
        UserSettings::get(UserSettings::SettingName::Iec104T0));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::Iec104T1),
        UserSettings::get(UserSettings::SettingName::Iec104T1));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::Iec104T2),
        UserSettings::get(UserSettings::SettingName::Iec104T2));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::Iec104T3),
        UserSettings::get(UserSettings::SettingName::Iec104T3));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::Iec104K),
        UserSettings::get(UserSettings::SettingName::Iec104K));
    LEFunc::SetLEData(this, UserSettings::getName(UserSettings::SettingName::Iec104W),
        UserSettings::get(UserSettings::SettingName::Iec104W));
}

void SettingsDialog::acceptSettings()
{
    bool tmpb = false;
    QString timezone = CBFunc::CBData(this, UserSettings::getName(UserSettings::SettingName::Timezone));
    if (!timezone.isEmpty())
        m_settings.set<Timezone>(timezone);
    auto newPassword = LEFunc::LEData(this, "pswle");
    if (!newPassword.isEmpty())
        updatePassword(newPassword);

    auto tuneCount = LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::TuneCount)).toInt(&tmpb);
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
    m_settings.set<MipIp>(LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::MipIp)));
    m_settings.set<MipPort>(LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::MipPort)));
    m_settings.set<MipBsAddress>(LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::MipBsAddress)));

    if (ChBFunc::ChBData(this, UserSettings::getName(UserSettings::SettingName::LoggingEnabled), tmpb))
        m_settings.set<LoggingEnabled>(tmpb);
    auto alarmsInterval
        = LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::AlarmsInterval)).toInt(&tmpb);
    if (tmpb)
    {
        m_settings.set<AlarmsInterval>(alarmsInterval);
        emit alarmIntervalUpdate(alarmsInterval);
    }
    if (ChBFunc::ChBData(this, UserSettings::getName(UserSettings::SettingName::AlarmsEnabled), tmpb))
    {
        m_settings.set<AlarmsEnabled>(tmpb);
        emit alarmOperationUpdate(tmpb);
    }
    m_settings.set<SilentInterval>(
        LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::SilentInterval)));
    m_settings.set<TimeoutCount>(LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::TimeoutCount)));
    m_settings.set<ErrorCount>(LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::ErrorCount)));

    m_settings.set<ProtocomTimeout>(
        LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::ProtocomTimeout)));
    m_settings.set<ProtocomReconnect>(
        LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::ProtocomReconnect)));
    m_settings.set<ModbusTimeout>(
        LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::ModbusTimeout)));
    m_settings.set<ModbusReconnect>(
        LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::ModbusReconnect)));
    m_settings.set<Iec104Timeout>(
        LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::Iec104Timeout)));
    m_settings.set<Iec104Reconnect>(
        LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::Iec104Reconnect)));
    m_settings.set<Iec104T0>(LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::Iec104T0)));
    m_settings.set<Iec104T1>(LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::Iec104T1)));
    m_settings.set<Iec104T2>(LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::Iec104T2)));
    m_settings.set<Iec104T3>(LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::Iec104T3)));
    m_settings.set<Iec104K>(LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::Iec104K)));
    m_settings.set<Iec104W>(LEFunc::LEData(this, UserSettings::getName(UserSettings::SettingName::Iec104W)));
    close();
}

void SettingsDialog::resetPassword()
{
    if (EMessageBox::question(this, "Установить пароль по умолчанию? Это действие невозможно отменить"))
        m_settings.set<PasswordHash>(m_settings.defaultValue(PasswordHash));
}

void SettingsDialog::updatePassword(const QString &newPassword)
{
    // Просим подтверждение пользователя
    if (!EMessageBox::question(this, "Обновить пароль? Это действие невозможно отменить"))
        return;
    // Просим ввести старый пароль перед изменением
    if (!EMessageBox::password(this, UserSettings::get(UserSettings::SettingName::PasswordHash)))
    {
        EMessageBox::warning(this, "Пароль введён неверно!");
        return;
    }
    // Сохраняем новый хэш
    QCryptographicHash hasher(QCryptographicHash::Sha3_256);
    hasher.addData(newPassword.toUtf8());
    m_settings.set<PasswordHash>(QString::fromUtf8(hasher.result().toHex()));
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
        Settings::UserSettings::GetInstance().set<Settings::Theme>(newTheme);
    }
}
