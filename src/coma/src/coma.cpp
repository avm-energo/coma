/*
 * COMA is the "COMplex for AV-TUK", i.e. the program to make a communication
 * with the modules of automatic controller AV-TUK by AVM-Energo LLC
 *
 * Copyright (C) 2017 <Evel> <forevel@yandex.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <alarms/alarmwidget.h>
#include <avm-widgets/emessagebox.h>
#include <avm-widgets/epopup.h>
#include <avm-widgets/filefunc.h>
#include <avm-widgets/graphfunc.h>
#include <avm-widgets/lblfunc.h>
#include <avm-widgets/styleloader.h>
#include <avm-widgets/waitwidget.h>
#include <avm-widgets/wdfunc.h>
#include <comawidgets/gasdensitywidget.h>
#include <comawidgets/splashscreen.h>
#include <common/appconfig.h>
#include <common/comaversion.h>
#include <common/constants.h>
#include <common/hex2binfileconverter.h>
#include <device/current_device.h>
#include <dialogs/aboutdialog.h>
#include <dialogs/connectdialog.h>
#include <dialogs/errordialog.h>
#include <dialogs/keypressdialog.h>
#include <dialogs/reconnectdialog.h>
#include <dialogs/settingsdialog.h>
#include <dialogs/slicegetdialog.h>
#include <gen/errorqueue.h>
#include <gen/files.h>
#include <gen/logger.h>
#include <gen/settings.h>
#include <gen/stdfunc.h>
#include <gen/timefunc.h>
#include <interfaces/types/serial_settings.h>
#include <journals/journalviewer.h>
#include <oscillograms/dialogs/switchjournaldialog.h>
#include <oscillograms/dialogs/trendviewdialog.h>
#include <oscillograms/swjmanager.h>
#include <oscillograms/swjpackconvertor.h>
#include <s2/s2configstorage.h>
#include <s2/s2datafactory.h>
#include <s2/s2util.h>
#include <xml/xmlconfigloader.h>
#include <xml/xmleditor/xmleditor.h>

#include <QApplication>
#include <QDir>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QPainter>
#include <QProgressBar>
#include <QToolBar>
#include <QtGlobal>
#include <coma.h>

Coma::Coma(QWidget *parent)
    : QMainWindow(parent)
    , m_connectionManager(new ConnectionManager(this))
    , m_currentDevice(nullptr)
    , m_dlgManager(new DialogManager(this))
    , m_reconnectDialog(nullptr)
    , m_xmlEditor(nullptr)
{
    connect(m_connectionManager, &ConnectionManager::reconnectUI, this, &Coma::showReconnectDialog);
    connect(m_connectionManager, &ConnectionManager::connectFailed, this, //
        [this](const QString &errMsg) { EMessageBox::error(this, errMsg); });
    connect(this, &Coma::settingsChanged, m_connectionManager, &ConnectionManager::settingsChanged);
    EMessageBox::setHash(
        Settings::get("passwordHash", "d93fdd6d1fb5afcca939fa650b62541d09dbcb766f41c39352dc75f348fb35dc"));
}

Coma::~Coma() { }

void Coma::setupUI()
{
    QString caption(QCoreApplication::applicationName());
    caption.append(" v").append(QCoreApplication::applicationVersion());
    setWindowTitle(caption);
    setMinimumSize(QSize(1150, 700));
    auto wdgt = new QWidget(this);
    auto lyout = new QVBoxLayout(wdgt);
    auto hlyout = new QHBoxLayout;
    hlyout->addWidget(createToolBar(), Qt::AlignTop | Qt::AlignLeft);
    m_alarmW = new AlarmWidget(this);
    hlyout->addWidget(m_alarmW, Qt::AlignCenter);
    lyout->addLayout(hlyout);
    lyout->addWidget(least());
    wdgt->setLayout(lyout);
    setCentralWidget(wdgt);
    setupMenubar();
    StyleLoader::setStyle(Settings::get("theme", "Light"));
}

QToolBar *Coma::createToolBar()
{
    auto toolbar = new QToolBar(this);
    toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    toolbar->setIconSize(QSize(40, 40));
    toolbar->addAction(QIcon(Constants::Resources[Constants::STARTICON]), "Соединение", this, &Coma::connectDialog);
    toolbar->addAction(
        QIcon(Constants::Resources[Constants::STOPICON]), "Разрыв соединения", this, &Coma::disconnectAndClear);
    toolbar->addSeparator();
    toolbar->addAction(QIcon(Constants::Resources[Constants::SETTINGSICON]), "Настройки",
        [this]()
        {
            auto dialog = new SettingsDialog(this);
            dialog->setMinimumSize(this->size() / 4);
            connect(dialog, &SettingsDialog::alarmOperationUpdate, m_alarmW, &AlarmWidget::updateAlarmOperation);
            connect(dialog, &SettingsDialog::alarmIntervalUpdate, m_alarmW, &AlarmWidget::updateAlarmInterval);
            connect(dialog, &SettingsDialog::settingChanged, this, &Coma::settingsChanged);
            dialog->exec();
        });

    const QIcon jourIcon(Constants::Resources[Constants::FROSYA]);
    auto jourAct = new QAction(jourIcon, tr("&Журнал..."), this);
    jourAct->setShortcuts(QKeySequence::Open);
    jourAct->setStatusTip(tr("Открыть протокол работы"));

    connect(jourAct, &QAction::triggered, this,
        [this]()
        {
            ErrorDialog *dlg = new ErrorDialog(this);
            dlg->setMinimumWidth(this->width() / 2);
            dlg->setMinimumHeight(this->height() / 2);
            dlg->setAttribute(Qt::WA_DeleteOnClose);
            dlg->show();
        });
    connect(jourAct, &QAction::triggered, [jourAct, jourIcon] { jourAct->setIcon(jourIcon); });
    const auto &queue = ErrorQueue::GetInstance();
    connect(&queue, &ErrorQueue::errCounts, this, std::bind(&ComaHelper::convertPixmap, std::placeholders::_1, jourAct),
        Qt::QueuedConnection);
    toolbar->addAction(jourAct);
    return toolbar;
}

QWidget *Coma::least()
{
    auto w = new QWidget;
    auto lyout = new QVBoxLayout;
    auto inlyout = new QHBoxLayout;
    lyout->addLayout(inlyout);
    auto workspace = m_dlgManager->getUI();
    inlyout->addWidget(workspace.first);
    inlyout->addWidget(workspace.second);

    auto line = new QFrame;
    lyout->addWidget(line);
    inlyout = new QHBoxLayout;
    inlyout->addWidget(LBLFunc::New(this, "Обмен"));
    inlyout->addWidget(LBLFunc::New(this, "", "prb1lbl"));
    QProgressBar *prb = new QProgressBar;
    prb->setObjectName("prb1prb");
    prb->setOrientation(Qt::Horizontal);
    prb->setMaximumHeight(height() / 50);
    inlyout->addWidget(prb);
    lyout->addLayout(inlyout);
    w->setLayout(lyout);
    return w;
}

void Coma::setupMenubar()
{
    auto menubar = new QMenuBar(this);
    auto menu = new QMenu(menubar);
    menu->setTitle("Главное");
    menu->addAction("Выход", this, &Coma::close);
    menu->addAction(QIcon(Constants::Resources[Constants::STARTICON]), "Соединение", this, &Coma::connectDialog);
    menu->addAction(
        QIcon(Constants::Resources[Constants::STOPICON]), "Разрыв соединения", this, &Coma::disconnectAndClear);
    menubar->addMenu(menu);
    if (AppConfiguration::app() != AppConfiguration::Service)
    {
        menu = new QMenu(menubar);
        menu->setTitle("Снимки");
        menu->addAction("Создать снимок", this, &Coma::createSlice);
        menu->addAction("Восстановить из снимка", this, &Coma::restoreFromSlice);
    }
    menubar->addMenu(menu);
    menubar->addAction("О программе", this, &Coma::showAboutDialog);
    menubar->addSeparator();

    if (AppConfiguration::app() != AppConfiguration::Service)
    {
        menu = new QMenu(menubar);
        menu->setTitle("Автономная работа");
        menu->addAction("Загрузка осциллограммы", this, qOverload<>(&Coma::loadOsc));
        menu->addAction("Загрузка файла переключений", this, qOverload<>(&Coma::loadSwj));
        menu->addAction("Конвертация файлов переключений", this, &Coma::loadSwjPackConvertor);
        menu->addAction("Конвертация файлов HEX -> BIN", this, &Coma::hex2BinConverter);
        menu->addAction("Редактор XML модулей", this, &Coma::openXmlEditor);
        menu->addAction("Просмотрщик журналов JN", this, &Coma::openJournalViewerJN);
        menu->addAction("Просмотрщик журналов DAT", this, &Coma::openJournalViewerDAT);
        menu->addAction("Просмотреть снимок", this, &Coma::openSlice);
        menubar->addMenu(menu);
    }
    setMenuBar(menubar);
}

void Coma::loadSwj(const QString &filename)
{
    SwjManager swjManager;
    m_fileVector = m_oscManager.loadFromFile(filename);
    auto oscVector = swjManager.loadFromFile(filename);
    std::move(oscVector.begin(), oscVector.end(), std::back_inserter(m_fileVector));
    SwjModel *swjModel = nullptr;
    TrendViewModel *oscModel = nullptr;
    for (auto &item : m_fileVector)
    {
        std::visit(
            overloaded {
                [&](S2::OscHeader &header) { m_oscManager.setHeader(header); }, //
                [&](SwjModel &model) { swjModel = &model; },                    //
                [&](TrendViewModel *model) { oscModel = model; }                //
            },
            item);
    }
    if (!swjModel || !oscModel)
        return;

    auto dialog = new SwitchJournalViewDialog(*swjModel, oscModel, m_oscManager);
    dialog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
    dialog->setMinimumHeight(WDFunc::getMainWindow()->height());
    dialog->setMinimumWidth(WDFunc::getMainWindow()->width());
    dialog->adjustSize();
}

void Coma::loadOsc()
{
    TrendViewDialog *dlg = new TrendViewDialog;
    dlg->loadOsc();
}

void Coma::loadSwj()
{
    auto filepath = FileFunc::chooseFileForOpen(this, "Switch journal files (*.swj)");
    if (!filepath.isEmpty())
        loadSwj(filepath);
}

void Coma::loadSwjPackConvertor()
{
    auto convertor = new SwjPackConvertor(this);
    convertor->selectDirectory();
}

void Coma::hex2BinConverter()
{
    Hex2BinFileConverter *converter = new Hex2BinFileConverter(this);
    converter->start();
}

void Coma::loadJournal(const QString &filename, journals::JournalType type)
{
    auto jourViewer = new journals::JournalViewer(filename, type, this);
    jourViewer->exec();
}

void Coma::createSlice()
{
    if (!m_currentDevice)
    {
        EMessageBox::warning(this, "Нет соединения с устройством");
        return;
    }
    m_dlgManager.get()->disableNotifications(); // prevent messageboxes like "reading success" to appear
    QObject::disconnect(m_prbConnection);
    SliceGetDialog *dlg = new SliceGetDialog(m_currentDevice, this);
    dlg->SetupUI();
    dlg->exec();
    m_dlgManager.get()->enableNotifications(); // enabling messageboxes
    m_prbConnection = m_currentDevice->async()->connection(this, &Coma::update);
}

void Coma::restoreFromSlice() { }

void Coma::openSlice() { }

void Coma::openJournalViewerJN()
{
    auto filepath = FileFunc::chooseFileForOpen(this, "Journal files (*.jn*)");
    if (!filepath.isEmpty())
        loadJournal(filepath, journals::JournalType::Jn);
}

void Coma::openJournalViewerDAT()
{
    auto filepath = FileFunc::chooseFileForOpen(this, "Journal files (*.dat)");
    if (!filepath.isEmpty())
        loadJournal(filepath, journals::JournalType::Dat);
}

void Coma::openXmlEditor()
{
    if (m_xmlEditor == nullptr)
        m_xmlEditor = new XmlEditor(this);
    else
        m_xmlEditor->exec();
}

void Coma::showAboutDialog()
{
    auto aboutDialog = new AboutDialog(this);
    const auto progName(QCoreApplication::applicationName());
    const auto comaVer(QCoreApplication::applicationVersion());
    GitVersion version;
    aboutDialog->appendLine("Config version: " + version.getConfigVersion());
    aboutDialog->prependLine(progName + " version " + QString(comaVer) + "-" + version.getGitHash());
    aboutDialog->setupUI();
    aboutDialog->exec();
}

void Coma::go()
{
    // Load settings before anything
    auto splash = new SplashScreen();
    splash->show();
    m_bar = new EStatusBar(this);
    setStatusBar(m_bar);
    setupUI();
    // StdFunc::Wait(10000);
    splash->finish(this);
    splash->deleteLater();
    MessageHandler::setLogLevel(Logger::s_logLevelsMap[Settings::get("logLevel", "Warn")]);
    show();
}

void Coma::setProgressBarSize(int prbnum, int size)
{
    QString prbname = "prb" + QString::number(prbnum) + "prb";
    QString lblname = "prb" + QString::number(prbnum) + "lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb == nullptr)
    {
        qDebug("Пустой prb");
        return;
    }
    LBLFunc::setText(this, lblname, QString::number(size), false);
    prb->setMinimum(0);
    prb->setMaximum(size);
}

void Coma::setProgressBarCount(int prbnum, int count)
{
    QString prbname = "prb" + QString::number(prbnum) + "prb";
    QString lblname = "prb" + QString::number(prbnum) + "lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb != nullptr)
    {
        prb->setValue(count);
        LBLFunc::setText(this, lblname, QString::number(count) + " из " + QString::number(prb->maximum()));
        // Сброс прогресс-бара после окончания операции чтения/записи
        if (prb->value() >= prb->maximum())
        {
            prb->setValue(0);
            LBLFunc::setText(this, lblname, " ");
        }
    }
}

void Coma::connectDialog()
{
    auto action = qobject_cast<QAction *>(sender());
    Q_ASSERT(action);
    action->setDisabled(true);
    if (m_currentDevice)
    {
        action->setEnabled(true);
        return;
    }
    auto connDialog = new ConnectDialog(this);
    connect(connDialog, &ConnectDialog::accepted, this, &Coma::initConnection);
    connect(connDialog, &QDialog::destroyed, this, [=] { action->setEnabled(true); });
    connDialog->adjustSize();
    connDialog->exec();
}

void Coma::initConnection(const ConnectionSettings &st)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    auto connection = m_connectionManager->createConnection(st);
    if (connection && (connection->getConnectionState() != Interface::State::Disconnect))
        initDevice(connection);
    QApplication::restoreOverrideCursor();
}

void Coma::initDevice(Interface::AsyncConnection *connection)
{
    using namespace Device;
    m_currentDevice = DeviceFabric::create(connection);
    if (m_currentDevice)
    {
        connect(m_currentDevice, &CurrentDevice::initBSIFinished, this,
            [this](const Error::Msg err)
            {
                if (err == Error::Msg::NoError)
                    initInterfaceConnection();
                else if (err == Error::Msg::Timeout)
                {
                    EMessageBox::error(this, "Превышено время ожидания блока BSI. Disconnect...");
                    disconnectAndClear();
                }
                else
                {
                    /// TODO: Handle other error types?
                }
            });
        m_currentDevice->initBSI();
    }
}

void Coma::initInterfaceConnection()
{
    m_prbConnection = m_currentDevice->async()->connection(this, &Coma::update);
    connectStatusBar();
    loadXML();
    prepareDialogs();
}

void Coma::connectStatusBar()
{
    static const QMap<Interface::IfaceType, QString> images {
        { IfaceType::USB, Constants::Resources[Constants::USBICON] },           //
        { IfaceType::RS485, Constants::Resources[Constants::RS485ICON] },       //
        { IfaceType::Ethernet, Constants::Resources[Constants::ETHERNETICON] }, //
        { IfaceType::Unknown, Constants::Resources[Constants::STOPICON] }       //
    };
    auto currentConnection = m_currentDevice->async();

    int height = m_bar->height() - m_bar->layout()->contentsMargins().bottom();
    m_bar->setModelString(m_currentDevice->getDeviceName());
    connect(m_currentDevice, &Device::CurrentDevice::typeChanged, m_bar, &EStatusBar::setModelString);
    m_bar->setSerialNumber(m_currentDevice->getSerialNumber());
    connect(m_currentDevice, &Device::CurrentDevice::serialChanged, m_bar, &EStatusBar::setSerialNumber);

    m_bar->setConnectionState(Interface::stateToString(m_currentDevice->async()->getConnectionState()));
    connect(
        currentConnection, &Interface::AsyncConnection::stateChanged, this, [=](const Interface::State state)
        { m_bar->setConnectionState(Interface::stateToString(state)); }, Qt::QueuedConnection);

    const auto interfaceType = currentConnection->getInterfaceType();
    QString connName = QVariant::fromValue(interfaceType).toString();
    m_bar->setConnectionType(connName);
    QPixmap pixmap = QIcon(QString(images.value(interfaceType))).pixmap(QSize(height, height));
    m_bar->setConnectionPixmap(pixmap);

    // Показываем размер очереди только в Наладке
    if (AppConfiguration::app() == AppConfiguration::Debug)
    {
        connect(currentConnection, &Interface::AsyncConnection::queueSizeChanged, this, //
            [=](const quint64 size) { m_bar->setQueueSize(QString("Queue size: %1").arg(size)); });
    }
}

void Coma::prepareDialogs()
{
    EMessageBox::information(this, "Установлена связь с " + m_currentDevice->getDeviceName());

    m_alarmW->configure(m_currentDevice);
    m_dlgManager->setupUI(m_currentDevice, size());
    // Запрашиваем s2 конфигурацию от модуля
    m_currentDevice->getFileProvider()->request(S2::FilesEnum::Config, true);
    // нет конфигурации
    if (m_currentDevice->health().isNoConfig())
        qCritical() << "В модуле отсутствует конфигурация";
    // нет коэффициентов
    if (m_currentDevice->health().isNoTuneCoef())
        qCritical() << "Модуль требует проведения регулировки";
}

void Coma::disconnectAndClear()
{
    if (m_currentDevice && (m_currentDevice->async()->getConnectionState() != Interface::State::Disconnect))
    {
        m_alarmW->clear();
        m_dlgManager->clearDialogs();
        m_connectionManager->breakConnection();
        m_currentDevice = nullptr;
    }
}

void Coma::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        StdFunc::Cancel();
    QMainWindow::keyPressEvent(event);
}

void Coma::loadXML()
{
    auto cfgLoader = new Xml::ConfigLoader(m_currentDevice);
    if (!cfgLoader->loadSettings())
    {
        EMessageBox::error(this,
            "Не удалось найти конфигурацию для модуля.\n"
            "Проверьте журнал сообщений.\n"
            "Доступны минимальные функции.");
    }
    cfgLoader->deleteLater();
}

void Coma::update(const DataTypes::GeneralResponseStruct &rsp)
{
    if (rsp.type == DataTypes::GeneralResponseTypes::DataCount)
        setProgressBarCount(1, rsp.data);

    if (rsp.type == DataTypes::GeneralResponseTypes::DataSize)
        setProgressBarSize(1, rsp.data);
}

void Coma::showReconnectDialog()
{
    if (m_reconnectDialog == nullptr)
    {
        m_reconnectDialog = new ReconnectDialog(this);
        connect(m_connectionManager, &ConnectionManager::reconnectSuccess,       //
            m_reconnectDialog, &ReconnectDialog::reconnectSuccess);
        connect(m_connectionManager, &ConnectionManager::reconnectSuccess, this, //
            [this] { m_reconnectDialog = nullptr; });
        connect(m_reconnectDialog, &ReconnectDialog::breakConnection, this,
            [this]
            {
                disconnectAndClear();
                m_reconnectDialog = nullptr;
            });
        m_reconnectDialog->open();
    }
}

void Coma::closeEvent(QCloseEvent *event)
{
    disconnectAndClear();
    event->accept();
}
