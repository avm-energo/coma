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

#include "../avtuk/switchjournaldialog.h"
#include "../avtuk/swjmanager.h"
#include "../comaversion/comaversion.h"
#include "../dialogs/connectdialog.h"
#include "../dialogs/errordialog.h"
#include "../dialogs/keypressdialog.h"
#include "../dialogs/settingsdialog.h"
#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../gen/errorqueue.h"
#include "../gen/files.h"
#include "../gen/logger.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../interfaces/protocom.h"
#include "../interfaces/settingstypes.h"
#include "../module/module.h"
#include "../widgets/aboutwidget.h"
#include "../widgets/splashscreen.h"
#include "../widgets/styleloader.h"
#include "../widgets/wd_func.h"
#include "waitwidget.h"

#include <QApplication>
#include <QDir>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QProgressBar>
#include <QToolBar>
#include <QtGlobal>
#include <functional>
#include <memory>
#ifdef Q_OS_WINDOWS
// clang-format off
#include <windows.h>
// Header dbt must be the last header, thanx to microsoft
#include <dbt.h>
// clang-format on
void registerForDeviceNotification(QWidget *ptr)
{
    DEV_BROADCAST_DEVICEINTERFACE devInt;
    ZeroMemory(&devInt, sizeof(devInt));
    // GUID _guid1 = { 0x25dbce51, 0x6c8f, 0x4a72, { 0x8a, 0x6d, 0xb5, 0x4c, 0x2b, 0x4f, 0xc8, 0x35 } };

    GUID _guid = { 0xa5dcbf10, 0x6530, 0x11d2, { 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed } };
    devInt.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);

    devInt.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    // With DEVICE_NOTIFY_ALL_INTERFACE_CLASSES this property ignores
    devInt.dbcc_classguid = _guid;

    HDEVNOTIFY blub;
    // NOTE Проверить со всеми модулями
    blub = RegisterDeviceNotification((HDEVNOTIFY)ptr->winId(), &devInt,
        DEVICE_NOTIFY_ALL_INTERFACE_CLASSES /*DBT_DEVTYP_OEM*/ /*DEVICE_NOTIFY_WINDOW_HANDLE*/);
}
#endif

Coma::Coma(QWidget *parent) : QMainWindow(parent)
{
}

Coma::~Coma()
{
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
    QToolBar *tb = new QToolBar(this);
    tb->setContextMenuPolicy(Qt::PreventContextMenu);
    tb->setIconSize(QSize(40, 40));
    tb->addAction(QIcon(":/icons/tnstart.svg"), "Соединение", this, &Coma::prepareConnectDlg);
    tb->addAction(QIcon(":/icons/tnstop.svg"), "Разрыв соединения", this, &Coma::DisconnectAndClear);
    tb->addSeparator();
    tb->addAction(QIcon(":/icons/tnsettings.svg"), "Настройки", [this]() {
        SettingsDialog *dlg = new SettingsDialog(this);
        dlg->setMinimumSize(this->size() / 4);
        connect(dlg, &SettingsDialog::disableAlarmUpdate, AlarmW, &AlarmWidget::disableAlarm);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->show();
        this->SaveSettings();
    });
    const QIcon jourIcon(":/icons/tnfrosya.svg");

    QAction *jourAct = new QAction(jourIcon, tr("&Журнал..."), this);
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
    tb->addAction(jourAct);
    return tb;
}

void Coma::SetupUI()
{
    QString caption(QCoreApplication::applicationName());
    caption.append(" v. ").append(QCoreApplication::applicationVersion());
    setWindowTitle(caption);
    setMinimumSize(QSize(1050, 700));
    QWidget *wdgt = new QWidget(this);
    QVBoxLayout *lyout = new QVBoxLayout(wdgt);
    QHBoxLayout *hlyout = new QHBoxLayout;

    hlyout->addWidget(createToolBar() /*, Qt::AlignTop | Qt::AlignLeft*/);

    AlarmW = new AlarmWidget(this);

    hlyout->addWidget(AlarmW, Qt::AlignCenter);

    lyout->addLayout(hlyout);
    lyout->addWidget(least());
    wdgt->setLayout(lyout);
    setCentralWidget(wdgt);
    setupMenubar();
}

void Coma::PrepareDialogs()
{
    Q_INIT_RESOURCE(settings);
    if (m_Module->isConfigOutdated())
    {
        m_Module->eraseSettings();
        m_Module->putConfigVersion();
    }

    if (!m_Module->loadSettings())
    {
        QMessageBox::critical(this, "Ошибка",
            "Не удалось найти конфигурацию для модуля.\n"
            "Проверьте журнал сообщений.\n"
            "Доступны минимальные функции.");
    }

    Q_CLEANUP_RESOURCE(settings);
    m_Module->createAlarm(AlarmW);
    m_Module->create(BdaTimer);
}

QWidget *Coma::least()
{
    QWidget *w = new QWidget;

    QVBoxLayout *lyout = new QVBoxLayout;

    QHBoxLayout *inlyout = new QHBoxLayout;
    lyout->addLayout(inlyout);

    MainTW = new QStackedWidget(this);
    auto sizePolizy = MainTW->sizePolicy();
    sizePolizy.setRetainSizeWhenHidden(true);
    MainTW->setSizePolicy(sizePolizy);

    MainLW = new QListWidget(this);
    sizePolizy = MainLW->sizePolicy();
    sizePolizy.setRetainSizeWhenHidden(true);
    MainLW->setSizePolicy(sizePolizy);
    inlyout->addWidget(MainLW);
    inlyout->addWidget(MainTW);
    MainLW->setMinimumWidth(width() / 6);
    MainLW->setMaximumWidth(width() / 5);
    MainTW->hide();
    MainLW->hide();
    connect(MainTW, &QStackedWidget::currentChanged, this, &Coma::mainTWTabChanged);

    QFrame *line = new QFrame;
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
    QMenuBar *menubar = new QMenuBar(this);

    QMenu *menu = new QMenu(menubar);
    menu->setTitle("Главное");

    menu->addAction("Выход", this, &Coma::close);
    menu->addAction(QIcon(":/icons/tnstart.svg"), "Соединение", this, &Coma::prepareConnectDlg);
    menu->addAction(QIcon(":/icons/tnstop.svg"), "Разрыв соединения", this, &Coma::DisconnectAndClear);

    menubar->addMenu(menu);

    menubar->addAction("О программе", this, &Coma::getAbout);

    menubar->addSeparator();
    menu = new QMenu(menubar);
    menu->setTitle("Автономная работа");
    menu->addAction("Загрузка осциллограммы", this, qOverload<>(&Coma::loadOsc));
    menu->addAction("Загрузка журнала переключений", this, qOverload<>(&Coma::loadSwj));
    menubar->addMenu(menu);
    setMenuBar(menubar);
}

void Coma::prepareConnectDlg()
{
    QAction *action = qobject_cast<QAction *>(sender());
    Q_ASSERT(action);
    action->setDisabled(true);
    auto const &board = Board::GetInstance();
    if (board.connectionState() != Board::ConnectionState::Closed)
    {
        action->setEnabled(true);
        return;
    }
    if (!Reconnect)
    {

        QEventLoop loop;
        ConnectDialog *dlg = new ConnectDialog(this);
        connect(dlg, &ConnectDialog::accepted, this, [=](const ConnectStruct st) {
            dlg->close();
            startWork(st);
        });
        dlg->adjustSize();
        dlg->show();
        connect(dlg, &QDialog::destroyed, this, [=] { action->setEnabled(true); });
    }
    else
        action->setEnabled(true);

    // Stage3

    DisconnectAndClear();
}

void Coma::startWork(const ConnectStruct st)
{
    ConnectSettings = st;
    SaveSettings();
    QApplication::setOverrideCursor(Qt::WaitCursor);

    setupConnection();
}

void Coma::loadOsc(QString &filename)
{
    fileVector = oscManager.loadFromFile(filename);
    TrendViewModel *oscModel = nullptr;
    for (auto &item : fileVector)
    {
        std::visit(overloaded {
                       [&](S2DataTypes::OscHeader &header) { oscManager.setHeader(header); },  //
                       [](auto &&arg) { Q_UNUSED(arg) },                                       //
                       [&](std::unique_ptr<TrendViewModel> &model) { oscModel = model.get(); } //
                   },
            item);
    }
    if (oscModel)
    {
        oscManager.loadOsc(oscModel);
    }
}

void Coma::loadSwj(QString &filename)
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
                       [&](S2DataTypes::OscHeader &header) { oscManager.setHeader(header); }, //
                       [&](SwjModel &model) { swjModel = &model; },                           //
                       [&](std::unique_ptr<TrendViewModel> &model) { oscModel = &model; }     //
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

void Coma::newTimers()
{
    BdaTimer = new QTimer(this);
    BdaTimer->setInterval(1000);

    AlrmTimer = new QTimer(this);
    AlrmTimer->setInterval(5000);
    AlrmTimer->start();

    //    ReconnectTimer = new QTimer(this);
    //    ReconnectTimer->setInterval(INTERVAL::RECONNECT);
    //    ReconnectTimer->setSingleShot(true);
    //    connect(ReconnectTimer, &QTimer::timeout, this, &Coma::AttemptToRec);
}

void Coma::setupConnections()
{

    connect(&DataManager::GetInstance(), &DataManager::responseReceived, this, &Coma::update);
}

void Coma::prepare()
{

    auto const &board = Board::GetInstance();
    QMessageBox::information(this, "Связь установлена", "Установлена связь с " + board.moduleName(), QMessageBox::Ok);
    Reconnect = true;

    PrepareDialogs();
    setupConnections();
    // нет конфигурации
    if (board.noConfig())
        qCritical() << Error::Msg::NoConfError;
    // нет коэффициентов
    if (board.noRegPars())
        qCritical() << Error::Msg::NoTuneError;

    QList<UDialog *> dlgs = m_Module->dialogs();
    Q_ASSERT(MainTW->count() == 0);
    for (auto *d : dlgs)
    {
        auto *item = new QListWidgetItem(d->getCaption(), MainLW);
        item->setSizeHint(QSize(0, height() / 20));
        item->setTextAlignment(Qt::AlignCenter);
        MainLW->addItem(item);
        MainTW->addWidget(d);
    }

    connect(MainLW, &QListWidget::currentRowChanged, MainTW, &QStackedWidget::setCurrentIndex);

    MainTW->show();
    MainLW->show();
    qDebug() << MainTW->width() << width();

    AlrmTimer->start();
    qDebug() << NAMEOF(MainTW) << "created";

    BdaTimer->start();
    auto *msgSerialNumber = statusBar()->findChild<QLabel *>("SerialNumber");
    msgSerialNumber->setText(QString::number(board.serialNumber(Board::BaseMezzAdd), 16));
    auto *msgModel = statusBar()->findChild<QLabel *>("Model");
    msgModel->setText(board.moduleName());
}

bool Coma::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result)
#ifdef __linux
    Q_UNUSED(message)
#endif
    if (eventType == "windows_generic_MSG")
    {
#ifdef Q_OS_WINDOWS
        MSG *msg = static_cast<MSG *>(message);
        int msgType = msg->message;
        if (msgType != WM_DEVICECHANGE)
            return false;
        emit sendMessage(message);

        if (BdaTimer->isActive())
            BdaTimer->stop();
        if (AlrmTimer->isActive())
            AlrmTimer->stop();
        if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected)
        {
            BdaTimer->start();
            AlrmTimer->start();
        }
#endif
    }

    return false;
}

void Coma::go()
{
    // Load settings before anything

    SplashScreen *splash = new SplashScreen(QPixmap("images/surgery.png"));
    splash->show();
    // http://stackoverflow.com/questions/2241808/checking-if-a-folder-exists-and-creating-folders-in-qt-c
    QDir dir(StdFunc::GetHomeDir());
    if (!dir.exists())
        dir.mkpath(".");
    StdFunc::Init();
    qInfo("=== Log started ===\n");
#ifdef Q_OS_LINUX
    // linux code goes here
#endif
#ifdef Q_OS_WINDOWS
    // Listen to device events
    registerForDeviceNotification(this);
#else

#endif

    Reconnect = false;

    newTimers();

    splash->finish(this);
    LoadSettings();
    splash->deleteLater();
    setStatusBar(WDFunc::NewSB(this));

    SetupUI();
    show();
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
        clearWidgets();
        MainTW->hide();
    }

    QMessageBox msgBox;
    msgBox.setText("Связь разорвана.\nПопытка переподключения");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.show();
    msgBox.button(QMessageBox::Ok)->animateClick(3000);
    attemptToRec();
}

void Coma::attemptToRec()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    SaveSettings();
    QApplication::restoreOverrideCursor();
    prepareConnectDlg();
}

void Coma::LoadSettings()
{
    QString HomeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/"
        + QCoreApplication::applicationName() + "/";
    auto sets = std::unique_ptr<QSettings>(new QSettings);
    StyleLoader::GetInstance().attach();
    StdFunc::SetHomeDir(sets->value("Homedir", HomeDir).toString());
}

void Coma::SaveSettings()
{
    auto sets = std::unique_ptr<QSettings>(new QSettings);
    sets->setValue("Homedir", StdFunc::GetHomeDir());
}

void Coma::clearWidgets()
{
    while (MainTW->count())
    {
        QWidget *wdgt = MainTW->widget(0);
        MainTW->removeWidget(wdgt);
        wdgt->deleteLater();
    }
    MainLW->clear();
    MainTW->hide();
    MainLW->hide();
}

void Coma::SetProgressBarSize(int prbnum, int size)
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

void Coma::SetProgressBarCount(int prbnum, int count)
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

void Coma::disconnect()
{
    qInfo(__PRETTY_FUNCTION__);
    BdaTimer->stop();
    AlarmW->clear();

    BaseInterface::iface()->stop();

    //     emit StopCommunications();
    // while (ActiveThreads) // wait for all threads to finish
    //    QCoreApplication::processEvents();
    //  }
    // Board::GetInstance().setConnectionState(Board::ConnectionState::Closed);
}

void Coma::setupConnection()
{
    auto const &board = Board::GetInstance();

    connect(BaseInterface::iface(), &BaseInterface::stateChanged, [](const BaseInterface::State state) {
        using State = BaseInterface::State;
        switch (state)
        {
        case State::Run:
            Board::GetInstance().setConnectionState(Board::ConnectionState::Connected);
            break;
        case State::Stop:
            Board::GetInstance().setConnectionState(Board::ConnectionState::Closed);
            break;
        case State::Wait:
            Board::GetInstance().setConnectionState(Board::ConnectionState::AboutToFinish);
            break;
        default:
            break;
        }
    });
    WaitWidget *ww = new WaitWidget;

    auto connectionReady = std::shared_ptr<QMetaObject::Connection>(new QMetaObject::Connection);
    auto connectionTimeout = std::shared_ptr<QMetaObject::Connection>(new QMetaObject::Connection);
    *connectionTimeout = connect(ww, &WaitWidget::destroyed, this, [=] {
        QObject::disconnect(*connectionReady);
        QObject::disconnect(*connectionTimeout);
        if (Board::GetInstance().type() != 0)
            return;

        QMessageBox::critical(this, "Ошибка", "Не удалось соединиться с прибором", QMessageBox::Ok);
        DisconnectAndClear();
        qCritical() << "Cannot connect" << Error::Timeout;
        QApplication::restoreOverrideCursor();
    });
    *connectionReady = connect(&board, &Board::readyRead, this, [=]() {
        QObject::disconnect(*connectionTimeout);
        QObject::disconnect(*connectionReady);
        ww->Stop();

        QApplication::restoreOverrideCursor();
        prepare();
    });

    ww->setObjectName("ww");
    WaitWidget::ww_struct wws = { true, false, WaitWidget::WW_TIME,
        15 }; // isallowedtostop = true, isIncrement = false, format: mm:ss, 30 minutes
    ww->Init(wws);
    ww->SetMessage("Пожалуйста, подождите");
    // QEventLoop loop;
    //  connect(ww, &WaitWidget::finished, &loop, &QEventLoop::quit);
    ww->Start();
    // loop.exec();
    if (!BaseInterface::iface()->start(ConnectSettings))
    {
        QObject::disconnect(*connectionReady);
        QObject::disconnect(*connectionTimeout);
        ww->Stop();

        QMessageBox::critical(this, "Ошибка", "Не удалось установить связь", QMessageBox::Ok);
        QApplication::restoreOverrideCursor();
        qCritical() << "Cannot connect" << Error::GeneralError;

        return;
    }

    DataManager::clearQueue();
    BaseInterface::iface()->reqBSI();
    connect(this, &Coma::sendMessage, BaseInterface::iface(), &BaseInterface::nativeEvent);
}

void Coma::loadOsc()
{
    QString filename = WDFunc::ChooseFileForOpen(this, "Oscillogram files (*.osc)");
    loadOsc(filename);
}

void Coma::loadSwj()
{
    QString filename = WDFunc::ChooseFileForOpen(this, "Switch journal files (*.swj)");
    loadSwj(filename);
}

void Coma::DisconnectAndClear()
{
    qDebug(__PRETTY_FUNCTION__);
    const auto &board = Board::GetInstance();
    if (board.connectionState() == Board::ConnectionState::Closed)
        return;

    AlarmW->clear();
    disconnect();
    if (m_Module)
    {
        m_Module->closeDialogs();
        clearWidgets();
    }
    Board::GetInstance().reset();
    // BUG Segfault
    //    if (Reconnect)
    //        QMessageBox::information(this, "Разрыв связи", "Связь разорвана", QMessageBox::Ok, QMessageBox::Ok);
    //    else
    //        QMessageBox::information(this, "Разрыв связи", "Не удалось установить связь");

    Reconnect = false;
}

void Coma::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
}

void Coma::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        StdFunc::cancel();
    QMainWindow::keyPressEvent(e);
}

void Coma::mainTWTabChanged(int tabindex)
{
    m_Module->parentTWTabChanged(tabindex);
}

void Coma::update(const DataTypes::GeneralResponseStruct &rsp)
{

    if (rsp.type == DataTypes::GeneralResponseTypes::DataCount)
        SetProgressBarCount(1, rsp.data);

    if (rsp.type == DataTypes::GeneralResponseTypes::DataSize)
        //        SetProgressBar1Size(rsp.data);
        SetProgressBarSize(1, rsp.data);
}

void Coma::closeEvent(QCloseEvent *event)
{
    DisconnectAndClear();
    event->accept();
}

void ComaHelper::parserHelper(const char *appName, Coma *coma)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(appName);
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "file with oscillogramm (*.osc) or with switch journal (*.swj)");
    parser.process(QCoreApplication::arguments());
    const QStringList files = parser.positionalArguments();
    if (!files.isEmpty())
    {
        QString Parameter;
        Parameter = files.at(0);
        QString filestail = Parameter.right(3);
        if (filestail == "osc")
        {
            coma->loadOsc(Parameter);
        }
        else if (filestail == "swj")
        {
            coma->loadSwj(Parameter);
        }
        // TODO
        // else if (filestail == "vrf")
        else
        {
            std::cout << "Wrong file" << std::endl;
            std::exit(0);
        }
    }
}
