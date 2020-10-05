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

#include "../check/checkdialogharmonickdv.h"
#include "../check/checkdialogharmonicktf.h"
#include "../check/checkdialogkdv.h"
#include "../check/checkdialogktf.h"
#include "../check/checkdialogvibrkdv.h"
#include "../check/checkkivdialog.h"
#include "../config/confdialogkdv.h"
#include "../config/confdialogktf.h"
#include "../config/confkivdialog.h"
#include "../dialogs/errordialog.h"
#include "../dialogs/keypressdialog.h"
#include "../dialogs/settingsdialog.h"
#include "../gen/stdfunc.h"
#include "../tune/kiv/tunekivdialog.h"

#include <QApplication>
#include <QDir>
#include <QMenuBar>
#include <QProgressBar>
#include <QSplashScreen>
#include <QStandardPaths>
#include <QToolBar>
#include <QtGlobal>

void registerForDeviceNotification(AvmDebug *ptr)
{
    DEV_BROADCAST_DEVICEINTERFACE devInt;
    ZeroMemory(&devInt, sizeof(devInt));
    devInt.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    devInt.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    devInt.dbcc_classguid = { 0x25dbce51, 0x6c8f, 0x4a72, 0x8a, 0x6d, 0xb5, 0x4c, 0x2b, 0x4f, 0xc8, 0x35 };

    HDEVNOTIFY blub;
    blub = RegisterDeviceNotification((HDEVNOTIFY)ptr->winId(), &devInt, DEVICE_NOTIFY_WINDOW_HANDLE);
}

AvmDebug::AvmDebug(QWidget *parent) : QMainWindow(parent)
{
    QSplashScreen *splash = new QSplashScreen(QPixmap("images/2.1.x.png"));
    splash->show();
    splash->showMessage("Подготовка окружения...", Qt::AlignRight, Qt::white);
    // http://stackoverflow.com/questions/2241808/checking-if-a-folder-exists-and-creating-folders-in-qt-c
    QDir dir(StdFunc::GetHomeDir());
    if (!dir.exists())
        dir.mkpath(".");
    StdFunc::Init();
    Error::Init();

#ifdef __linux__
    // linux code goes here
#elif _WIN32
    // Listen to device events
    registerForDeviceNotification(this);
#else

#endif

    S2Config = new S2ConfigType;
    Reconnect = false;
    timeDialog = nullptr;
    mainConfDialog = nullptr;
    //    confBDialog = confMDialog = nullptr;
    //    checkBDialog = checkMDialog = nullptr;
    AlarmStateAllDialog = nullptr;
    //#ifdef AVM_DEBUG
    //    tuneDialog = nullptr;
    //#endif
    //    HarmDialog = nullptr;
    //    VibrDialog = nullptr;
    //    corDialog = nullptr;
    CurTabIndex = -1;
    for (int i = 0; i < 20; ++i)
    {
        PredAlarmEvents[i] = 0;
        AlarmEvents[i] = 0;
    }
    ActiveUSBThread = false;
    Alarm = new AlarmClass(this);

    NewTimers();
    LoadSettings();

    splash->finish(this);
    setStatusBar(WDFunc::NewSB(this));
}

AvmDebug::~AvmDebug()
{
    //    Disconnect();
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
    QToolBar *tb = new QToolBar(this);

    tb->setStyleSheet("QToolBar {background: 0px; margin: 0px; spacing: 5px; padding: 0px;}");
    tb->setIconSize(QSize(20, 20));
    tb->addAction(QIcon("images/play.png"), "Соединение", this, &AvmDebug::StartWork);
    tb->addAction(QIcon("images/stop.png"), "Разрыв соединения", this, &AvmDebug::DisconnectAndClear);
    tb->addSeparator();
    tb->addAction(QIcon("images/settings.png"), "Настройки", [this]() {
        SettingsDialog *dlg = new SettingsDialog;
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->show();
        this->SaveSettings();
    });
    tb->addAction(QIcon("images/skull-and-bones.png"), "Соединение", []() {
        ErrorDialog *dlg = new ErrorDialog;
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->show();
    });

    hlyout->addWidget(tb);

    AlarmW = new AlarmWidget(Alarm, this);
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

void AvmDebug::addConfTab(ETabWidget *MainTW, QString str)
{
    MainTW->addTab(corDialog, str);
    corDialog->corDIndex = MainTW->indexOf(corDialog);
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
    CurTabIndex = -1;
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

    str = (checkMDialog == nullptr) ? "Текущие параметры" : "Текущие параметры\nБазовая";
    if (checkBDialog != nullptr)
    {
        checkBDialog->setMinimumHeight(500);
        MainTW->addTab(checkBDialog, str);
        CheckIndex = MainTW->indexOf(checkBDialog);
    }
    str = (checkBDialog == nullptr) ? "Текущие параметры" : "Текущие параметры\nМезонин";
    if (checkMDialog != nullptr)
        MainTW->addTab(checkMDialog, str);

    if (HarmDialog != nullptr)
    {
        MainTW->addTab(HarmDialog, "Гармоники");
        CheckHarmIndex = MainTW->indexOf(HarmDialog);
    }

    if (VibrDialog != nullptr)
    {
        MainTW->addTab(VibrDialog, "Вибрации");
        CheckVibrIndex = MainTW->indexOf(VibrDialog);
    }

    if (confBDialog != nullptr)
    {
        str = (confMDialog == nullptr) ? "Конфигурирование" : "Конфигурирование\nБазовая";
        MainTW->addTab(confBDialog, str);
        if (confBDialog->IsNeededDefConf)
        {
            confBDialog->SetDefConf();
            QMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию", QMessageBox::Ok);
            // QMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию");
        }
        connect(confBDialog, &AbstractConfDialog::NewConfToBeLoaded, this, &AvmDebug::Fill);
        connect(confBDialog, &AbstractConfDialog::DefConfToBeLoaded, this, &AvmDebug::SetDefConf);
    }
    if (confMDialog != nullptr)
    {
        str = (confBDialog == nullptr) ? "Конфигурирование" : "Конфигурирование\nМезонин";
        MainTW->addTab(confMDialog, str);
        ///
        ConfIndex = MainTW->indexOf(confMDialog);

        if (confMDialog->IsNeededDefConf)
        {
            confMDialog->SetDefConf();
            QMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию", QMessageBox::Ok);
        }
        connect(confMDialog, &AbstractConfDialog::NewConfToBeLoaded, this, &AvmDebug::Fill);
        connect(confMDialog, &AbstractConfDialog::DefConfToBeLoaded, this, &AvmDebug::SetDefConf);
    }
    if (timeDialog != nullptr)
    {
        MainTW->addTab(timeDialog, "Время");
        TimeIndex = MainTW->indexOf(timeDialog);
        connect(TimeTimer, &QTimer::timeout, timeDialog, &TimeDialog::slot2_timeOut);
    }

    if (corDialog != nullptr)
    {
        switch (Board::GetInstance().typeB())
        {
        case Config::MTB_A2:
            switch (Board::GetInstance().typeM())
            {
            case Config::MTM_84:
                addConfTab(MainTW, "Начальные значения");
                break;

            case Config::MTM_87:
                addConfTab(MainTW, "Старение изоляции");
                break;
            }
            break;
        case Config::MTB_A3:
            switch (Board::GetInstance().typeM())
            {
            case Config::MTM_87:
                addConfTab(MainTW, "Старение изоляции");
                break;
            }

            break;
        };
    }

    // временно, пока не напишем интерфейсы   MainTW->addTab(jourDialog, "Журналы");

    if (ModuleBSI::Health() & HTH_CONFIG) // нет конфигурации
        Error::ShowErMsg(Error::Msg::ER_NOCONF);
    if (ModuleBSI::Health() & HTH_REGPARS) // нет коэффициентов
        Error::ShowErMsg(Error::Msg::ER_NOTUNECOEF);

    fwUpDialog = new fwupdialog;
    MainTW->addTab(fwUpDialog, "Загрузка ВПО");

    if (tuneDialog != nullptr)
        MainTW->addTab(tuneDialog, "Регулировка");

    MainTW->addTab(infoDialog, "О приборе");
    infoDialog->FillBsi();

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

    m_Module = Module::createModule();

    switch (Board::GetInstance().typeB())
    {
    case Config::MTB_A2:
        switch (Board::GetInstance().typeM())
        {
        case Config::MTM_84:
        {
            //            checkBDialog = new CheckDialogKIV(BoardTypes::BT_BASE);

            //            S2Config->clear();
            //            confMDialog = new ConfDialogKIV(S2Config);

            //            corDialog = new CorDialog;

            WarnAlarmKIVDialog = new WarnKIV(Alarm);
            connect(AlarmW, &AlarmWidget::ModuleWarnButtonPressed, WarnAlarmKIVDialog, &QDialog::show);
            connect(Alarm, &AlarmClass::SetWarnAlarmColor, WarnAlarmKIVDialog, &WarnKIV::Update);

            AvarAlarmKIVDialog = new AlarmKIV(Alarm);
            connect(AlarmW, &AlarmWidget::ModuleAlarmButtonPressed, AvarAlarmKIVDialog, &QDialog::show);
            connect(Alarm, &AlarmClass::SetAlarmColor, AvarAlarmKIVDialog, &AlarmKIV::Update);

            //            connect(Alarm, SIGNAL(SetWarnAlarmColor(QList<bool>)), checkBDialog,
            //            SLOT(SetWarnAlarmColor(QList<bool>))); connect(Alarm, SIGNAL(SetAlarmColor(QList<bool>)),
            //            checkBDialog, SLOT(SetAlarmColor(QList<bool>)));
            connect(Alarm, &AlarmClass::SetWarnAlarmColor, m_Module, &Module::setWarnColor);
            connect(Alarm, &AlarmClass::SetAlarmColor, m_Module, &Module::setAlarmColor);
            //            tuneDialog = new TuneKIVDialog(S2Config);
            break;
        }
        case Config::MTM_87:
        {
            HarmDialog = new CheckDialogHarmonicKTF(BoardTypes::BT_BASE);
            connect(BdaTimer, &QTimer::timeout, HarmDialog, &AbstractCheckDialog::USBUpdate);

            S2Config->clear();
            confMDialog = new ConfDialogKTF(S2Config);

            checkBDialog = new CheckDialogKTF(BoardTypes::BT_BASE);

            corDialog = new StartupKTFDialog;

            WarnAlarmKTFDialog = new WarnAlarmKTF(Alarm);
            connect(AlarmW, &AlarmWidget::ModuleWarnButtonPressed, WarnAlarmKTFDialog, &QDialog::show);
            connect(Alarm, &AlarmClass::SetWarnAlarmColor, WarnAlarmKTFDialog, &WarnAlarmKTF::Update);

            AvarAlarmKTFDialog = new AvarAlarmKTF(Alarm);
            connect(AlarmW, &AlarmWidget::ModuleAlarmButtonPressed, AvarAlarmKTFDialog, &QDialog::show);
            connect(Alarm, &AlarmClass::SetAlarmColor, AvarAlarmKTFDialog, &AvarAlarmKTF::Update);

            break;
        }
        break;
        }
    case Config::MTB_A3:

        switch (Board::GetInstance().typeM())
        {
        case Config::MTM_87:
            checkBDialog = new CheckDialogKDV(BoardTypes::BT_BASE);

            HarmDialog = new CheckDialogHarmonicKDV(BoardTypes::BT_BASE);
            connect(BdaTimer, &QTimer::timeout, HarmDialog, &AbstractCheckDialog::USBUpdate);

            VibrDialog = new CheckDialogVibrKDV(BoardTypes::BT_BASE);
            connect(BdaTimer, &QTimer::timeout, VibrDialog, &AbstractCheckDialog::USBUpdate);

            S2Config->clear();
            confMDialog = new ConfDialogKDV(S2Config);

            corDialog = new StartupKTFDialog;

            break;
        };

        break;
    };

    connect(this, &AvmDebug::ClearBsi, infoDialog, &InfoDialog::ClearBsi);
    connect(AlarmW, SIGNAL(SetWarnAlarmColor(QList<bool>)), checkBDialog, SLOT(SetWarnAlarmColor(QList<bool>)));
    connect(AlarmW, SIGNAL(SetAlarmColor(QList<bool>)), checkBDialog, SLOT(SetAlarmColor(QList<bool>)));

    NewTimersBda();
}

void AvmDebug::PrepareDialogs()
{
    //    infoDialog = new InfoDialog(this);
    //    jourDialog = new JournalDialog(Ch104);
    //    timeDialog = new TimeDialog(this);

    AlarmStateAllDialog = new AlarmStateAll;

    setupConnections();
}

void AvmDebug::CloseDialogs()
{
    QList<QDialog *> widgets = this->findChildren<QDialog *>();
    // this->findChildren
    for (auto &i : widgets)
    {
        qDebug() << i;
        i->close();
    }

    Alarm->AvarAlarmEvents.clear();
    Alarm->WarnAlarmEvents.clear();

    if (AvarAlarmKIVDialog != nullptr)
        AvarAlarmKIVDialog->close();

    if (AlarmStateAllDialog != nullptr)
    {
        AlrmTimer->stop();
        AlarmStateAllDialog->close();
        AlarmStateAllDialog = nullptr;
    }
    if (WarnAlarmKIVDialog != nullptr)
        WarnAlarmKIVDialog->close();
}

void AvmDebug::NewUSB()
{
    connect(this, &AvmDebug::StopCommunications, EProtocom::GetInstance(), &EProtocom::Disconnect);
    connect(EProtocom::GetInstance()->workerThread(), &QThread::finished, [=]() { ActiveUSBThread = false; });
    connect(EProtocom::GetInstance(), &EProtocom::SetDataSize, this, &AvmDebug::SetProgressBar1Size);
    connect(EProtocom::GetInstance(), &EProtocom::SetDataCount, this, &AvmDebug::SetProgressBar1);
    connect(EProtocom::GetInstance(), &EProtocom::ShowError,
        [this](const QString &msg) { QMessageBox::critical(this, "Ошибка", msg, QMessageBox::Ok); });
}

void AvmDebug::NewTimers()
{
    TimeTimer = new QTimer;
    TimeTimer->setInterval(1000);

    BdaTimer = new QTimer;
    BdaTimer->setInterval(ANMEASINT);

    AlrmTimer = new QTimer;
    AlrmTimer->setInterval(10000);
    AlrmTimer->start();

    ReceiveTimer = new QTimer;
    ReceiveTimer->setInterval(ANMEASINT);
    connect(ReceiveTimer, &QTimer::timeout, this, &AvmDebug::FileTimeOut);

    ReconnectTimer = new QTimer;
    ReconnectTimer->setInterval(INTERVAL::RECONNECT);
    ReconnectTimer->setSingleShot(true);
    connect(ReconnectTimer, &QTimer::timeout, this, &AvmDebug::AttemptToRec);
}

void AvmDebug::NewTimersBda()
{
    connect(BdaTimer, &QTimer::timeout, Alarm, &AlarmClass::UpdateAlarmUSB);
    //   connect(BdaTimer, &QTimer::timeout, AlarmStateAllDialog, &AlarmStateAll::UpdateHealth);

    if (checkBDialog != nullptr)
        connect(BdaTimer, &QTimer::timeout, checkBDialog, &AbstractCheckDialog::USBUpdate);
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
            EProtocom::GetInstance()->usbStateChanged(message);
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
        // QDialog *dlg = new QDialog;

        INFOMSG("Reconnect()");
        TimeTimer->stop();
        if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected)
        {
            qDebug() << "call Disconnect";
            Disconnect();
            emit ClearBsi();
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
    PasswordValid = false;
    StdFunc::ClearCancel();
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
    connect(dlg, &KeyPressDialog::Finished, this, &AvmDebug::PasswordCheck);
    connect(this, &AvmDebug::PasswordChecked, &PasswordLoop, &QEventLoop::quit);
    dlg->show();
    PasswordLoop.exec();
    if (StdFunc::IsCancelled())
    {
        ERMSG("Отмена ввода пароля");
        return Error::Msg::GeneralError;
    }

    if (!PasswordValid)
    {
        ERMSG("Пароль введён неверно");
        QMessageBox::critical(this, "Неправильно", "Пароль введён неверно", QMessageBox::Ok);
        return Error::Msg::GeneralError;
    }
    return Error::Msg::NoError;
}

void AvmDebug::setConf(unsigned char typeConf)
{
    switch (typeConf)
    {
    case 0x01:
        if (mainConfDialog != nullptr)
            mainConfDialog->SetDefConf();
    case 0x02:
        if (confBDialog != nullptr)
            confBDialog->SetDefConf();
    case 0x03:
        if (confMDialog != nullptr)
            confMDialog->SetDefConf();
        break;
    default:
        break;
    }
}

void AvmDebug::Fill()
{
    if (mainConfDialog != nullptr)
        mainConfDialog->Fill();
    if (confBDialog != nullptr)
        confBDialog->Fill();
    if (confMDialog != nullptr)
        confMDialog->Fill();
}

void AvmDebug::PasswordCheck(QString psw)
{
    if (psw == "se/7520a")
        PasswordValid = true;
    else
        PasswordValid = false;
    emit PasswordChecked();
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
            EProtocom::GetInstance()->Disconnect();
    }
}

void AvmDebug::DisconnectAndClear()
{
    INFOMSG("DisconnectAndClear()");
    TimeTimer->stop();
    if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected)
    {
        AlarmW->Clear();
        Disconnect();
        CloseDialogs();
        emit ClearBsi();
        ClearTW();
        ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
        if (MainTW == nullptr)
        {
            ERMSG("Пустой MainTW");
            return;
        }
        // Проверить после отключения алармов
        // if (Reconnect)
        //    QMessageBox::information(this, "Разрыв связи", "Связь разорвана", QMessageBox::Ok, QMessageBox::Ok);
        //        else
        //            QMessageBox::information(this, "Разрыв связи", "Не удалось установить связь");
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
        StdFunc::Cancel();
    QMainWindow::keyPressEvent(e);
}

void AvmDebug::MainTWTabClicked(int tabindex)
{
    if (tabindex == CurTabIndex) // to prevent double function invocation by doubleclicking on tab
        return;
    CurTabIndex = tabindex;
    if (corDialog != nullptr)
        corDialog->GetCorBd(tabindex);
    if (checkBDialog != nullptr || HarmDialog != nullptr || VibrDialog != nullptr)
    {
        if (tabindex == CheckIndex || tabindex == CheckHarmIndex || tabindex == CheckVibrIndex)
            BdaTimer->start();
        else
            BdaTimer->stop();
    }

    if (timeDialog != nullptr)
    {
        if (tabindex == TimeIndex)
            TimeTimer->start();
        else
            TimeTimer->stop();
    }
    else
        TimeTimer->stop();

    if (confMDialog != nullptr)
    {
        if (tabindex == ConfIndex)
            confMDialog->ReadConf();
    }
}

void AvmDebug::SetDefConf()
{
    for (unsigned char i = 0x00; i != 0x03;)
        setConf(++i);
    Fill();
    QMessageBox::information(this, "Успешно", "Конфигурация по умолчанию", QMessageBox::Ok);
}

void AvmDebug::closeEvent(QCloseEvent *event)
{
    DisconnectAndClear();
    event->accept();
}
