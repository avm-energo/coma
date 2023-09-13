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

#include "coma.h"

#include "../comaversion/comaversion.h"
#include "../dialogs/aboutdialog.h"
#include "../dialogs/connectdialog.h"
#include "../dialogs/errordialog.h"
#include "../dialogs/keypressdialog.h"
#include "../dialogs/settingsdialog.h"
#include "../dialogs/switchjournaldialog.h"
#include "../interfaces/connectionmanager.h"
#include "../interfaces/iec104.h"
#include "../interfaces/modbus.h"
#include "../interfaces/protocom.h"
#include "../interfaces/settingstypes.h"
#include "../journals/journalviewer.h"
#include "../module/board.h"
#include "../module/module.h"
#include "../module/s2requestservice.h"
#include "../oscillograms/swjmanager.h"
#include "../s2/s2configstorage.h"
#include "../s2/s2datafactory.h"
#include "../s2/s2util.h"
#include "alarmwidget.h"
#include "epopup.h"
#include "gasdensitywidget.h"
#include "splashscreen.h"
#include "styleloader.h"
#include "waitwidget.h"
#include "wd_func.h"

#include <QApplication>
#include <QDir>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QPainter>
#include <QProgressBar>
#include <QToolBar>
#include <QtGlobal>
#include <comaresources/manage.h>
#include <functional>
#include <gen/errorqueue.h>
#include <gen/files.h>
#include <gen/logger.h>
#include <gen/stdfunc.h>
#include <gen/timefunc.h>
#include <iostream>
#include <memory>

QPoint Coma::s_comaCenter = QPoint(0, 0);

Coma::Coma(const AppConfiguration &appCfg, QWidget *parent)
    : QMainWindow(parent)
    , connectionManager(new IfaceConnManager(this))
    , s2dataManager(new S2DataManager(this))
    , s2requestService(new S2RequestService(this))
    , proxyBS(new DataTypesProxy(this))
    , proxyGRS(new DataTypesProxy(this))
    , editor(nullptr)
    , mAppConfig(appCfg)
    , mDlgManager(new DialogManager(ConfigStorage::GetInstance().getModuleSettings(), //
          *s2dataManager, *s2requestService, this))
{
    proxyBS->RegisterType<DataTypes::BitStringStruct>();
    proxyGRS->RegisterType<DataTypes::GeneralResponseStruct>();
    connect(proxyGRS.get(), &DataTypesProxy::DataStorable, this, &Coma::update);
    // connections
    connect(                                                     //
        s2requestService.get(), &S2RequestService::response,     //
        s2dataManager.get(), &S2DataManager::parseS2File         //
    );                                                           //
    connect(                                                     //
        connectionManager.get(), &IfaceConnManager::sendMessage, //
        this, qOverload<void *>(&Coma::nativeEvent)              //
    );                                                           //
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
    if (size > 10)
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
        connect(dialog, &SettingsDialog::disableAlarmUpdate, AlarmW, &AlarmWidget::disableAlarms);
        dialog->exec();
        this->saveSettings();
    });

    const QIcon jourIcon(":/icons/tnfrosya.svg");
    auto jourAct = new QAction(jourIcon, tr("&Журнал..."), this);
    jourAct->setShortcuts(QKeySequence::Open);
    jourAct->setStatusTip(tr("Открыть протокол работы"));

    connect(jourAct, &QAction::triggered, this, [this]() {
        ErrorDialog *dlg = new ErrorDialog(this);
        dlg->setMinimumWidth(this->width() / 2);
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
    caption.append(" v. ").append(QCoreApplication::applicationVersion());
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

void Coma::prepareDialogs()
{
    auto &storage = ConfigStorage::GetInstance();
    module.reset(new Module(true, Board::GetInstance().baseSerialInfo(), this));
    if (module->loadS2Settings(s2dataManager->getStorage()))
    {
        if (!module->loadSettings(storage, *s2dataManager))
        {
            EMessageBox::error(this,
                "Не удалось найти конфигурацию для модуля.\n"
                "Проверьте журнал сообщений.\n"
                "Доступны минимальные функции.");
        }
    }
    AlarmW->configure();
    mDlgManager->setupUI(mAppConfig, size());
    // Запрашиваем s2 конфигурацию от модуля
    s2requestService->request(S2::FilesEnum::Config, true);
    connect(BdaTimer, &QTimer::timeout, mDlgManager.get(), &DialogManager::reqUpdate);
}

QWidget *Coma::least()
{
    auto w = new QWidget;
    auto lyout = new QVBoxLayout;
    auto inlyout = new QHBoxLayout;
    lyout->addLayout(inlyout);
    auto workspace = mDlgManager->getUI();
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
    menu->addAction("Редактор XML модулей", this, &Coma::openXmlEditor);
    menu->addAction("Просмотрщик журналов", this, &Coma::openJournalViewer);
    menubar->addMenu(menu);
    setMenuBar(menubar);
}

QPoint Coma::ComaCenter()
{
    return Coma::s_comaCenter;
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

void Coma::newTimers()
{
    BdaTimer = new QTimer(this);
    BdaTimer->setInterval(1000);
    // AlrmTimer = new QTimer(this);
    // AlrmTimer->setInterval(5000);
    // AlrmTimer->start();
}

void Coma::prepare()
{
    auto const &board = Board::GetInstance();
    EMessageBox::information(this, "Установлена связь с " + board.moduleName());
    Reconnect = true;
    prepareDialogs();

    // нет конфигурации
    if (board.noConfig())
        qCritical() << Error::Msg::NoConfError;
    // нет коэффициентов
    if (board.noRegPars())
        qCritical() << Error::Msg::NoTuneError;

    // AlrmTimer->start();
    BdaTimer->start();
    auto msgSerialNumber = statusBar()->findChild<QLabel *>("SerialNumber");
    msgSerialNumber->setText(QString::number(board.serialNumber(Board::BaseMezzAdd), 16));
    auto msgModel = statusBar()->findChild<QLabel *>("Model");
    msgModel->setText(board.moduleName());
}

void Coma::nativeEvent(void *message)
{
    Q_UNUSED(message);
    if (BdaTimer->isActive())
        BdaTimer->stop();
    // if (AlrmTimer->isActive())
    //    AlrmTimer->stop();
    if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected)
    {
        BdaTimer->start();
        // AlrmTimer->start();
    }
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
    StdFunc::Init();
    qInfo("=== Log started ===\n");

    connectionManager->registerDeviceNotifications(this);
    Reconnect = false;
    newTimers();
    loadSettings();
    setStatusBar(WDFunc::NewSB(this));
    connectSB();
    setupUI();
    splash->finish(this);
    splash->deleteLater();
    show();
}

void Coma::connectSB()
{
    static const QMap<Board::InterfaceType, QString> images {
        { Board::InterfaceType::USB, ":/icons/usb.svg" },           //
        { Board::InterfaceType::RS485, ":/icons/rs485.svg" },       //
        { Board::InterfaceType::Ethernet, ":/icons/ethernet.svg" }, //
        { Board::InterfaceType::Unknown, ":/icons/stop.svg" }       //
    };

    auto msgModel = this->findChild<QLabel *>("Model");
    auto msgConnectionState = this->findChild<QLabel *>("ConnectionState");
    auto msgConnectionType = this->findChild<QLabel *>("ConnectionType");
    auto msgConnectionImage = this->findChild<QLabel *>("ConnectionImage");
    if (msgModel && msgConnectionState && msgConnectionType && msgConnectionImage)
    {
        int height = this->statusBar()->height() - this->statusBar()->layout()->contentsMargins().bottom();
        auto board = &Board::GetInstance();
        QObject::connect(board, qOverload<>(&Board::typeChanged), msgModel, //
            [=]() { msgModel->setText(board->moduleName()); });
        QObject::connect(
            board, &Board::connectionStateChanged, msgConnectionState,
            [=](Board::ConnectionState state) {
                QString connState = QVariant::fromValue(Board::ConnectionState(state)).toString();
                msgConnectionState->setText(connState);
                msgConnectionState->setForegroundRole(QPalette::Highlight);
                msgConnectionState->setBackgroundRole(QPalette::HighlightedText);
            },
            Qt::QueuedConnection);
        QObject::connect(board, &Board::interfaceTypeChanged, msgConnectionType, //
            [=](const Board::InterfaceType &interfaceType) {
                QString connName = QVariant::fromValue(Board::InterfaceType(interfaceType)).toString();
                msgConnectionType->setText(connName);
            });
        QObject::connect(
            board, &Board::interfaceTypeChanged, msgConnectionImage, [=](const Board::InterfaceType &interfaceType) {
                QPixmap pixmap = QIcon(QString(images.value(interfaceType))).pixmap(QSize(height, height));
                msgConnectionImage->setPixmap(pixmap);
            });
    }
}

void Coma::reconnect()
{
    qInfo(__PRETTY_FUNCTION__);
    if (!Reconnect)
        return;

    if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected)
    {
        qDebug() << "call Disconnect";
        disconnect();
        mDlgManager->clearDialogs();
    }
    EMessageBox::infoWithoutButtons(this, "Связь разорвана.\nПопытка переподключения");
    attemptToRec();
}

void Coma::attemptToRec()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    saveSettings();
    QApplication::restoreOverrideCursor();
    connectDialog();
}

void Coma::loadSettings()
{
    auto homeDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/"
        + QCoreApplication::applicationName() + "/";
    auto sets = std::unique_ptr<QSettings>(new QSettings);
    StyleLoader::GetInstance().attach();
    StdFunc::SetHomeDir(sets->value("Homedir", homeDirectory).toString());
}

void Coma::saveSettings()
{
    auto sets = std::unique_ptr<QSettings>(new QSettings);
    sets->setValue("Homedir", StdFunc::GetHomeDir());
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
    }
}

// void Coma::disconnect()
//{
//    qInfo(__PRETTY_FUNCTION__);
//}

void Coma::connectDialog()
{
    auto action = qobject_cast<QAction *>(sender());
    Q_ASSERT(action);
    action->setDisabled(true);
    auto const &board = Board::GetInstance();
    if (board.connectionState() != Board::ConnectionState::Closed)
    {
        action->setEnabled(true);
        return;
    }
    // if (!Reconnect)
    //{
    auto connDialog = new ConnectDialog(this);
    connect(connDialog, &ConnectDialog::accepted, this, //
        [=](const ConnectStruct &st) {
            ConnectSettings = st;
            // connDialog->close();
            startWork();
        });
    connect(connDialog, &QDialog::destroyed, this, [=] { action->setEnabled(true); });
    connDialog->adjustSize();
    connDialog->exec();
    //}
    // else
    //    action->setEnabled(true);
    // Stage3
    // disconnectAndClear();
}

void Coma::startWork()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    saveSettings();
    initInterfaceConnection();
    setupConnection();
}

void Coma::initInterfaceConnection()
{
    auto const &board = Board::GetInstance();
    connect(proxyBS.get(), &DataTypesProxy::DataStorable, &board, &Board::update);
    BaseInterface::InterfacePointer device;
    switch (board.interfaceType())
    {
#ifdef ENABLE_EMULATOR
    case Board::InterfaceType::Emulator:
        device = BaseInterface::InterfacePointer(new Emulator());
        break;
#endif
    case Board::InterfaceType::USB:
        device.reset(new Protocom(this));
        break;
    case Board::InterfaceType::Ethernet:
        device.reset(new IEC104(this));
        break;
    case Board::InterfaceType::RS485:
        device.reset(new ModBus(this));
        break;
    default:
        qFatal("Connection type error");
    }
    BaseInterface::setIface(std::move(device));
}

void Coma::setupConnection()
{
    auto const &board = Board::GetInstance();
    connect(BaseInterface::iface(), &BaseInterface::stateChanged, [](const State state) {
        switch (state)
        {
        case State::Run:
            Board::GetInstance().setConnectionState(Board::ConnectionState::Connected);
            break;
        case State::Disconnect:
            Board::GetInstance().setConnectionState(Board::ConnectionState::Closed);
            break;
        case State::Reconnect:
            Board::GetInstance().setConnectionState(Board::ConnectionState::AboutToFinish);
            break;
        default:
            break;
        }
    });

    auto connectionReady = std::shared_ptr<QMetaObject::Connection>(new QMetaObject::Connection);
    auto connectionTimeout = std::shared_ptr<QMetaObject::Connection>(new QMetaObject::Connection);
    *connectionTimeout = connect(BaseInterface::iface(), &BaseInterface::disconnected, this, [=] {
        QObject::disconnect(*connectionReady);
        QObject::disconnect(*connectionTimeout);
        if (Board::GetInstance().type() != 0)
            return;

        EMessageBox::error(this, "Не удалось соединиться с прибором");
        disconnectAndClear();
        qCritical() << "Cannot connect" << Error::Timeout;
        QApplication::restoreOverrideCursor();
    });
    *connectionReady = connect(&board, &Board::readyRead, this, [=]() {
        QObject::disconnect(*connectionTimeout);
        QObject::disconnect(*connectionReady);
        QApplication::restoreOverrideCursor();
        prepare();
    });

    if (!BaseInterface::iface()->start(ConnectSettings))
    {
        QObject::disconnect(*connectionReady);
        QObject::disconnect(*connectionTimeout);
        EMessageBox::error(this, "Не удалось установить связь");
        QApplication::restoreOverrideCursor();
        qCritical() << "Cannot connect" << Error::GeneralError;
        return;
    }

    DataManager::GetInstance().clearQueue();
    BaseInterface::iface()->reqBSI();
    connect(                                                     //
        connectionManager.get(), &IfaceConnManager::sendMessage, //
        BaseInterface::iface(), &BaseInterface::nativeEvent      //
    );
}

void Coma::disconnectAndClear()
{
    qDebug(__PRETTY_FUNCTION__);
    const auto &board = Board::GetInstance();
    if (board.connectionState() != Board::ConnectionState::Closed)
    {
        // disconnect();
        BdaTimer->stop();
        AlarmW->clear();
        mDlgManager->clearDialogs();
        ConfigStorage::GetInstance().clearModuleSettings();
        s2dataManager->clear();
        Board::GetInstance().reset();
        BaseInterface::iface()->close();
        // BUG Segfault
        //    if (Reconnect)
        //        QMessageBox::information(this, "Разрыв связи", "Связь разорвана", QMessageBox::Ok, QMessageBox::Ok);
        //    else
        //        QMessageBox::information(this, "Разрыв связи", "Не удалось установить связь");
        Reconnect = false;
    }
}

void Coma::resizeEvent(QResizeEvent *event)
{
    s_comaCenter = geometry().center();
    QMainWindow::resizeEvent(event);
}

void Coma::moveEvent(QMoveEvent *event)
{
    s_comaCenter = geometry().center();
    QMainWindow::moveEvent(event);
}

void Coma::showEvent(QShowEvent *event)
{
    s_comaCenter = geometry().center();
    QMainWindow::showEvent(event);
}

void Coma::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        StdFunc::Cancel();
    QMainWindow::keyPressEvent(event);
}

void Coma::update(const QVariant &msg)
{
    auto rsp = msg.value<DataTypes::GeneralResponseStruct>();
    if (rsp.type == DataTypes::GeneralResponseTypes::DataCount)
        setProgressBarCount(1, rsp.data);

    if (rsp.type == DataTypes::GeneralResponseTypes::DataSize)
        setProgressBarSize(1, rsp.data);
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
