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
#include <comawidgets/gasdensitywidget.h>
#include <comawidgets/splashscreen.h>
#include <device/current_device.h>
#include <dialogs/aboutdialog.h>
#include <dialogs/connectdialog.h>
#include <dialogs/errordialog.h>
#include <dialogs/keypressdialog.h>
#include <dialogs/reconnectdialog.h>
#include <dialogs/settingsdialog.h>
#include <engines/slices.h>
#include <gen/errorqueue.h>
#include <gen/files.h>
#include <gen/logger.h>
#include <gen/stdfunc.h>
#include <gen/timefunc.h>
#include <helpers/appconfig.h>
#include <helpers/comaversion.h>
#include <helpers/constants.h>
#include <helpers/hex2binfileconverter.h>
#include <interfaces/types/settingstypes.h>
#include <journals/journalviewer.h>
#include <oscillograms/dialogs/switchjournaldialog.h>
#include <oscillograms/dialogs/trendviewdialog.h>
#include <oscillograms/swjmanager.h>
#include <oscillograms/swjpackconvertor.h>
#include <s2/s2configstorage.h>
#include <s2/s2datafactory.h>
#include <s2/s2util.h>
#include <widgets/emessagebox.h>
#include <widgets/epopup.h>
#include <widgets/filefunc.h>
#include <widgets/graphfunc.h>
#include <widgets/lblfunc.h>
#include <widgets/styleloader.h>
#include <widgets/waitwidget.h>
#include <widgets/wdfunc.h>
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
    , editor(nullptr)
{
    connect(m_connectionManager, &ConnectionManager::reconnectUI, this, &Coma::showReconnectDialog);
    connect(m_connectionManager, &ConnectionManager::connectFailed, this, //
        [this](const QString &errMsg) { EMessageBox::error(this, errMsg); });
    EMessageBox::setHash(UserSettings::get(UserSettings::PasswordHash));
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
    AlarmW = new AlarmWidget(this);
    hlyout->addWidget(AlarmW, Qt::AlignCenter);
    lyout->addLayout(hlyout);
    lyout->addWidget(least());
    wdgt->setLayout(lyout);
    setCentralWidget(wdgt);
    setupMenubar();
    StyleLoader::setStyle(UserSettings::get(UserSettings::SettingName::Theme));
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
            connect(dialog, &SettingsDialog::alarmOperationUpdate, AlarmW, &AlarmWidget::updateAlarmOperation);
            connect(dialog, &SettingsDialog::alarmIntervalUpdate, AlarmW, &AlarmWidget::updateAlarmInterval);
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
    menu = new QMenu(menubar);
    menu->setTitle("Снимки");
    menu->addAction("Создать снимок", this, &Coma::createSlice);
    menu->addAction("Восстановить из снимка", this, &Coma::restoreFromSlice);
    menubar->addMenu(menu);
    menubar->addAction("О программе", this, &Coma::showAboutDialog);
    menubar->addSeparator();

    menu = new QMenu(menubar);
    menu->setTitle("Автономная работа");
    menu->addAction("Загрузка осциллограммы", this, qOverload<>(&Coma::loadOsc));
    menu->addAction("Загрузка файла переключений", this, qOverload<>(&Coma::loadSwj));
    menu->addAction("Конвертация файлов переключений", this, &Coma::loadSwjPackConvertor);
    menu->addAction("Конвертация файлов HEX -> BIN", this, &Coma::hex2BinConverter);
    if (AppConfiguration::app() == AppConfiguration::Debug)
        menu->addAction("Редактор XML модулей", this, &Coma::openXmlEditor);
    menu->addAction("Просмотрщик журналов", this, &Coma::openJournalViewer);
    menu->addAction("Просмотреть снимок", this, &Coma::openSlice);
    menubar->addMenu(menu);
    setMenuBar(menubar);
}

void Coma::loadSwj(const QString &filename)
{
    SwjManager swjManager;
    fileVector = oscManager.loadFromFile(filename);
    auto oscVector = swjManager.loadFromFile(filename);
    std::move(oscVector.begin(), oscVector.end(), std::back_inserter(fileVector));
    SwjModel *swjModel = nullptr;
    TrendViewModel *oscModel = nullptr;
    for (auto &item : fileVector)
    {
        std::visit(
            overloaded {
                [&](S2::OscHeader &header) { oscManager.setHeader(header); }, //
                [&](SwjModel &model) { swjModel = &model; },                  //
                [&](TrendViewModel *model) { oscModel = model; }              //
            },
            item);
    }
    if (!swjModel || !oscModel)
        return;

    auto dialog = new SwitchJournalViewDialog(*swjModel, oscModel, oscManager);
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
    auto filepath = FileFunc::ChooseFileForOpen(this, "Switch journal files (*.swj)");
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

void Coma::loadJournal(const QString &filename)
{
    auto jourViewer = new journals::JournalViewer(filename, this);
    jourViewer->exec();
}

void Coma::createSlice() { }

void Coma::restoreFromSlice() { }

void Coma::openSlice() { }

void Coma::openJournalViewer()
{
    auto filepath = FileFunc::ChooseFileForOpen(this, "Journal files (*.jn*)");
    if (!filepath.isEmpty())
        loadJournal(filepath);
}

void Coma::openXmlEditor()
{
    if (editor == nullptr)
        editor = new XmlEditor(this);
    else
        editor->exec();
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
    QDir dir(Settings::logDir());
    qInfo("=== Log started ===\n");
    m_bar = new EStatusBar(this);
    setStatusBar(m_bar);
    setupUI();
    // StdFunc::Wait(10000);
    splash->finish(this);
    splash->deleteLater();
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
    LBLFunc::SetText(this, lblname, QString::number(size), false);
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
        LBLFunc::SetText(this, lblname, QString::number(count) + " из " + QString::number(prb->maximum()));
        // Сброс прогресс-бара после окончания операции чтения/записи
        if (prb->value() >= prb->maximum())
        {
            prb->setValue(0);
            LBLFunc::SetText(this, lblname, " ");
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

void Coma::initConnection(const ConnectStruct &st)
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
    m_currentDevice->async()->connection(this, &Coma::update);
    connectStatusBar();
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
    auto cfgLoader = new Xml::ConfigLoader(m_currentDevice);
    if (!cfgLoader->loadSettings())
    {
        EMessageBox::error(this,
            "Не удалось найти конфигурацию для модуля.\n"
            "Проверьте журнал сообщений.\n"
            "Доступны минимальные функции.");
    }
    cfgLoader->deleteLater();

    AlarmW->configure(m_currentDevice);
    m_dlgManager->setupUI(m_currentDevice, size());
    // Запрашиваем s2 конфигурацию от модуля
    // s2requestService->request(S2::FilesEnum::Config, true);
    m_currentDevice->getFileProvider()->request(S2::FilesEnum::Config, true);
    // нет конфигурации
    if (m_currentDevice->health().isNoConfig())
        qCritical() << Error::Msg::NoConfError;
    // нет коэффициентов
    if (m_currentDevice->health().isNoTuneCoef())
        qCritical() << Error::Msg::NoTuneError;
}

void Coma::disconnectAndClear()
{
    if (m_currentDevice && (m_currentDevice->async()->getConnectionState() != Interface::State::Disconnect))
    {
        AlarmW->clear();
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
