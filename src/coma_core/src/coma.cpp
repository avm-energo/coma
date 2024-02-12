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

#include "coma_core/coma.h"

#include "../../comaversion/comaversion.h"
#include "../../dialogs/aboutdialog.h"
#include "../../dialogs/connectdialog.h"
#include "../../dialogs/errordialog.h"
#include "../../dialogs/keypressdialog.h"
#include "../../dialogs/reconnectdialog.h"
#include "../../dialogs/settingsdialog.h"
#include "../../dialogs/switchjournaldialog.h"
#include "../../journals/journalviewer.h"
//#include "../../module/board.h"
//#include "../../module/module.h"
//#include "../../module/s2requestservice.h"
#include "../../oscillograms/swjmanager.h"
#include "../../oscillograms/swjpackconvertor.h"
#include "../../widgets/epopup.h"
#include "../../widgets/gasdensitywidget.h"
#include "../../widgets/splashscreen.h"
#include "../../widgets/styleloader.h"
#include "../../widgets/waitwidget.h"
#include "../../widgets/wd_func.h"
#include "../../xml/xmlconfigloader.h"
#include "../../xml/xmleditor/xmleditor.h"

#include <QApplication>
#include <QDir>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QPainter>
#include <QProgressBar>
#include <QToolBar>
#include <QtGlobal>
#include <alarms/alarmwidget.h>
#include <comaresources/manage.h>
#include <device/current_device.h>
#include <functional>
#include <gen/errorqueue.h>
#include <gen/files.h>
#include <gen/logger.h>
#include <gen/stdfunc.h>
#include <gen/timefunc.h>
//#include <interfaces/conn/active_connection.h>
#include <interfaces/types/settingstypes.h>
#include <iostream>
#include <memory>
#include <s2/s2configstorage.h>
#include <s2/s2datafactory.h>
#include <s2/s2util.h>

namespace Core
{

Coma::Coma(const AppConfiguration appCfg, QWidget *parent)
    : QMainWindow(parent)
    , m_appConfig(appCfg)
    //, m_currentConnection(nullptr)
    , m_connectionManager(new ConnectionManager(this))
    , m_currentDevice(nullptr)
    //, s2dataManager(new S2DataManager(this))
    //, s2requestService(new S2RequestService(this))
    , m_dlgManager(new DialogManager(this))
    , editor(nullptr)
{
    // connections
    // connect(s2requestService.get(), &S2RequestService::response, //
    //    s2dataManager.get(), &S2DataManager::parseS2File);
    connect(m_connectionManager, &ConnectionManager::reconnectUI, this, &Coma::showReconnectDialog);
    // connect(m_connectionManager, &ConnectionManager::connectSuccesfull, this, &Coma::prepare);
    connect(m_connectionManager, &ConnectionManager::connectFailed, this, //
        [this](const QString &errMsg) { EMessageBox::error(this, errMsg); });
}

Coma::~Coma()
{
    freeResources();
}

void convertPixmap(size_t size, QAction *jourAct)
{
    const QIcon jourIcon(":/icons/tnfrosya.svg");
    QPixmap pix = jourIcon.pixmap(QSize(40, 40), QIcon::Disabled);
    QPainter painter(&pix);
    painter.drawPixmap(QRect(20, 0, 20, 20), WDFunc::NewCircle(Qt::red, 20));
    QFont font(painter.font());
    font.setPixelSize(14);
    painter.setFont(font);
    painter.setPen(Qt::white);
    if (size > 99)
        painter.drawText(QRect(20, 0, 20, 20), Qt::AlignCenter, "...");
    else
        painter.drawText(QRect(20, 0, 20, 20), Qt::AlignCenter, QString::number(size));
    jourAct->setIcon(pix);
}

QToolBar *Coma::createToolBar()
{
    auto toolbar = new QToolBar(this);
    toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    toolbar->setIconSize(QSize(40, 40));
    toolbar->addAction(QIcon(":/icons/tnstart.svg"), "Соединение", this, &Coma::connectDialog);
    toolbar->addAction(QIcon(":/icons/tnstop.svg"), "Разрыв соединения", this, &Coma::disconnectAndClear);
    toolbar->addSeparator();
    toolbar->addAction(QIcon(":/icons/tnsettings.svg"), "Настройки", [this]() {
        auto dialog = new SettingsDialog(this);
        dialog->setMinimumSize(this->size() / 4);
        connect(dialog, &SettingsDialog::alarmOperationUpdate, AlarmW, &AlarmWidget::updateAlarmOperation);
        connect(dialog, &SettingsDialog::alarmIntervalUpdate, AlarmW, &AlarmWidget::updateAlarmInterval);
        dialog->exec();
        saveSettings();
    });

    const QIcon jourIcon(":/icons/tnfrosya.svg");
    auto jourAct = new QAction(jourIcon, tr("&Журнал..."), this);
    jourAct->setShortcuts(QKeySequence::Open);
    jourAct->setStatusTip(tr("Открыть протокол работы"));

    connect(jourAct, &QAction::triggered, this, [this]() {
        ErrorDialog *dlg = new ErrorDialog(this);
        dlg->setMinimumWidth(this->width() / 2);
        dlg->setMinimumHeight(this->height() / 2);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->show();
    });
    connect(jourAct, &QAction::triggered, [jourAct, jourIcon] { jourAct->setIcon(jourIcon); });
    const auto &queue = ErrorQueue::GetInstance();
    connect(&queue, &ErrorQueue::errCounts, this, std::bind(&convertPixmap, std::placeholders::_1, jourAct),
        Qt::QueuedConnection);
    toolbar->addAction(jourAct);
    return toolbar;
}

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
    inlyout->addWidget(WDFunc::NewLBL2(this, "Обмен"));
    inlyout->addWidget(WDFunc::NewLBL2(this, "", "prb1lbl"));
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
    menu->addAction(QIcon(":/icons/tnstart.svg"), "Соединение", this, &Coma::connectDialog);
    menu->addAction(QIcon(":/icons/tnstop.svg"), "Разрыв соединения", this, &Coma::disconnectAndClear);
    menubar->addMenu(menu);
    menubar->addAction("О программе", this, &Coma::showAboutDialog);
    menubar->addSeparator();

    menu = new QMenu(menubar);
    menu->setTitle("Автономная работа");
    menu->addAction("Загрузка осциллограммы", this, qOverload<>(&Coma::loadOsc));
    menu->addAction("Загрузка файла переключений", this, qOverload<>(&Coma::loadSwj));
    menu->addAction("Конвертация файлов переключений", this, &Coma::loadSwjPackConvertor);
    if (m_appConfig == AppConfiguration::Debug)
        menu->addAction("Редактор XML модулей", this, &Coma::openXmlEditor);
    menu->addAction("Просмотрщик журналов", this, &Coma::openJournalViewer);
    menubar->addMenu(menu);
    setMenuBar(menubar);
}

void Coma::loadOsc(const QString &filename)
{
    fileVector = oscManager.loadFromFile(filename);
    TrendViewModel *oscModel = nullptr;
    for (auto &item : fileVector)
    {
        std::visit(overloaded {
                       [&](S2::OscHeader &header) { oscManager.setHeader(header); },           //
                       [](auto &&arg) { Q_UNUSED(arg) },                                       //
                       [&](std::unique_ptr<TrendViewModel> &model) { oscModel = model.get(); } //
                   },
            item);
    }
    if (oscModel)
        oscManager.loadOsc(oscModel);
}

void Coma::loadSwj(const QString &filename)
{
    SwjManager swjManager;
    fileVector = oscManager.loadFromFile(filename);
    auto oscVector = swjManager.loadFromFile(filename);
    std::move(oscVector.begin(), oscVector.end(), std::back_inserter(fileVector));
    SwjModel *swjModel = nullptr;
    std::unique_ptr<TrendViewModel> *oscModel = nullptr;
    for (auto &item : fileVector)
    {
        std::visit(overloaded {
                       [&](S2::OscHeader &header) { oscManager.setHeader(header); },      //
                       [&](SwjModel &model) { swjModel = &model; },                       //
                       [&](std::unique_ptr<TrendViewModel> &model) { oscModel = &model; } //
                   },
            item);
    }
    if (!swjModel || !oscModel)
        return;

    auto dialog = new SwitchJournalViewDialog(*swjModel, *oscModel, oscManager);
    dialog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
    dialog->setMinimumHeight(WDFunc::getMainWindow()->height());
    dialog->setMinimumWidth(WDFunc::getMainWindow()->width());
    dialog->adjustSize();
}

void Coma::loadOsc()
{
    auto filepath = WDFunc::ChooseFileForOpen(this, "Oscillogram files (*.osc)");
    if (!filepath.isEmpty())
        loadOsc(filepath);
}

void Coma::loadSwj()
{
    auto filepath = WDFunc::ChooseFileForOpen(this, "Switch journal files (*.swj)");
    if (!filepath.isEmpty())
        loadSwj(filepath);
}

void Coma::loadSwjPackConvertor()
{
    auto convertor = new SwjPackConvertor(this);
    convertor->selectDirectory();
}

void Coma::loadJournal(const QString &filename)
{
    auto jourViewer = new journals::JournalViewer(filename, this);
    jourViewer->exec();
}

void Coma::openJournalViewer()
{
    auto filepath = WDFunc::ChooseFileForOpen(this, "Journal files (*.jn*)");
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
    // http://stackoverflow.com/questions/2241808/checking-if-a-folder-exists-and-creating-folders-in-qt-c
    QDir dir(StdFunc::GetHomeDir());
    if (!dir.exists())
        dir.mkpath(".");
    qInfo("=== Log started ===\n");
    loadSettings();
    setStatusBar(WDFunc::NewSB(this));
    setupUI();
    splash->finish(this);
    splash->deleteLater();
    show();
}

void Coma::loadSettings()
{
    auto homeDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/"
        + QCoreApplication::applicationName() + "/";
    QSettings settings;
    StyleLoader::GetInstance().attach();
    StdFunc::SetHomeDir(settings.value("Homedir", homeDirectory).toString());
}

void Coma::saveSettings()
{
    QSettings settings;
    settings.setValue("Homedir", StdFunc::GetHomeDir());
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
    WDFunc::SetLBLText(this, lblname, QString::number(size), false);
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
        WDFunc::SetLBLText(this, lblname, QString::number(count) + " из " + QString::number(prb->maximum()));
        // Сброс прогресс-бара после окончания операции чтения/записи
        if (prb->value() >= prb->maximum())
        {
            prb->setValue(0);
            WDFunc::SetLBLText(this, lblname, " ");
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
    saveSettings();
    auto connection = m_connectionManager->createConnection(st);
    if (connection && (connection->getConnectionState() != Interface::State::Disconnect))
    {
        // initInterfaceConnection();
        // prepare();
        initDevice(connection);
    }
    else
        QApplication::restoreOverrideCursor();
}

void Coma::initDevice(Interface::AsyncConnection *connection)
{
    using namespace Device;
    m_currentDevice = DeviceFabric::create(connection);
    if (m_currentDevice)
    {
        connect(m_currentDevice, &CurrentDevice::initBSIFinished, this, [this](const Error::Msg err) {
            if (err == Error::Msg::NoError)
                initInterfaceConnection();
            else if (err == Error::Msg::Timeout)
                EMessageBox::error(this, "Превышено время ожидания блока BSI. Disconnect...");
            else
            {
                /// TODO: Handle other error types?
            }
            QApplication::restoreOverrideCursor();
        });
        m_currentDevice->initBSI();
    }
}

void Coma::initInterfaceConnection()
{
    // Update global storage for current connection
    // ActiveConnection::update(ActiveConnection::AsyncConnectionPtr { m_currentConnection });
    // m_currentConnection->connection(this, &Coma::update);
    auto currentConnection = m_currentDevice->async();
    currentConnection->connection(this, &Coma::update);
    // m_dlgManager->updateConnection(currentConnection);
    // s2requestService->updateConnection(currentConnection);
    connectSetupBar();
    prepareDialogs();
}

void Coma::connectSetupBar()
{
    static const QMap<Interface::IfaceType, QString> images {
        { IfaceType::USB, ":/icons/usb.svg" },           //
        { IfaceType::RS485, ":/icons/rs485.svg" },       //
        { IfaceType::Ethernet, ":/icons/ethernet.svg" }, //
        { IfaceType::Unknown, ":/icons/stop.svg" }       //
    };
    auto currentConnection = m_currentDevice->async();

    auto msgModel = statusBar()->findChild<QLabel *>("Model");
    auto msgSerialNumber = statusBar()->findChild<QLabel *>("SerialNumber");
    auto msgConnectionState = statusBar()->findChild<QLabel *>("ConnectionState");
    auto msgConnectionType = statusBar()->findChild<QLabel *>("ConnectionType");
    auto msgConnectionImage = statusBar()->findChild<QLabel *>("ConnectionImage");
    auto msgQueueSize = statusBar()->findChild<QLabel *>("QueueSize");

    if (msgModel && msgSerialNumber && msgConnectionState && msgConnectionType && msgConnectionImage)
    {
        // auto board = &Board::GetInstance();
        //        connect(board, qOverload<>(&Board::typeChanged), msgModel, //
        //            [=]() { msgModel->setText(board->moduleName()); });
        int height = this->statusBar()->height() - this->statusBar()->layout()->contentsMargins().bottom();
        msgModel->setText(m_currentDevice->getDeviceName());
        connect(m_currentDevice, &Device::CurrentDevice::typeChanged, this,
            [=](auto) { msgModel->setText(m_currentDevice->getDeviceName()); });

        msgSerialNumber->setText(QString::number(m_currentDevice->getSerialNumber(), 16));
        connect(m_currentDevice, &Device::CurrentDevice::serialChanged, this,
            [=](u32 serialNumber) { msgSerialNumber->setText(QString::number(serialNumber, 16)); });

        connect(
            currentConnection, &Interface::AsyncConnection::stateChanged, this,
            [=](const Interface::State state) {
                msgConnectionState->setText(Interface::stateToString(state));
                msgConnectionState->setForegroundRole(QPalette::Highlight);
                msgConnectionState->setBackgroundRole(QPalette::HighlightedText);
            },
            Qt::QueuedConnection);

        const auto interfaceType = currentConnection->getInterfaceType();
        QString connName = QVariant::fromValue(interfaceType).toString();
        msgConnectionType->setText(connName);
        QPixmap pixmap = QIcon(QString(images.value(interfaceType))).pixmap(QSize(height, height));
        msgConnectionImage->setPixmap(pixmap);

        // Показываем размер очереди только в Наладке
        if (m_appConfig == AppConfiguration::Debug && msgQueueSize)
        {
            connect(currentConnection, &Interface::AsyncConnection::queueSizeChanged, this, //
                [=](const std::size_t size) { msgQueueSize->setText(QString("Queue size: %1").arg(size)); });
        }
    }
}

// void Coma::prepare()
//{
//    prepareDialogs();
//}

void Coma::prepareDialogs()
{
    // StdFunc::Wait(40); // Подождём, пока BSI долетит до Board
    // auto &board = Board::GetInstance();
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

    //    auto &storage = ConfigStorage::GetInstance();
    //    module.reset(new Module(true, Board::GetInstance().baseSerialInfo(), this));
    //    if (module->loadS2Settings(s2dataManager->getStorage()))
    //    {
    //        if (!module->loadSettings(storage, *s2dataManager))
    //        {
    //            EMessageBox::error(this,
    //                "Не удалось найти конфигурацию для модуля.\n"
    //                "Проверьте журнал сообщений.\n"
    //                "Доступны минимальные функции.");
    //        }
    //        // Обновляем описание протокола
    //        else
    //            ActiveConnection::async()->updateProtocol(storage.getProtocolDescription());
    //    }

    AlarmW->configure(m_currentDevice);
    m_dlgManager->setupUI(m_currentDevice, m_appConfig, size());
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
        // ConfigStorage::GetInstance().clear();
        // s2dataManager->clear();
        // Board::GetInstance().reset();
        // ActiveConnection::reset();
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
    auto reconnectDialog = new ReconnectDialog(this);
    connect(m_connectionManager, &ConnectionManager::reconnectSuccess, //
        reconnectDialog, &ReconnectDialog::reconnectSuccess);
    connect(reconnectDialog, &ReconnectDialog::breakConnection, this, &Coma::disconnectAndClear);
    reconnectDialog->open();
}

void Coma::closeEvent(QCloseEvent *event)
{
    disconnectAndClear();
    event->accept();
}

void ComaHelper::initAppSettings(const QString &appName, const QString &orgName, const QString &version)
{
    QCoreApplication::setApplicationName(appName);
    QCoreApplication::setOrganizationName(orgName);
    QCoreApplication::setApplicationVersion(version);
    initResources();
    StdFunc::Init();
    Logger::writeStart(StdFunc::GetSystemHomeDir() + "coma.log");
    qInstallMessageHandler(Logger::messageHandler);
}

void ComaHelper::parserHelper(Coma *coma)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "file with oscillogramm (*.osc) or with switch journal (*.swj)");
    parser.process(QCoreApplication::arguments());
    const QStringList files = parser.positionalArguments();
    if (!files.isEmpty())
    {
        auto &filepath = files.at(0);
        QFileInfo fileInfo(filepath);
        auto ext = fileInfo.suffix();
        if (ext == "osc")
            coma->loadOsc(filepath);
        else if (ext == "swj")
            coma->loadSwj(filepath);
        else if (ext.contains("jn"))
            coma->loadJournal(filepath);
        // TODO
        // else if (filestail == "vrf")
        else
        {
            std::cout << "Wrong file" << std::endl;
            std::exit(0);
        }
    }
}

}
