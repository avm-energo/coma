#include <common/appconfig.h>
#include <common/names.h>
#include <dialogs/settingsdialog.h>
#include <gen/logger.h>
#include <gen/settings.h>
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
    if (AppConfiguration::app() == AppConfiguration::Debug)
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
    auto timezoneCB = CBFunc::New(m_workspace, SettingsKeys::timezone, zonestrList);
    timezoneCB->setCurrentText(Settings::get(SettingsKeys::timezone,
        QTimeZone::systemTimeZone().displayName(QTimeZone::StandardTime, QTimeZone::OffsetName)));
    auto timezoneLayout = new QHBoxLayout;
    timezoneLayout->addWidget(new QLabel("Часовой пояс", m_workspace));
    timezoneLayout->addWidget(timezoneCB);
    workspaceLayout->addLayout(timezoneLayout);
    workspaceLayout->addWidget(GraphFunc::newHLine(m_workspace));

    if (AppConfiguration::app() == AppConfiguration::Debug)
    {
        // Изменение пароля
        auto passwordGB = new QGroupBox("Пароль", m_workspace);
        auto passwordLayout = new QGridLayout;
        passwordLayout->addWidget(LBLFunc::New(passwordGB, "Обновить пароль:"), 1, 1, 1, 1);
        passwordLayout->addWidget(LEFunc::NewPsw(passwordGB, SettingsKeys::psw, QLineEdit::Password), 1, 2, 1, 1);
        auto resetBtn = new QPushButton("Установить пароль по умолчанию", passwordGB);
        connect(resetBtn, &QAbstractButton::clicked, this, &SettingsDialog::resetPassword);
        passwordLayout->addWidget(resetBtn, 2, 1, 1, 2);
        passwordGB->setLayout(passwordLayout);
        workspaceLayout->addWidget(passwordGB);
    }
}

void SettingsDialog::setupConnectionTab() noexcept
{
    auto setupTabs = createTabWidget(createWorkspaceLayout("Настройки соединения"));
    constexpr auto reconnectIntervalTooltip
        = "<p><font size=\"4\">Интервал между попытками переподключиться к устройству при сбое соединения.</font></p>";

    // Вкладка "Общие"
    auto generalLayout = createTabLayout(setupTabs, "Общие");
    generalLayout->addWidget(ChBFunc::New(m_workspace, SettingsKeys::loggingEnabled, "Запись обмена данными в файл"));
    generalLayout->addWidget(
        CBFunc::NewLBL(m_workspace, "Уровень логировки", SettingsKeys::logLevel, Logger::logLevelsList()));
    generalLayout->addWidget(GraphFunc::newHLine(m_workspace));
    generalLayout->addWidget(
        LEFunc::NewLBL(m_workspace, "Интервал запроса сигнализации, мс", SettingsKeys::alarmsInterval, true));
    generalLayout->addWidget(GraphFunc::newHLine(m_workspace));
    generalLayout->addWidget(ChBFunc::New(m_workspace, SettingsKeys::alarmsEnabled, "Обновление сигнализации"));
    generalLayout->addWidget(GraphFunc::newHLine(m_workspace));
    auto widget
        = LEFunc::NewLBL(m_workspace, "Время \"тихого\" переподключения, мс", SettingsKeys::silentInterval, true);
    widget->setToolTip("<p><font size=\"4\">Для выполнения некоторых операций (загрузка конфигурации или обновление "
                       "ВПО) требуется перезапуск устройства. Для таких операций предусмотрена возможность \"тихого\""
                       "переподключения, при котором не отображается диалог переподключения к устройству, а само "
                       "переподключение выполняется в фоновом режиме. Если указанного времени не хватит для "
                       "переподключения, диалог о переподключении к устройству появится в любом случае.</font></p>");
    generalLayout->addWidget(widget);
    generalLayout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "Кол-во таймаутов для переподключения", SettingsKeys::timeoutCount, true);
    widget->setToolTip("<p><font size=\"4\">Количество подряд полученных таймаутов на посылаемые устройству "
                       "запросы, после которых будет предпринята попытка переподключиться к устройству по "
                       "тем же настройкам и интерфейсу для восстановления связи.</font></p>");
    generalLayout->addWidget(widget);
    generalLayout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "Кол-во ошибок для переподключения", SettingsKeys::errorCount, true);
    widget->setToolTip("<p><font size=\"4\">Количество подряд полученных ошибок от интерфейса связи с "
                       "устройством, после которых будет предпринята попытка переподключения к устройству.</font></p>");
    generalLayout->addWidget(widget);

    // Вкладка "USB HID"
    auto protocomLayout = createTabLayout(setupTabs, "USB HID");
    protocomLayout->addWidget(
        LEFunc::NewLBL(m_workspace, "Таймаут отправки запроса, мс", SettingsKeys::USB::protocomTimeout, true));
    protocomLayout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "Интервал переподключения, мс", SettingsKeys::USB::protocomReconnect, true);
    widget->setToolTip(reconnectIntervalTooltip);
    protocomLayout->addWidget(widget);

    // Вкладка "Modbus"
    auto modbusLayout = createTabLayout(setupTabs, "Modbus");
    modbusLayout->addWidget(
        LEFunc::NewLBL(m_workspace, "Таймаут отправки запроса, мс", SettingsKeys::Serial::modbusTimeout, true));
    modbusLayout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "Интервал переподключения, мс", SettingsKeys::Serial::modbusReconnect, true);
    widget->setToolTip(reconnectIntervalTooltip);
    modbusLayout->addWidget(widget);

    // Вкладка "МЭК 61850-104"
    auto iec104Layout = createTabLayout(setupTabs, "МЭК 61850-104");
    iec104Layout->addWidget(
        LEFunc::NewLBL(m_workspace, "Таймаут отправки запроса, мс", SettingsKeys::Iec104::iec104Timeout, true));
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "Интервал переподключения, мс", SettingsKeys::Iec104::iec104Reconnect, true);
    widget->setToolTip(reconnectIntervalTooltip);
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "t0, с", SettingsKeys::Iec104::iec104T0, true);
    widget->setToolTip("<p><font size=\"4\">Тайм-аут при установке соединения.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "t1, с", SettingsKeys::Iec104::iec104T1, true);
    widget->setToolTip("<p><font size=\"4\">Тайм-аут при посылке или тестировании APDU.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "t2, с", SettingsKeys::Iec104::iec104T2, true);
    widget->setToolTip("<p><font size=\"4\">Тайм-аут для отправки подтверждения о принятых APDU "
                       "в случае отсутствия сообщения с данными.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "t3, с", SettingsKeys::Iec104::iec104T3, true);
    widget->setToolTip("<p><font size=\"4\">Тайм-аут для посылки блоков тестирования "
                       "в случае долгого простоя соединения.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "k", SettingsKeys::Iec104::iec104K, true);
    widget->setToolTip("<p><font size=\"4\">Максимальное число неподтверждённых "
                       "APDU, отправляемых станцией.</font></p>");
    iec104Layout->addWidget(widget);
    iec104Layout->addWidget(GraphFunc::newHLine(m_workspace));
    widget = LEFunc::NewLBL(m_workspace, "w", SettingsKeys::Iec104::iec104W, true);
    widget->setToolTip("<p><font size=\"4\">Количество APDU информационного формата (I-посылок), "
                       "после приёма которых требуется отправить подтверждение.</font></p>");
    iec104Layout->addWidget(widget);
}

void SettingsDialog::setupTuneTab() noexcept
{
    auto tuneTabs = createTabWidget(createWorkspaceLayout("Настройки регулировки"));
    // Вкладка "Общие"
    auto generalLayout = createTabLayout(tuneTabs, "Общие");
    generalLayout->addWidget(
        LEFunc::NewLBL(m_workspace, "Степень усреднения для регулировки", SettingsKeys::tuneCount, true));
    // Вкладка "МИП-02"
    auto mipLayout = createTabLayout(tuneTabs, "МИП-02");
    mipLayout->addWidget(LEFunc::NewLBL(m_workspace, "IP адрес устройства", SettingsKeys::Mip::mipIp, true));
    mipLayout->addWidget(GraphFunc::newHLine(m_workspace));
    mipLayout->addWidget(LEFunc::NewLBL(m_workspace, "Порт устройства", SettingsKeys::Mip::mipPort, true));
    mipLayout->addWidget(GraphFunc::newHLine(m_workspace));
    mipLayout->addWidget(LEFunc::NewLBL(m_workspace, "Адрес базовой станции", SettingsKeys::Mip::mipBsAddress, true));
}

void SettingsDialog::fill()
{
    CBFunc::SetData(this, SettingsKeys::timezone,
        Settings::get(SettingsKeys::timezone,
            QTimeZone::systemTimeZone().displayName(QTimeZone::StandardTime, QTimeZone::OffsetName)));
    ChBFunc::SetData(this, SettingsKeys::loggingEnabled, Settings::get(SettingsKeys::loggingEnabled, true));
    CBFunc::SetData(this, SettingsKeys::logLevel, Settings::get(SettingsKeys::logLevel, "Warn"));
    LEFunc::SetData(this, SettingsKeys::alarmsInterval, Settings::get(SettingsKeys::alarmsInterval, 10000));
    ChBFunc::SetData(this, SettingsKeys::alarmsEnabled, Settings::get(SettingsKeys::alarmsEnabled, true));
    LEFunc::SetData(this, SettingsKeys::silentInterval, Settings::get(SettingsKeys::silentInterval, 10000));
    LEFunc::SetData(this, SettingsKeys::timeoutCount, Settings::get(SettingsKeys::timeoutCount, 5));
    LEFunc::SetData(this, SettingsKeys::errorCount, Settings::get(SettingsKeys::errorCount, 5));
    LEFunc::SetData(this, SettingsKeys::tuneCount, Settings::get(SettingsKeys::tuneCount, 20));
    LEFunc::SetData(this, SettingsKeys::Mip::mipIp, Settings::get(SettingsKeys::Mip::mipIp, "172.16.31.178"));
    LEFunc::SetData(this, SettingsKeys::Mip::mipPort, Settings::get(SettingsKeys::Mip::mipPort, 2404));
    LEFunc::SetData(this, SettingsKeys::Mip::mipBsAddress, Settings::get(SettingsKeys::Mip::mipBsAddress, 1));
    LEFunc::SetData(this, SettingsKeys::USB::protocomTimeout, Settings::get(SettingsKeys::USB::protocomTimeout, 5000));
    LEFunc::SetData(
        this, SettingsKeys::USB::protocomReconnect, Settings::get(SettingsKeys::USB::protocomReconnect, 100));
    LEFunc::SetData(
        this, SettingsKeys::Serial::modbusTimeout, Settings::get(SettingsKeys::Serial::modbusTimeout, 3000));
    LEFunc::SetData(
        this, SettingsKeys::Serial::modbusReconnect, Settings::get(SettingsKeys::Serial::modbusReconnect, 1000));
    LEFunc::SetData(
        this, SettingsKeys::Iec104::iec104Timeout, Settings::get(SettingsKeys::Iec104::iec104Timeout, 1000));
    LEFunc::SetData(
        this, SettingsKeys::Iec104::iec104Reconnect, Settings::get(SettingsKeys::Iec104::iec104Reconnect, 1000));
    LEFunc::SetData(this, SettingsKeys::Iec104::iec104T0, Settings::get(SettingsKeys::Iec104::iec104T0, 30));
    LEFunc::SetData(this, SettingsKeys::Iec104::iec104T1, Settings::get(SettingsKeys::Iec104::iec104T1, 15));
    LEFunc::SetData(this, SettingsKeys::Iec104::iec104T2, Settings::get(SettingsKeys::Iec104::iec104T2, 10));
    LEFunc::SetData(this, SettingsKeys::Iec104::iec104T3, Settings::get(SettingsKeys::Iec104::iec104T3, 20));
    LEFunc::SetData(this, SettingsKeys::Iec104::iec104K, Settings::get(SettingsKeys::Iec104::iec104K, 12));
    LEFunc::SetData(this, SettingsKeys::Iec104::iec104W, Settings::get(SettingsKeys::Iec104::iec104W, 8));
}

void SettingsDialog::set(const QString &name, const QVariant &value)
{
    Settings::set(name, value);
    emit settingChanged(name, value);
}

void SettingsDialog::acceptSettings()
{
    bool tmpb = false;
    QString timezone = CBFunc::Data(this, SettingsKeys::timezone);
    if (!timezone.isEmpty())
        set(SettingsKeys::timezone, timezone);
    auto newPassword = LEFunc::Data(this, SettingsKeys::psw);
    if (!newPassword.isEmpty())
        updatePassword(newPassword);

    auto tuneCount = LEFunc::Data(this, SettingsKeys::tuneCount).toInt(&tmpb);
    if (tmpb)
    {
        if ((tuneCount < 0) || (tuneCount > 100))
        {
            tuneCount = 20;
            qWarning() << QString("Неверное число степени усреднения, "
                                  "установлено значение по умолчанию %1")
                              .arg(tuneCount);
        }
        set("tuneCount", tuneCount);
    }
    set(SettingsKeys::Mip::mipIp, LEFunc::Data(this, SettingsKeys::Mip::mipIp));
    set(SettingsKeys::Mip::mipPort, LEFunc::Data(this, SettingsKeys::Mip::mipPort));
    set(SettingsKeys::Mip::mipBsAddress, LEFunc::Data(this, SettingsKeys::Mip::mipBsAddress));

    if (ChBFunc::Data(this, SettingsKeys::loggingEnabled, tmpb))
        set(SettingsKeys::loggingEnabled, tmpb);
    set(SettingsKeys::logLevel, CBFunc::Data(this, SettingsKeys::logLevel));
    auto alarmsInterval = LEFunc::Data(this, SettingsKeys::alarmsInterval).toInt(&tmpb);
    if (tmpb)
    {
        set(SettingsKeys::alarmsInterval, alarmsInterval);
        emit alarmIntervalUpdate(alarmsInterval);
    }
    if (ChBFunc::Data(this, "alarmsEnabled", tmpb))
    {
        set(SettingsKeys::alarmsEnabled, tmpb);
        emit alarmOperationUpdate(tmpb);
    }
    set(SettingsKeys::silentInterval, LEFunc::Data(this, SettingsKeys::silentInterval));
    set(SettingsKeys::timeoutCount, LEFunc::Data(this, SettingsKeys::timeoutCount));
    set(SettingsKeys::errorCount, LEFunc::Data(this, SettingsKeys::errorCount));

    set(SettingsKeys::USB::protocomTimeout, LEFunc::Data(this, SettingsKeys::USB::protocomTimeout));
    set(SettingsKeys::USB::protocomReconnect, LEFunc::Data(this, SettingsKeys::USB::protocomReconnect));
    set(SettingsKeys::Serial::modbusTimeout, LEFunc::Data(this, SettingsKeys::Serial::modbusTimeout));
    set(SettingsKeys::Serial::modbusReconnect, LEFunc::Data(this, SettingsKeys::Serial::modbusReconnect));
    set(SettingsKeys::Iec104::iec104Timeout, LEFunc::Data(this, SettingsKeys::Iec104::iec104Timeout));
    set(SettingsKeys::Iec104::iec104Reconnect, LEFunc::Data(this, SettingsKeys::Iec104::iec104Reconnect));
    set(SettingsKeys::Iec104::iec104T0, LEFunc::Data(this, SettingsKeys::Iec104::iec104T0));
    set(SettingsKeys::Iec104::iec104T1, LEFunc::Data(this, SettingsKeys::Iec104::iec104T1));
    set(SettingsKeys::Iec104::iec104T2, LEFunc::Data(this, SettingsKeys::Iec104::iec104T2));
    set(SettingsKeys::Iec104::iec104T3, LEFunc::Data(this, SettingsKeys::Iec104::iec104T3));
    set(SettingsKeys::Iec104::iec104K, LEFunc::Data(this, SettingsKeys::Iec104::iec104K));
    set(SettingsKeys::Iec104::iec104W, LEFunc::Data(this, SettingsKeys::Iec104::iec104W));
    close();
}

void SettingsDialog::resetPassword()
{
    if (EMessageBox::question(this, "Установить пароль по умолчанию? Это действие невозможно отменить"))
        set(SettingsKeys::passwordHash, "d93fdd6d1fb5afcca939fa650b62541d09dbcb766f41c39352dc75f348fb35dc");
}

void SettingsDialog::updatePassword(const QString &newPassword)
{
    // Просим подтверждение пользователя
    if (!EMessageBox::question(this, "Обновить пароль? Это действие невозможно отменить"))
        return;
    // Просим ввести старый пароль перед изменением
    if (!EMessageBox::password(this,
            Settings::get(
                SettingsKeys::passwordHash, "d93fdd6d1fb5afcca939fa650b62541d09dbcb766f41c39352dc75f348fb35dc")))
    {
        EMessageBox::warning(this, "Пароль введён неверно!");
        return;
    }
    // Сохраняем новый хэш
    QCryptographicHash hasher(QCryptographicHash::Sha3_256);
    hasher.addData(newPassword.toUtf8());
    set(SettingsKeys::passwordHash, QString::fromUtf8(hasher.result().toHex()));
}

void SettingsDialog::themeChanged(const QString &newTheme)
{
    auto answer = QMessageBox::question(this, "Предупреждение", //
        "Тема будет изменена\n Приложение может не отвечать некоторое время");
    if (answer == QMessageBox::Yes)
    {
        StyleLoader::setStyle(newTheme);
        set(SettingsKeys::theme, newTheme);
    }
}
