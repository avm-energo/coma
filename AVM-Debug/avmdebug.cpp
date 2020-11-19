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

#include "avmdebug.h"

#include "../dialogs/errordialog.h"
#include "../dialogs/keypressdialog.h"
#include "../dialogs/settingsdialog.h"
#include "../gen/errorqueue.h"
#include "../gen/stdfunc.h"
#include "../widgets/splashscreen.h"

#include <QApplication>
#include <QDir>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QSettings>
#include <QSplashScreen>
#include <QStandardPaths>
#include <QToolBar>
#include <QVBoxLayout>
#include <QtGlobal>

void registerForDeviceNotification(AvmDebug *ptr)
{
    DEV_BROADCAST_DEVICEINTERFACE devInt;
    ZeroMemory(&devInt, sizeof(devInt));
    devInt.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    devInt.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    devInt.dbcc_classguid = { 0x25dbce51, 0x6c8f, 0x4a72, { 0x8a, 0x6d, 0xb5, 0x4c, 0x2b, 0x4f, 0xc8, 0x35 } };

    HDEVNOTIFY blub;
    blub = RegisterDeviceNotification((HDEVNOTIFY)ptr->winId(), &devInt, DEVICE_NOTIFY_WINDOW_HANDLE);
}

AvmDebug::AvmDebug(QWidget *parent) : QMainWindow(parent)
{
    SplashScreen *splash = new SplashScreen(QPixmap("images/surgery.png"));
    splash->show();
    splash->showMessage("Подготовка окружения...", Qt::AlignRight, Qt::white);
    // http://stackoverflow.com/questions/2241808/checking-if-a-folder-exists-and-creating-folders-in-qt-c
    QDir dir(StdFunc::GetHomeDir());
    if (!dir.exists())
        dir.mkpath(".");
    StdFunc::Init();
    qInfo("=== Log started ===\n");

#ifdef __linux__
    // linux code goes here
#elif _WIN32
    // Listen to device events
    registerForDeviceNotification(this);
#else

#endif

    S2Config = new S2ConfigType;
    Reconnect = false;
    //    AlarmStateAllDialog = nullptr;
    ActiveThreads = false;
    //    Alarm = new AlarmClass(this);

    NewTimers();
    LoadSettings();

    splash->finish(this);
    splash->deleteLater();
    setStatusBar(WDFunc::NewSB(this));
}

AvmDebug::~AvmDebug()
{
}

void convertPixmap(size_t size, QAction *jourAct)
{
    const QIcon jourIcon("images/skull-and-bones.png");
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

QToolBar *AvmDebug::createToolBar()
{
    QToolBar *tb = new QToolBar(this);
    tb->setContextMenuPolicy(Qt::PreventContextMenu);
    tb->setStyleSheet("QToolBar {background: 0px; margin: 0px; spacing: 5px; padding: 0px;}");
    tb->setIconSize(QSize(40, 40));
    tb->addAction(QIcon("images/play.png"), "Соединение", this, &AvmDebug::StartWork);
    tb->addAction(QIcon("images/stop.png"), "Разрыв соединения", this, &AvmDebug::DisconnectAndClear);
    tb->addSeparator();
    tb->addAction(QIcon("images/settings.svg"), "Настройки", [this]() {
        SettingsDialog *dlg = new SettingsDialog;
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->show();
        this->SaveSettings();
    });
    const QIcon jourIcon("images/skull-and-bones.png");

    QAction *jourAct = new QAction(jourIcon, tr("&Журнал..."), this);
    jourAct->setShortcuts(QKeySequence::Open);
    jourAct->setStatusTip(tr("Открыть протокол работы"));

    connect(jourAct, &QAction::triggered, this, []() {
        ErrorDialog *dlg = new ErrorDialog;
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

void AvmDebug::SetupUI()
{
    setWindowTitle(PROGCAPTION);
    QString tmps = "QMainWindow {background-color: " + QString(Colors::MAINWINCLR) + ";}";
    setStyleSheet(tmps);
    setMinimumSize(QSize(1050, 700));
    QWidget *wdgt = new QWidget(this);
    QVBoxLayout *lyout = new QVBoxLayout(wdgt);
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(createToolBar());

    AlarmW = new AlarmWidget;
    hlyout->addWidget(AlarmW, Qt::AlignCenter);

    lyout->addLayout(hlyout);
    lyout->addWidget(Least());
    wdgt->setLayout(lyout);
    setCentralWidget(wdgt);
    SetupMenubar();
}

QWidget *AvmDebug::Least()
{
    QWidget *w = new QWidget;

    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addStretch(500);

    QHBoxLayout *inlyout = new QHBoxLayout;
    lyout->addLayout(inlyout);

    ETabWidget *MainTW = new ETabWidget;
    MainTW->setObjectName("maintw");
    MainTW->setTabPosition(QTabWidget::West);
    inlyout->addWidget(MainTW, 60);
    MainTW->hide();

    QFrame *line = new QFrame;
    line->setLineWidth(0);
    line->setMidLineWidth(1);
    line->setFrameStyle(QFrame::Sunken | QFrame::HLine);
    lyout->addWidget(line);

    inlyout = new QHBoxLayout;
    inlyout->addWidget(WDFunc::NewLBLT(this, "Обмен"));
    inlyout->addWidget(WDFunc::NewLBLT(this, "", "prb1lbl"));

    QProgressBar *prb = new QProgressBar;
    prb->setObjectName("prb1prb");
    prb->setOrientation(Qt::Horizontal);
    // prb->setMinimumWidth(50);
    prb->setMaximumHeight(height() / 50);
    inlyout->addWidget(prb);
    lyout->addLayout(inlyout);

    inlyout = new QHBoxLayout;
    inlyout->addWidget(WDFunc::NewLBLT(this, "Отсчёт"));
    inlyout->addWidget(WDFunc::NewLBLT(this, "", "prb2lbl"));

    prb = new QProgressBar;
    prb->setObjectName("prb2prb");
    prb->setOrientation(Qt::Horizontal);
    // prb->setMinimumWidth(50);
    prb->setMaximumHeight(height() / 50);
    inlyout->addWidget(prb);
    lyout->addLayout(inlyout);
    w->setLayout(lyout);
    return w;
}

void AvmDebug::SetupMenubar()
{
    QMenuBar *menubar = new QMenuBar(this);
    QString tmps = "QMenuBar {background-color: " + QString(Colors::MAINWINCLRA1)
        + ";}"
          "QMenuBar::item {background-color: "
        + QString(Colors::MAINWINCLRA1) + ";}";
    menubar->setStyleSheet(tmps);
    QMenu *menu = new QMenu(this);
    menu->setTitle("Главное");

    menu->addAction("Выход", this, &AvmDebug::close);
    menu->addAction(QIcon("images/play.png"), "Соединение", this, &AvmDebug::StartWork);
    menu->addAction(QIcon("images/stop.png"), "Разрыв соединения", this, &AvmDebug::DisconnectAndClear);

    menubar->addMenu(menu);

    menubar->addAction("О программе", this, &AvmDebug::GetAbout);

    menubar->addSeparator();

    setMenuBar(menubar);
}

void AvmDebug::StartWork()
{
    NewUSB();
    if (!Reconnect)
    {
        QEventLoop loop;
        Cancelled = false;
        ConnectDialog *dlg = new ConnectDialog;
        connect(dlg, &ConnectDialog::Accepted, [this](ConnectDialog::ConnectStruct *st) {
            this->ConnectSettings = *st;
            emit CloseConnectDialog();
        });
        connect(dlg, &ConnectDialog::Cancelled, [this]() {
            this->Cancelled = true;
            emit CloseConnectDialog();
        });
        connect(this, &AvmDebug::CloseConnectDialog, dlg, &ConnectDialog::close);
        connect(this, &AvmDebug::CloseConnectDialog, &loop, &QEventLoop::quit);
        dlg->show();
        loop.exec();
        if (Cancelled)
        {
            ERMSG("Отмена подключения");
            return;
        }
        S2Config->clear();
        SaveSettings();
    }

    // Stage3

    DisconnectAndClear();
    QString str;
    Board::GetInstance().setTypeB(0);
    Board::GetInstance().setTypeM(0);
    ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
    if (MainTW == nullptr)
    {
        ERMSG("MainTW is empty");
        return;
    }
    connect(MainTW, &ETabWidget::tabClicked, this, &AvmDebug::MainTWTabClicked);
    if (Commands::Connect() != Error::Msg::NoError) // cn->Connect()
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось установить связь", QMessageBox::Ok);
        QApplication::restoreOverrideCursor();
        ERMSG("cn: can't connect");
        return;
    }
    Error::Msg res = ModuleBSI::SetupBSI();
    if (res == Error::Msg::GeneralError)
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось установить связь", QMessageBox::Ok);
        ERMSG("BSI read error");
        return;
    }
    else if (res == Error::Msg::NoError)
    {
        if (ModuleBSI::ModuleTypeString != "")
            QMessageBox::information(
                this, "Успешно", "Связь с " + ModuleBSI::ModuleTypeString + " установлена", QMessageBox::Ok);
        else
        {
            QMessageBox::critical(this, "Ошибка", "Неизвестный тип модуля", QMessageBox::Ok);
            ERMSG("Unknown module type");
            return;
        }
    }
    ActiveUSBThread = true;
    Board::GetInstance().setTypeB(ModuleBSI::GetMType(BoardTypes::BT_BASE));
    Board::GetInstance().setTypeM(ModuleBSI::GetMType(BoardTypes::BT_MEZONIN));
    if (Board::GetInstance().typeM() == 0)
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось соединиться с прибором", QMessageBox::Ok);
        DisconnectAndClear();
        ERMSG("Не получили BSI, нет соединения");
        //            Disconnect();
        return;
    }
    Board::GetInstance().setConnectionState(Board::ConnectionState::Connected);
    Reconnect = true;

    PrepareDialogs();

    if (ModuleBSI::Health() & HTH_CONFIG) // нет конфигурации
        Error::ShowErMsg(Error::Msg::ER_NOCONF);
    if (ModuleBSI::Health() & HTH_REGPARS) // нет коэффициентов
        Error::ShowErMsg(Error::Msg::ER_NOTUNECOEF);

    QList<UDialog *> dlgs = m_Module->dialogs();
    foreach (UDialog *d, dlgs)
        MainTW->addTab(d, d->getCaption());
    MainTW->repaint();
    MainTW->show();

    BdaTimer->start();
    auto *msgSerialNumber = statusBar()->findChild<QLabel *>("SerialNumber");
    msgSerialNumber->setText(QString::number(ModuleBSI::ModuleBsi.SerialNum, 16));

    // check for incomplete calibration
    QString usbserialnum = EProtocom::GetInstance().usbSerial();
    QSettings storedcalibrations(StdFunc::GetSystemHomeDir() + "calibr.ini", QSettings::IniFormat);
    if (storedcalibrations.contains(usbserialnum))
    {
        QMessageBox::warning(this, "Внимание",
            "Для модуля имеется незавершённая регулировка\n"
            "Просьба провести её, начиная с шага "
                + storedcalibrations.value(usbserialnum + "/step", "1").toString());
    }
}

void AvmDebug::setupConnections()
{
    connect(AlarmW, &AlarmWidget::AlarmButtonPressed, AlarmStateAllDialog, &QDialog::show);
    connect(AlarmW, &AlarmWidget::ModuleWarnButtonPressed, m_Module->getWarn(), &QDialog::show);
    connect(AlarmW, &AlarmWidget::ModuleAlarmButtonPressed, m_Module->getAlarm(), &QDialog::show);
    NewTimersBda();
}

void AvmDebug::PrepareDialogs()
{
    m_Module = Module::createModule(BdaTimer);
    Alarm->setModule(m_Module);
    AlarmStateAllDialog = new AlarmStateAll;
    setupConnections();
}

void AvmDebug::CloseDialogs()
{
    m_Module->closeDialogs();
    if (AlarmStateAllDialog != nullptr)
    {
        AlrmTimer->stop();
        AlarmStateAllDialog->close();
        AlarmStateAllDialog = nullptr;
    }
}

void AvmDebug::NewUSB()
{
    connect(this, &AvmDebug::StopCommunications, &EProtocom::GetInstance(), &EProtocom::Disconnect);
    connect(EProtocom::GetInstance().workerThread(), &QThread::finished, [=]() { ActiveUSBThread = false; });
    connect(&EProtocom::GetInstance(), &EProtocom::SetDataSize, this, &AvmDebug::SetProgressBar1Size);
    connect(&EProtocom::GetInstance(), &EProtocom::SetDataCount, this, &AvmDebug::SetProgressBar1);
    connect(&EProtocom::GetInstance(), &EProtocom::ShowError,
        [this](const QString &msg) { QMessageBox::critical(this, "Ошибка", msg, QMessageBox::Ok); });
}

void AvmDebug::NewTimers()
{
    BdaTimer = new QTimer;
    BdaTimer->setInterval(1000);

    AlrmTimer = new QTimer;
    AlrmTimer->setInterval(5000);
    AlrmTimer->start();

    ReceiveTimer = new QTimer;
    ReceiveTimer->setInterval(2000);
    connect(ReceiveTimer, &QTimer::timeout, this, &AvmDebug::FileTimeOut);
}

void AvmDebug::NewTimersBda()
{
    connect(AlrmTimer, &QTimer::timeout, Alarm, &AlarmClass::UpdateAlarmUSB);
    if (AlarmStateAllDialog != nullptr)
        connect(AlrmTimer, &QTimer::timeout, AlarmStateAllDialog, &AlarmStateAll::CallUpdateHealth);
}

bool AvmDebug::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result)
#ifdef __linux
    Q_UNUSED(message)
#endif
    if (eventType == "windows_generic_MSG")
    {
#ifdef _WIN32
        MSG *msg = static_cast<MSG *>(message);
        int msgType = msg->message;
        if (msgType == WM_DEVICECHANGE)
        {
            if (BdaTimer->isActive())
                BdaTimer->stop();
            if (AlrmTimer->isActive())
                AlrmTimer->stop();
            EProtocom::GetInstance().usbStateChanged(message);
            if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected
                && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
            {
                BdaTimer->start();
                AlrmTimer->start();
            }
        }
#endif
    }
    return false;
}

void AvmDebug::SetMode(int mode)
{
    Mode = mode;
}

void AvmDebug::Go(const QString &parameter)
{
    Q_UNUSED(parameter)
    if (Mode != COMA_GENERALMODE)
        StdFunc::SetEmulated(true);
    SetupUI();
    show();
}

void AvmDebug::ReConnect()
{
    if (Reconnect)
    {
        INFOMSG("Reconnect()");
        //        TimeTimer->stop();
        if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected)
        {
            qDebug() << "call Disconnect";
            Disconnect();
            //            emit ClearBsi();
            ClearTW();
            ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
            if (MainTW == nullptr)
            {
                ERMSG("Ошибка открытия файла");
                return;
            }
            MainTW->hide();
            StdFunc::SetEmulated(false);
        }

        QMessageBox msgBox;
        msgBox.setText("Связь разорвана.\nПопытка переподключения");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.show();
        AttemptToRec();
    }
}

void AvmDebug::AttemptToRec()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    SaveSettings();
    QApplication::restoreOverrideCursor();
    StartWork();
}

void AvmDebug::ConnectMessage()
{
    QMessageBox msgBox;

    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText("Связь с " + ModuleBSI::ModuleTypeString + " установлена");
    msgBox.show();
    StdFunc::Wait(INTERVAL::WAIT);
    msgBox.close();
}

void AvmDebug::LoadSettings()
{
    QString HomeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" + PROGNAME + "/";
    QSettings *sets = new QSettings("EvelSoft", PROGNAME);
    StdFunc::SetHomeDir(sets->value("Homedir", HomeDir).toString());
}

void AvmDebug::SaveSettings()
{
    QSettings *sets = new QSettings("EvelSoft", PROGNAME);
    sets->setValue("Homedir", StdFunc::GetHomeDir());
}

void AvmDebug::ClearTW()
{
    ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
    if (MainTW == nullptr)
    {
        ERMSG("Пустой MainTW");
        return;
    }

    while (MainTW->count())
    {
        QWidget *wdgt = MainTW->widget(0);
        MainTW->removeTab(0);
        wdgt->deleteLater();
    }
}

Error::Msg AvmDebug::CheckPassword()
{
    KeyPressDialog dlg; // = new KeyPressDialog;
    return dlg.CheckPassword("se/7520a");
}

void AvmDebug::SetProgressBar1Size(int size)
{
    fileSize = size;
    SetProgressBarSize(1, size);
}

void AvmDebug::SetProgressBar1(int cursize)
{
    curfileSize = cursize;
    ReceiveTimer->stop();
    ReceiveTimer->setInterval(5000);
    SetProgressBar(1, cursize);
    ReceiveTimer->start();
}

void AvmDebug::FileTimeOut()
{
    QString prbname = "prb1prb";
    QString lblname = "prb1lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb == nullptr)
    {
        ERMSG("Пустой prb");
        DBGMSG;
        return;
    }
    WDFunc::SetLBLText(this, lblname, StdFunc::PrbMessage() + QString::number(0), false);

    ReceiveTimer->stop();
}

void AvmDebug::SetProgressBar2Size(int size)
{
    SetProgressBarSize(2, size);
}

void AvmDebug::SetProgressBar2(int cursize)
{
    SetProgressBar(2, cursize);
}

void AvmDebug::SetProgressBarSize(int prbnum, int size)
{
    QString prbname = "prb" + QString::number(prbnum) + "prb";
    QString lblname = "prb" + QString::number(prbnum) + "lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb == nullptr)
    {
        DBGMSG;
        return;
    }
    WDFunc::SetLBLText(this, lblname, StdFunc::PrbMessage() + QString::number(size), false);
    prb->setMinimum(0);
    prb->setMaximum(size);
}

void AvmDebug::SetProgressBar(int prbnum, int cursize)
{
    QString prbname = "prb" + QString::number(prbnum) + "prb";
    QString lblname = "prb" + QString::number(prbnum) + "lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb != nullptr)
    {
        prb->setValue(cursize);
        WDFunc::SetLBLText(
            this, lblname, StdFunc::PrbMessage() + QString::number(cursize) + " из " + QString::number(prb->maximum()));
    }
}

void AvmDebug::GetAbout()
{
    setWindowIcon(QPixmap("images/avm-energo.png"));
    QMessageBox::about(this, PROGCAPTION,
        "ООО \"АВМ-Энерго\" \n"
        "2015-2020 гг.\n"
        "info@avmenergo.ru");
}

void AvmDebug::Disconnect()
{
    INFOMSG("Disconnect()");
    AlarmW->Clear();
    if (!StdFunc::IsInEmulateMode())
    {
        BdaTimer->stop();
        if (Board::GetInstance().connectionState() != Board::ConnectionState::Closed)
            EProtocom::GetInstance().Disconnect();
    }
}

void AvmDebug::DisconnectAndClear()
{
    INFOMSG("DisconnectAndClear()");
    //    TimeTimer->stop();
    if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected)
    {
        AlarmW->Clear();
        Disconnect();
        CloseDialogs();
        ClearTW();
        ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
        if (MainTW == nullptr)
        {
            ERMSG("Пустой MainTW");
            return;
        }
        MainTW->hide();
        StdFunc::SetEmulated(false);
    }

    Reconnect = false;
}

void AvmDebug::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
}

void AvmDebug::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
        emit Finished();
    if (e->key() == Qt::Key_Escape)
        StdFunc::cancel();
    QMainWindow::keyPressEvent(e);
}

void AvmDebug::MainTWTabClicked(int tabindex)
{
    m_Module->parentTWTabClicked(tabindex);
}

void AvmDebug::SetDefConf()
{
    m_Module->setDefConf();
    QMessageBox::information(this, "Успешно", "Конфигурация по умолчанию", QMessageBox::Ok);
}

void AvmDebug::closeEvent(QCloseEvent *event)
{
    DisconnectAndClear();
    event->accept();
}
