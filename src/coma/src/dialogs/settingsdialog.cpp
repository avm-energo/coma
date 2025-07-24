#include <dialogs/settingsdialog.h>
#include <settings/user_settings.h>
#include <widgets/cbfunc.h>
#include <widgets/chbfunc.h>
#include <widgets/emessagebox.h>
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

SettingsDialog::~SettingsDialog() noexcept { }

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
    auto timezoneCB = CBFunc::New(m_workspace, "timezone", zonestrList);
    timezoneCB->setCurrentText(UserSettings::get(UserSettings::SettingName::Timezone));
    auto timezoneLayout = new QHBoxLayout;
    timezoneLayout->addWidget(new QLabel("Часовой пояс", m_workspace));
    timezoneLayout->addWidget(timezoneCB);
    workspaceLayout->addLayout(timezoneLayout);
    workspaceLayout->addWidget(GraphFunc::newHLine(m_workspace));

    // Изменение пароля
    auto passwordGB = new QGroupBox("Пароль", m_workspace);
    auto passwordLayout = new QGridLayout;
    passwordLayout->addWidget(LBLFunc::New(passwordGB, "Обновить пароль:"), 1, 1, 1, 1);
    passwordLayout->addWidget(LEFunc::NewPsw(passwordGB, "password", QLineEdit::Password), 1, 2, 1, 1);
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
    generalLayout->addWidget(ChBFunc::New(m_workspace, "loggingEnabled", "Запись обмена данными в файл"));
    generalLayout->addWidget(GraphFunc::newHLine(m_workspace));
    generalLayout->addWidget(LEFunc::NewLBL(m_workspace, "Интервал запроса сигнализации, мс", "alarmsInterval", true));
    generalLayout->addWidget(GraphFunc::newHLine(m_workspace));
    generalLayout->addWidget(ChBFunc::New(m_workspace, "alarmsEnabled", "Обновление сигнализации"));
    generalLayout->addWidget(GraphFunc::newHLine(m_workspace));
    auto widget = LEFunc::NewLBL(m_workspace, "Время \"тихого\" переподключения, мс", "silentInterval", true);
    widget->setToolTip("<p><font size=\"4\">Для выполнения некоторых операций (загрузка конфигурации или обновление "
                       "ВПО) требуется перезапуск устройства. Для таких операций предусмотрена возможность \"тихого\""
                       "переподключения, при котором не отображается диалог переподключения к устройству, а само "
                       "переподключение выполняется в фоновом режиме. Если указанного времени не хватит для "
                       "переподключения, диалог о переподключении к устройству появится в любом случае.</font></p>");
    generalLayout->addWidget(widget);
    generalLayout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "Кол-во таймаутов для переподключения", "timeoutCount", true);
    widget->setToolTip("<p><font size=\"4\">Количество подряд полученных таймаутов на посылаемые устройству "
                       "запросы, после которых будет предпринята попытка переподключиться к устройству по "
                       "тем же настройкам и интерфейсу для восстановления связи.</font></p>");
    generalLayout->addWidget(widget);
    generalLayout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "Кол-во ошибок для переподключения", "errorCount", true);
    widget->setToolTip("<p><font size=\"4\">Количество подряд полученных ошибок от интерфейса связи с "
                       "устройством, после которых будет предпринята попытка переподключения к устройству.</font></p>");
    generalLayout->addWidget(widget);

    // Вкладка "USB HID"
    auto protocomLayout = createTabLayout(setupTabs, "USB HID");
    protocomLayout->addWidget(LEFunc::NewLBL(m_workspace, "Таймаут отправки запроса, мс", "protocomTimeout", true));
    protocomLayout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "Интервал переподключения, мс", "protocomReconnect", true);
    widget->setToolTip(reconnectIntervalTooltip);
    protocomLayout->addWidget(widget);

    // Вкладка "Modbus"
    auto modbusLayout = createTabLayout(setupTabs, "Modbus");
    modbusLayout->addWidget(LEFunc::NewLBL(m_workspace, "Таймаут отправки запроса, мс", "modbusTimeout", true));
    modbusLayout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "Интервал переподключения, мс", "modbusReconnect", true);
    widget->setToolTip(reconnectIntervalTooltip);
    modbusLayout->addWidget(widget);

    // Вкладка "МЭК 61850-104"
    auto iec104Layout = createTabLayout(setupTabs, "МЭК 61850-104");
    iec104Layout->addWidget(LEFunc::NewLBL(m_workspace, "Таймаут отправки запроса, мс", "iec104Timeout", true));
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "Интервал переподключения, мс", "iec104Reconnect", true);
    widget->setToolTip(reconnectIntervalTooltip);
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "t0, с", "iec104T0", true);
    widget->setToolTip("<p><font size=\"4\">Тайм-аут при установке соединения.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "t1, с", "iec104T1", true);
    widget->setToolTip("<p><font size=\"4\">Тайм-аут при посылке или тестировании APDU.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "t2, с", "iec104T2", true);
    widget->setToolTip("<p><font size=\"4\">Тайм-аут для отправки подтверждения о принятых APDU "
                       "в случае отсутствия сообщения с данными.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "t3, с", "iec104T3", true);
    widget->setToolTip("<p><font size=\"4\">Тайм-аут для посылки блоков тестирования "
                       "в случае долгого простоя соединения.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "k", "iec104K", true);
    widget->setToolTip("<p><font size=\"4\">Максимальное число неподтверждённых "
                       "APDU, отправляемых станцией.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "w", "iec104W", true);
    widget->setToolTip("<p><font size=\"4\">Количество APDU информационного формата (I-посылок), "
                       "после приёма которых требуется отправить подтверждение.</font></p>");
    iec104Layout->addWidget(widget);
}

void SettingsDialog::setupTuneTab() noexcept
{
    auto tuneTabs = createTabWidget(createWorkspaceLayout("Настройки регулировки"));
    // Вкладка "Общие"
    auto generalLayout = createTabLayout(tuneTabs, "Общие");
    generalLayout->addWidget(LEFunc::NewLBL(m_workspace, "Степень усреднения для регулировки", "tuneCount", true));
    // Вкладка "МИП-02"
    auto mipLayout = createTabLayout(tuneTabs, "МИП-02");
    mipLayout->addWidget(LEFunc::NewLBL(m_workspace, "IP адрес устройства", "mipIp", true));
    mipLayout->addWidget(GraphFunc::newHLine(m_workspace));
    mipLayout->addWidget(LEFunc::NewLBL(m_workspace, "Порт устройства", "mipPort", true));
    mipLayout->addWidget(GraphFunc::newHLine(m_workspace));
    mipLayout->addWidget(LEFunc::NewLBL(m_workspace, "Адрес базовой станции", "mipBsAddress", true));
}

void SettingsDialog::fill()
{
    CBFunc::SetData(this, "timezone", UserSettings::get(UserSettings::SettingName::Timezone));
    ChBFunc::SetData(this, "loggingEnabled", UserSettings::get(UserSettings::SettingName::LoggingEnabled));
    LEFunc::SetData(this, "alarmsInterval", UserSettings::get(UserSettings::SettingName::AlarmsInterval));
    ChBFunc::SetData(this, "alarmsEnabled", UserSettings::get(UserSettings::SettingName::AlarmsEnabled));
    LEFunc::SetData(this, "silentInterval", UserSettings::get(UserSettings::SettingName::SilentInterval));
    LEFunc::SetData(this, "timeoutCount", UserSettings::get(UserSettings::SettingName::TimeoutCount));
    LEFunc::SetData(this, "errorCount", UserSettings::get(UserSettings::SettingName::ErrorCount));
    LEFunc::SetData(this, "tuneCount", UserSettings::get(UserSettings::SettingName::TuneCount));
    LEFunc::SetData(this, "mipIp", UserSettings::get(UserSettings::SettingName::MipIp));
    LEFunc::SetData(this, "mipPort", UserSettings::get(UserSettings::SettingName::MipPort));
    LEFunc::SetData(this, "mipBsAddress", UserSettings::get(UserSettings::SettingName::MipBsAddress));
    LEFunc::SetData(this, "protocomTimeout", UserSettings::get(UserSettings::SettingName::ProtocomTimeout));
    LEFunc::SetData(this, "protocomReconnect", UserSettings::get(UserSettings::SettingName::ProtocomReconnect));
    LEFunc::SetData(this, "modbusTimeout", UserSettings::get(UserSettings::SettingName::ModbusTimeout));
    LEFunc::SetData(this, "modbusReconnect", UserSettings::get(UserSettings::SettingName::ModbusReconnect));
    LEFunc::SetData(this, "iec104Timeout", UserSettings::get(UserSettings::SettingName::Iec104Timeout));
    LEFunc::SetData(this, "iec104Reconnect", UserSettings::get(UserSettings::SettingName::Iec104Reconnect));
    LEFunc::SetData(this, "iec104T0", UserSettings::get(UserSettings::SettingName::Iec104T0));
    LEFunc::SetData(this, "iec104T1", UserSettings::get(UserSettings::SettingName::Iec104T1));
    LEFunc::SetData(this, "iec104T2", UserSettings::get(UserSettings::SettingName::Iec104T2));
    LEFunc::SetData(this, "iec104T3", UserSettings::get(UserSettings::SettingName::Iec104T3));
    LEFunc::SetData(this, "iec104K", UserSettings::get(UserSettings::SettingName::Iec104K));
    LEFunc::SetData(this, "iec104W", UserSettings::get(UserSettings::SettingName::Iec104W));
}

void SettingsDialog::acceptSettings()
{
    bool tmpb = false;
    QString timezone = CBFunc::Data(this, "timezone");
    if (!timezone.isEmpty())
        UserSettings::set(UserSettings::Timezone, timezone);
    auto newPassword = LEFunc::Data(this, "pswle");
    if (!newPassword.isEmpty())
        updatePassword(newPassword);

    auto tuneCount = LEFunc::Data(this, "tuneCount").toInt(&tmpb);
    if (tmpb)
    {
        if ((tuneCount < 0) || (tuneCount > 100))
        {
            tuneCount = UserSettings::getDefValue(UserSettings::TuneCount);
            qWarning() << QString("Неверное число степени усреднения, "
                                  "установлено значение по умолчанию %1")
                              .arg(tuneCount);
        }
        UserSettings::set(UserSettings::TuneCount, tuneCount);
    }
    UserSettings::set(UserSettings::MipIp, LEFunc::Data(this, "mipIp"));
    UserSettings::set(UserSettings::MipPort, LEFunc::Data(this, "mipPort"));
    UserSettings::set(UserSettings::MipBsAddress, LEFunc::Data(this, "mipBsAddress"));

    if (ChBFunc::Data(this, "loggingEnabled", tmpb))
        UserSettings::set(UserSettings::LoggingEnabled, tmpb);
    auto alarmsInterval = LEFunc::Data(this, "alarmsInterval").toInt(&tmpb);
    if (tmpb)
    {
        UserSettings::set(UserSettings::AlarmsInterval, alarmsInterval);
        emit alarmIntervalUpdate(alarmsInterval);
    }
    if (ChBFunc::Data(this, "alarmsEnabled", tmpb))
    {
        UserSettings::set(UserSettings::AlarmsEnabled, tmpb);
        emit alarmOperationUpdate(tmpb);
    }
    UserSettings::set(UserSettings::SilentInterval, LEFunc::Data(this, "silentInterval"));
    UserSettings::set(UserSettings::TimeoutCount, LEFunc::Data(this, "timeoutCount"));
    UserSettings::set(UserSettings::ErrorCount, LEFunc::Data(this, "errorCount"));

    UserSettings::set(UserSettings::ProtocomTimeout, LEFunc::Data(this, "protocomTimeout"));
    UserSettings::set(UserSettings::ProtocomReconnect, LEFunc::Data(this, "protocomReconnect"));
    UserSettings::set(UserSettings::ModbusTimeout, LEFunc::Data(this, "modbusTimeout"));
    UserSettings::set(UserSettings::ModbusReconnect, LEFunc::Data(this, "modbusReconnect"));
    UserSettings::set(UserSettings::Iec104Timeout, LEFunc::Data(this, "iec104Timeout"));
    UserSettings::set(UserSettings::Iec104Reconnect, LEFunc::Data(this, "iec104Reconnect"));
    UserSettings::set(UserSettings::Iec104T0, LEFunc::Data(this, "iec104T0"));
    UserSettings::set(UserSettings::Iec104T1, LEFunc::Data(this, "iec104T1"));
    UserSettings::set(UserSettings::Iec104T2, LEFunc::Data(this, "iec104T2"));
    UserSettings::set(UserSettings::Iec104T3, LEFunc::Data(this, "iec104T3"));
    UserSettings::set(UserSettings::Iec104K, LEFunc::Data(this, "iec104K"));
    UserSettings::set(UserSettings::Iec104W, LEFunc::Data(this, "iec104W"));
    close();
}

void SettingsDialog::resetPassword()
{
    if (EMessageBox::question(this, "Установить пароль по умолчанию? Это действие невозможно отменить"))
        UserSettings::setDefValue(UserSettings::PasswordHash);
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
    UserSettings::set(UserSettings::PasswordHash, QString::fromUtf8(hasher.result().toHex()));
}

void SettingsDialog::themeChanged(const QString &newTheme)
{
    auto answer = QMessageBox::question(this, "Предупреждение", //
        "Тема будет изменена\n Приложение может не отвечать некоторое время");
    if (answer == QMessageBox::Yes)
    {
        StyleLoader::setStyle(newTheme);
        UserSettings::set(UserSettings::Theme, newTheme);
    }
}
