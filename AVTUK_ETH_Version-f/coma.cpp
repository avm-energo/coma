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

#include "../check/checkdialogkiv.h"
#include "../check/chekdialogktf.h"
#include "../config/confdialogkdv.h"
#include "../config/confdialogkiv.h"
#include "../config/confdialogktf.h"
#include "../dialogs/errordialog.h"
#include "../dialogs/settingsdialog.h"
#include "../gen/colors.h"
#include "../gen/files.h"
#include "../gen/logclass.h"
#include "../gen/modulebsi.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../modbus/modbus.h"
#include "../usb/commands.h"
#include "../widgets/emessagebox.h"
#include "../widgets/etabwidget.h"
#include "../widgets/waitwidget.h"
#include "../widgets/wd_func.h"

#include <QApplication>
#include <QCursor>
#include <QDialog>
#include <QDir>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QSettings>
#include <QSplashScreen>
#include <QStandardPaths>
#include <QStringListModel>
#include <QToolBar>

Coma::Coma(QWidget *parent) : QMainWindow(parent)
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
    DEV_BROADCAST_DEVICEINTERFACE devInt;
    ZeroMemory(&devInt, sizeof(devInt));
    devInt.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    devInt.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    devInt.dbcc_classguid = { 0x25dbce51, 0x6c8f, 0x4a72, 0x8a, 0x6d, 0xb5, 0x4c, 0x2b, 0x4f, 0xc8, 0x35 };
    ;

    HANDLE blub;
    blub = RegisterDeviceNotification((HANDLE)winId(), &devInt, DEVICE_NOTIFY_WINDOW_HANDLE);
#else

#endif

    S2Config = new QVector<S2::DataRec>;
    // S2ConfigForTune.clear();
    Disconnected = true;
    Reconnect = false;
    TimeD = nullptr;
    MainConfDialog = nullptr;
    ConfB = ConfM = nullptr;
    CheckB = CheckM = nullptr;
    Wpred = Walarm = nullptr;
    CorD = nullptr;
    CurTabIndex = -1;
    for (int i = 0; i < 20; ++i)
    {
        PredAlarmEvents[i] = 0;
        AlarmEvents[i] = 0;
    }
    ActiveThreads = 0;
    Alarm = new AlarmClass;

    New104();
    NewModbus();
    NewUSB();
    NewTimers();
    LoadSettings();

    splash->finish(this);
}

Coma::~Coma()
{
    Disconnect();
}

void Coma::SetupUI()
{
    setWindowTitle(PROGCAPTION);
    QString tmps = "QMainWindow {background-color: " + QString(MAINWINCLR) + ";}";
    setStyleSheet(tmps);
    setMinimumSize(QSize(800, 700));
    QWidget *wdgt = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QToolBar *tb = new QToolBar;

    tb->setStyleSheet("QToolBar {background: 0px; margin: 0px; spacing: 5px; padding: 0px;}");
    tb->setIconSize(QSize(20, 20));
    tb->addAction(QIcon("images/play.png"), "Соединение", this, &Coma::StartWork);
    tb->addAction(QIcon("images/stop.png"), "Разрыв соединения", this, &Coma::DisconnectAndClear);
    tb->addSeparator();
    tb->addAction(QIcon("images/settings.png"), "Соединение", this, &Coma::StartSettingsDialog);
    tb->addAction(QIcon("images/skull-and-bones.png"), "Соединение", this, &Coma::ShowErrorDialog);

    hlyout->addWidget(tb);

    AlarmW = new AlarmWidget(Alarm);
    hlyout->addWidget(AlarmW, Qt::AlignCenter);

    lyout->addLayout(hlyout);
    lyout->addWidget(Least());
    wdgt->setLayout(lyout);
    setCentralWidget(wdgt);
    SetupMenubar();
}

QWidget *Coma::Least()
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

void Coma::SetupMenubar()
{
    QMenuBar *menubar = new QMenuBar;
    QString tmps = "QMenuBar {background-color: " + QString(MAINWINCLRA1)
        + ";}"
          "QMenuBar::item {background-color: "
        + QString(MAINWINCLRA1) + ";}";
    menubar->setStyleSheet(tmps);
    QMenu *menu = new QMenu;
    menu->setTitle("Главное");

    QAction *act = new QAction(this);
    act->setText("Выход");
    connect(act, &QAction::triggered, this, &Coma::close);
    menu->addAction(act);

    act = new QAction(this);
    act->setText("Соединение");
    act->setIcon(QIcon("images/play.png"));
    connect(act, &QAction::triggered, this, &Coma::StartWork);
    menu->addAction(act);

    act = new QAction(this);
    act->setText("Разрыв соединения");
    act->setIcon(QIcon("images/stop.png"));
    connect(act, &QAction::triggered, this, &Coma::DisconnectAndClear);
    menu->addAction(act);
    menubar->addMenu(menu);

    act = new QAction(this);
    act->setText("О программе");
    connect(act, &QAction::triggered, this, &Coma::GetAbout);
    menubar->addAction(act);

    menubar->addSeparator();

    setMenuBar(menubar);
}

void Coma::addConfTab(ETabWidget *MainTW, QString str)
{
    MainTW->addTab(CorD, str);
    CorD->corDIndex = MainTW->indexOf(CorD);
    if (MainInterface == I_RS485)
        ChModbus->CorIndex = CorD->corDIndex;
}

void Coma::StartWork()
{
    if (!Reconnect)
    {
        QEventLoop loop;
        Cancelled = false;
        ConnectDialog *dlg = new ConnectDialog;
        connect(dlg, &ConnectDialog::Accepted, this, &Coma::SetConnection);
        connect(dlg, &ConnectDialog::Cancelled, this, &Coma::Cancel);
        connect(this, &Coma::CloseConnectDialog, dlg, &ConnectDialog::close);
        connect(this, &Coma::CloseConnectDialog, &loop, &QEventLoop::quit);
        dlg->show();
        loop.exec();
        //        ShowInterfaceDialog();
        //        ShowConnectDialog();
        if (Cancelled)
        {
            ERMSG("Отмена подключения");
            return;
        }
        S2ConfigForTune.clear();
        SaveSettings();
    }

    // Stage3

    DisconnectAndClear();
    QString str;
    MTypeB = 0;
    MTypeM = 0;
    CurTabIndex = -1;
    ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
    if (MainTW == nullptr)
    {
        ERMSG("MainTW is empty");
        return;
    }
    connect(MainTW, &ETabWidget::tabClicked, this, &Coma::MainTWTabClicked);

    if (MainInterface == I_USB)
    {
        if (Commands::Connect() != NOERROR) // cn->Connect()
        {
            EMessageBox::error(this, "Ошибка", "Не удалось установить связь");
            QApplication::restoreOverrideCursor();
            ERMSG("cn: can't connect");
            return;
        }
        int res = ModuleBSI::SetupBSI();
        if (res == GENERALERROR)
        {
            EMessageBox::error(this, "Ошибка", "Не удалось установить связь");
            ERMSG("BSI read error");
            return;
        }
        else if (res == NOERROR)
        {
            if (ModuleBSI::ModuleTypeString != "")
                EMessageBox::information(this, "Успешно", "Связь с " + ModuleBSI::ModuleTypeString + " установлена");
            else
            {
                EMessageBox::error(this, "Ошибка", "Неизвестный тип модуля");
                ERMSG("Unknown module type");
                return;
            }
        }
        ActiveThreads |= THREAD::USB;
        MTypeB = ModuleBSI::GetMType(BoardTypes::BT_BASE);
        MTypeM = ModuleBSI::GetMType(BoardTypes::BT_MEZONIN);
        //        emit USBBsiRefresh();
    }
    else
    {
        if (MainInterface == I_ETHERNET)
        {
            if (!Ch104->Working())
                Ch104->Connect(ConnectSettings.iec104st);
            ActiveThreads |= THREAD::P104;
        }
        else
        {
            if (ChModbus->Connect(ConnectSettings.serialst) != NOERROR)
            {
                ERMSG("Modbus not connected");
                return;
            }
            ChModbus->BSIrequest();
            ActiveThreads |= THREAD::MODBUS;
        }
        QElapsedTimer tmr;
        tmr.start();
        while ((MTypeB == 0) && (tmr.elapsed() < INTERVAL::WAIT) && !Cancelled)
            QCoreApplication::processEvents();
        if (MTypeB == 0)
        {
            EMessageBox::error(this, "Ошибка", "Не удалось соединиться с прибором");
            DisconnectAndClear();
            ERMSG("Не получили BSI, нет соединения");
            //            Disconnect();
            return;
        }
    }
    // MTypeB & MTypeM are acquired
    MTypeB <<= 8;

    Disconnected = false;
    Reconnect = true;

    PrepareDialogs();

    str = (CheckM == nullptr) ? "Текущие параметры" : "Текущие параметры\nБазовая";
    if (CheckB != nullptr)
    {
        CheckB->setMinimumHeight(500);
        MainTW->addTab(CheckB, str);
        CheckIndex = MainTW->indexOf(CheckB);
        if (MainInterface == I_RS485)
            ChModbus->CheckIndex = CheckIndex;
    }
    str = (CheckB == nullptr) ? "Текущие параметры" : "Текущие параметры\nМезонин";
    if (CheckM != nullptr)
        MainTW->addTab(CheckM, str);

    if (ConfB != nullptr)
    {
        str = (ConfM == nullptr) ? "Конфигурирование" : "Конфигурирование\nБазовая";
        MainTW->addTab(ConfB, str);
        if (ConfB->IsNeededDefConf)
        {
            ConfB->SetDefConf();
            EMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию");
        }
        connect(ConfB, &AbstractConfDialog::NewConfToBeLoaded, this, &Coma::Fill);
        connect(ConfB, &AbstractConfDialog::DefConfToBeLoaded, this, &Coma::SetDefConf);
    }
    if (ConfM != nullptr)
    {
        str = (ConfB == nullptr) ? "Конфигурирование" : "Конфигурирование\nМезонин";
        MainTW->addTab(ConfM, str);
        ConfIndex = MainTW->indexOf(ConfM);

        if (ConfM->IsNeededDefConf)
        {
            ConfM->SetDefConf();
            EMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию");
        }
        connect(ConfM, &AbstractConfDialog::NewConfToBeLoaded, this, &Coma::Fill);
        connect(ConfM, &AbstractConfDialog::DefConfToBeLoaded, this, &Coma::SetDefConf);
    }
    if (TimeD != nullptr)
    {
        MainTW->addTab(TimeD, "Время");
        TimeIndex = MainTW->indexOf(TimeD);
        ChModbus->TimeIndex = TimeIndex;
        connect(TimeTimer, &QTimer::timeout, TimeD, &MNKTime::slot2_timeOut);
    }

    if (CorD != nullptr)
    {
        switch (MTypeB)
        {
        case Config::MTB_A2:
            switch (MTypeM)
            {
            case Config::MTM_84:
                addConfTab(MainTW, "Начальные значения");
                break;

            case Config::MTM_87:
                addConfTab(MainTW, "Текущее значение");
                break;
            }
            break;
        case Config::MTB_A3:

            switch (MTypeM)
            {
            case Config::MTM_87:
                addConfTab(MainTW, "Начальные значения");
                break;
            }

            break;
        };
    }

    if (MainInterface != I_RS485)
        MainTW->addTab(JourD, "Журналы");

    if (ModuleBSI::Health() & HTH_CONFIG) // нет конфигурации
        Error::ShowErMsg(ER_NOCONF);
    if (ModuleBSI::Health() & HTH_REGPARS) // нет коэффициентов
        Error::ShowErMsg(ER_NOTUNECOEF);

    if (MainInterface == I_USB)
    {
        FwUpD = new fwupdialog;
        MainTW->addTab(FwUpD, "Загрузка ВПО");
    }

    MainTW->addTab(IDialog, "О приборе");
    IDialog->FillBsi();

    MainTW->repaint();
    MainTW->show();

    INFOMSG("MainTW created");
    if (MainInterface == I_USB)
        BdaTimer->start();
}

void Coma::PrepareDialogs()
{
    IDialog = new InfoDialog;
    JourD = new JournalDialog(Ch104);
    TimeD = new MNKTime;

    AlarmStateAllWidget = new AlarmStateAll;
    connect(AlarmW, &AlarmWidget::AlarmButtonPressed, AlarmStateAllWidget, &QWidget::show);

    switch (MTypeB)
    {
    case Config::MTB_A2:
        switch (MTypeM)
        {
        case Config::MTM_84:
            CheckB = new CheckDialogKIV(BoardTypes::BT_BASE);
            S2Config->clear();
            if (MainInterface != I_RS485)
                ConfM = new ConfDialogKIV(S2Config);
            CorD = new CorDialog;

            WarnAlarmKIVWidget = new WarnAlarmKIV(Alarm);
            connect(AlarmW, &AlarmWidget::ModuleWarnButtonPressed, WarnAlarmKIVWidget, &QWidget::show);
            connect(Alarm, &AlarmClass::SetWarnAlarmColor, WarnAlarmKIVWidget, &WarnAlarmKIV::Update);

            AvarAlarmKIVWidget = new AvarAlarmKIV(Alarm);
            connect(AlarmW, &AlarmWidget::AlarmButtonPressed, AvarAlarmKIVWidget, &QWidget::show);
            connect(Alarm, &AlarmClass::SetAlarmColor, AvarAlarmKIVWidget, &AvarAlarmKIV::Update);

            break;

        case Config::MTM_87:
            CheckB = new ChekDialogKTF(BoardTypes::BT_BASE);
            S2Config->clear();
            if (MainInterface != I_RS485)
                ConfM = new ConfDialogKTF(S2Config);
            CorD = new CorDialogKTF;

            WarnAlarmKTFWidget = new WarnAlarmKTF(Alarm);
            connect(AlarmW, &AlarmWidget::ModuleWarnButtonPressed, WarnAlarmKTFWidget, &QWidget::show);
            connect(Alarm, &AlarmClass::SetWarnAlarmColor, WarnAlarmKTFWidget, &WarnAlarmKTF::Update);

            AvarAlarmKTFWidget = new AvarAlarmKTF(Alarm);
            connect(AlarmW, &AlarmWidget::ModuleAlarmButtonPressed, AvarAlarmKTFWidget, &QWidget::show);
            connect(Alarm, &AlarmClass::SetAlarmColor, AvarAlarmKTFWidget, &AvarAlarmKTF::Update);

            break;
        }
        break;
    case Config::MTB_A3:

        switch (MTypeM)
        {
        case Config::MTM_87:

            S2Config->clear();
            if (MainInterface != I_RS485)
                ConfM = new ConfDialogKDV(S2Config);

            break;
        };

        break;
    };

    NewTimersBda();

    connect(this, &Coma::ClearBsi, IDialog, &InfoDialog::ClearBsi);
    connect(AlarmW, SIGNAL(SetWarnAlarmColor(QList<bool>)), CheckB, SLOT(SetWarnAlarmColor(QList<bool>)));
    connect(AlarmW, SIGNAL(SetAlarmColor(QList<bool>)), CheckB, SLOT(SetAlarmColor(QList<bool>)));

    if (MainInterface == I_ETHERNET)
    {
        connect(Ch104, SIGNAL(Floatsignalsready(IEC104Thread::FlSignals104 *)), CheckB,
            SLOT(UpdateFlData(IEC104Thread::FlSignals104 *)));
        connect(Ch104, SIGNAL(Sponsignalsready(IEC104Thread::SponSignals *)), CheckB,
            SLOT(UpdateSponData(IEC104Thread::SponSignals *)));

        connect(TimeD, &MNKTime::ethTimeRequest, Ch104, &IEC104::InterrogateTimeGr15);
        connect(Ch104, &IEC104::Bs104signalsready, TimeD, &MNKTime::FillTimeFrom104);
        connect(TimeD, &MNKTime::ethWriteTimeToModule, Ch104, &IEC104::com51WriteTime);

        connect(CorD, &AbstractCorDialog::SendCom45, Ch104, &IEC104::Com45);
        connect(CorD, &AbstractCorDialog::SendCom50, Ch104, &IEC104::Com50);
        connect(CorD, &AbstractCorDialog::CorReadRequest, Ch104, &IEC104::CorReadRequest);
        connect(Ch104, &IEC104::SendMessageOk, CorD, &AbstractCorDialog::MessageOk);
        connect(Ch104, &IEC104::Floatsignalsready, CorD, &AbstractCorDialog::UpdateFlCorData);

        connect(ConfM, &AbstractConfDialog::ReadConfig, Ch104, &IEC104::SelectFile);
        connect(Ch104, &IEC104::SendS2fromiec104, ConfM, &AbstractConfDialog::FillConf);
        connect(ConfM, &AbstractConfDialog::writeConfFile, Ch104, &IEC104::FileReady);
        connect(Ch104, &IEC104::SendConfMessageOk, ConfM, &AbstractConfDialog::WriteConfMessageOk);
    }
    else if (MainInterface == I_RS485)
    {
        connect(ChModbus, SIGNAL(ModbusState(ConnectionStates)), CheckB, SLOT(onModbusStateChanged(ConnectionStates)));
        connect(ChModbus, SIGNAL(SignalsReceived(QList<ModBus::SignalStruct>)), CheckB,
            SLOT(UpdateModBusData(QList<ModBus::SignalStruct>)));

        connect(TimeD, &MNKTime::modBusTimeRequest, ChModbus, &ModBus::ReadTime);
        connect(ChModbus, &ModBus::TimeSignalsReceived, TimeD, &MNKTime::FillTimeFromModBus);
        connect(TimeD, &MNKTime::modbusWriteTimeToModule, ChModbus, &ModBus::WriteTime);
        connect(ChModbus, &ModBus::TimeReadError, TimeD, &MNKTime::ErrorRead);
        connect(ChModbus, &ModBus::TimeWritten, TimeD, &MNKTime::TimeWritten);

        connect(ChModbus, &ModBus::ErrorRead, CorD, &AbstractCorDialog::ErrorRead);
        connect(ChModbus, &ModBus::CorSignalsReceived, CorD, &AbstractCorDialog::ModBusUpdateCorData);
        connect(ChModbus, &ModBus::CorSignalsWritten, CorD, &AbstractCorDialog::MessageOk);
        connect(CorD, &AbstractCorDialog::RS485WriteCorBd, ChModbus,
            &ModBus::ModWriteCor); //, int*)));
        connect(CorD, &AbstractCorDialog::RS485ReadCorBd, ChModbus, &ModBus::ModReadCor);
    }
}

void Coma::CloseDialogs()
{
    if (TimeD != nullptr)
        TimeD->close();
    if (CheckB != nullptr)
        CheckB->close();
    if (CheckM != nullptr)
        CheckM->close();
    if (MainConfDialog != nullptr)
        MainConfDialog->close();
    if (ConfB != nullptr)
        ConfB->close();
    if (ConfM != nullptr)
        ConfM->close();

    if (Wpred != nullptr)
        Wpred->close();
    if (Walarm != nullptr)
        Walarm->close();

    if (CorD != nullptr)
        CorD->close();
    if (IDialog != nullptr)
        IDialog->close();
    if (JourD != nullptr)
        JourD->close();
}

void Coma::New104()
{
    Ch104 = new IEC104(S2Config);
    connect(this, SIGNAL(StopCommunications()), Ch104, SLOT(StopAllThreads()));
    connect(Ch104, SIGNAL(Finished()), this, SLOT(Ch104Finished()));
    // connect(Ch104,SIGNAL(Sponsignalsready(IEC104Thread::SponSignals*)),this,SLOT(UpdatePredAlarmEvents(IEC104Thread::SponSignals*)));
    connect(Ch104, SIGNAL(SetDataSize(int)), this, SLOT(SetProgressBar1Size(int)));
    connect(Ch104, SIGNAL(SetDataCount(int)), this, SLOT(SetProgressBar1(int)));
    connect(Ch104, SIGNAL(ReconnectSignal()), this, SLOT(ReConnect()));
    connect(Ch104, SIGNAL(Sponsignalsready(IEC104Thread::SponSignals *)), this,
        SLOT(UpdateAlarm104(IEC104Thread::SponSignals *)));
    connect(Ch104, SIGNAL(Bs104signalsready(IEC104Thread::BS104Signals *)), this,
        SLOT(FillBSI(IEC104Thread::BS104Signals *)));
}

void Coma::NewModbus()
{
    ChModbus = new ModBus;
    connect(this, &Coma::StopCommunications, ChModbus, &ModBus::Stop);
    connect(ChModbus, &ModBus::Finished, this, &Coma::ModBusFinished);
    //  connect(ChModbus,SIGNAL(CoilSignalsReady(ModBus::Coils)), this,
    //  SLOT(ModBusUpdatePredAlarmEvents(ModBus::Coils)));
    connect(ChModbus, &ModBus::ReconnectSignal, this, &Coma::ReConnect);
    connect(ChModbus, SIGNAL(CoilSignalsReady(ModBus::Coils)), Alarm, SLOT(UpdateAlarmModbus(ModBus::Coils)));
    connect(ChModbus, SIGNAL(BsiFromModbus(QList<ModBus::BSISignalStruct>, unsigned int)), this,
        SLOT(FillBSI(QList<ModBus::BSISignalStruct>, unsigned int)));
}

void Coma::NewUSB()
{
    cn = new EUsbHid;
    connect(this, &Coma::StopCommunications, cn, &EAbstractProtocomChannel::Disconnect);
    connect(cn, &EAbstractProtocomChannel::Finished, this, &Coma::USBFinished);
    connect(cn, &EAbstractProtocomChannel::SetDataSize, this, &Coma::SetProgressBar1Size);
    connect(cn, &EAbstractProtocomChannel::SetDataCount, this, &Coma::SetProgressBar1);
    connect(cn, &EAbstractProtocomChannel::ShowError, this, &Coma::ShowErrorMessageBox);
    connect(cn, &EUsbHid::ReconnectSignal, this, &Coma::ReConnect);
    // connect(cn, SIGNAL(ReconnectSignal()), this, SLOT(ReConnect()));
}

void Coma::NewTimers()
{
    TimeTimer = new QTimer;
    TimeTimer->setInterval(1000);

    BdaTimer = new QTimer;
    BdaTimer->setInterval(ANMEASINT);

    ReceiveTimer = new QTimer;
    ReceiveTimer->setInterval(ANMEASINT);
    connect(ReceiveTimer, SIGNAL(timeout()), this, SLOT(FileTimeOut()));

    ReconnectTimer = new QTimer;
    ReconnectTimer->setInterval(INTERVAL::RECONNECT);
    ReconnectTimer->setSingleShot(true);
    connect(ReconnectTimer, SIGNAL(timeout()), this, SLOT(AttemptToRec()));
}

void Coma::NewTimersBda()
{
    connect(BdaTimer, SIGNAL(timeout()), Alarm, SLOT(UpdateAlarmUSB()));

    if (CheckB != nullptr)
        connect(BdaTimer, SIGNAL(timeout()), CheckB, SLOT(USBUpdate()));
}

bool Coma::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result);
    if (eventType == "windows_generic_MSG")
    {
        MSG *msg = reinterpret_cast<MSG *>(message);
        if (msg->message == WM_DEVICECHANGE)
        {
            // ReConnect();
            DEV_BROADCAST_PORT *port = reinterpret_cast<DEV_BROADCAST_PORT *>(msg->lParam);
            auto msgType = msg->wParam;
            switch (msgType)
            {
            case DBT_CONFIGCHANGECANCELED:
                qDebug("DBT_CONFIGCHANGECANCELED");
                break;
            case DBT_CONFIGCHANGED:
                qDebug("DBT_CONFIGCHANGED");
                break;
            case DBT_CUSTOMEVENT:
                qDebug("DBT_CUSTOMEVENT");
                break;
            case DBT_DEVICEARRIVAL:
            {
                QString device;
#ifdef UNICODE
                device = QString::fromStdWString(port->dbcp_name);
#else
                device = QString::fromStdString(port->dbcp_name);
#endif
                qDebug() << "DBT_DEVICEARRIVAL " << device;
                break;
            }
            case DBT_DEVICEQUERYREMOVE:
                qDebug("DBT_DEVICEQUERYREMOVE");
                break;
            case DBT_DEVICEQUERYREMOVEFAILED:
                qDebug("DBT_DEVICEQUERYREMOVEFAILED");
                break;
            case DBT_DEVICEREMOVEPENDING:
                qDebug("DBT_DEVICEREMOVEPENDING");
                break;
            case DBT_DEVICEREMOVECOMPLETE:
                qDebug("DBT_DEVICEREMOVECOMPLETE");
                // С ReConnect не работает
                if (!Disconnected)
                    Disconnect();
                // emit StopCommunications();
                // Disconnected = true;
                // DisconnectAndClear();
                // AttemptToRec();
                break;
            case DBT_DEVICETYPESPECIFIC:
                qDebug("DBT_DEVICETYPESPECIFIC");
                break;
            case DBT_QUERYCHANGECONFIG:
                qDebug("DBT_QUERYCHANGECONFIG");
                break;
            case DBT_DEVNODES_CHANGED:
                qDebug("DBT_DEVNODES_CHANGED");
                break;
            case DBT_USERDEFINED:
                qDebug("DBT_USERDEFINED");
                break;
            default:
                qDebug() << "Default";
                break;
            }
        }
    }
    return false;
}

void Coma::SetMode(int mode)
{
    Mode = mode;
}

void Coma::Go(const QString &parameter)
{
    Q_UNUSED(parameter)
    if (Mode != COMA_GENERALMODE)
        StdFunc::SetEmulated(true);
    SetupUI();
    show();
}

void Coma::ReConnect()
{
    if (Reconnect)
    {
        // QDialog *dlg = new QDialog;

        INFOMSG("Reconnect()");
        TimeTimer->stop();
        if (!Disconnected)
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
        msgBox.setText("Связь разорвана.\nПопытка переподключения будет выполнена через 3 секунды");
        msgBox.setIcon(QMessageBox::Warning);
        connect(ReconnectTimer, &QTimer::timeout, &msgBox, &QMessageBox::close);
        ReconnectTimer->start();
        msgBox.show();
        StdFunc::Wait(INTERVAL::RECONNECT);
        msgBox.close();
    }
}

void Coma::AttemptToRec()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    SaveSettings();
    QApplication::restoreOverrideCursor();
    StartWork();
}

void Coma::ConnectMessage()
{
    QMessageBox msgBox;

    msgBox.setIcon(QMessageBox::Information);
    if (MainInterface == I_USB)
        msgBox.setText("Связь с " + ModuleBSI::ModuleTypeString + " установлена");
    else
        msgBox.setText("Связь с " + ConnectSettings.name + " установлена");
    msgBox.show();
    StdFunc::Wait(INTERVAL::WAIT);
    msgBox.close();
}

void Coma::LoadSettings()
{
    QString HomeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" + PROGNAME + "/";
    QSettings *sets = new QSettings("EvelSoft", PROGNAME);
    StdFunc::SetHomeDir(sets->value("Homedir", HomeDir).toString());
}

void Coma::SaveSettings()
{
    QSettings *sets = new QSettings("EvelSoft", PROGNAME);
    sets->setValue("Homedir", StdFunc::GetHomeDir());
}

void Coma::ClearTW()
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

int Coma::CheckPassword()
{
    PasswordValid = false;
    StdFunc::ClearCancel();
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
    connect(dlg, SIGNAL(Finished(QString)), this, SLOT(PasswordCheck(QString)));
    connect(this, SIGNAL(PasswordChecked()), &PasswordLoop, SLOT(quit()));
    dlg->show();
    PasswordLoop.exec();
    if (StdFunc::IsCancelled())
    {
        ERMSG("Отмена ввода пароля");
        return GENERALERROR;
    }

    if (!PasswordValid)
    {
        ERMSG("Пароль введён неверно");
        EMessageBox::error(this, "Неправильно", "Пароль введён неверно");
        return GENERALERROR;
    }
    return NOERROR;
}

void Coma::SetMainDefConf()
{
    if (MainConfDialog != nullptr)
        MainConfDialog->SetDefConf();
}

void Coma::SetBDefConf()
{
    if (ConfB != nullptr)
        ConfB->SetDefConf();
}

void Coma::SetMDefConf()
{
    if (ConfM != nullptr)
        ConfM->SetDefConf();
}

void Coma::Fill()
{
    if (MainConfDialog != nullptr)
        MainConfDialog->Fill();
    if (ConfB != nullptr)
        ConfB->Fill();
    if (ConfM != nullptr)
        ConfM->Fill();
}

void Coma::FillBSI(IEC104Thread::BS104Signals *sig)
{
    unsigned int i, signum;
    int startadr = 0;

    memcpy(&startadr, &(sig->BS.SigAdr[0]), sizeof(sig->BS.SigAdr));
    signum = sig->SigNumber;
    INFOMSG("FillBSIe(): address=" + QString::number(startadr));

    if ((signum < sizeof(ModuleBSI::ModuleBsi)) && (startadr >= BSIREG && startadr <= BSIENDREG))
    {
        for (i = 0; i < signum; ++i)
            memcpy(((quint32 *)(&ModuleBSI::ModuleBsi) + (i + startadr - 1)), (((quint32 *)(&sig->BS.SigVal) + 4 * i)),
                sizeof(sig->BS.SigVal));
        MTypeB = ModuleBSI::ModuleBsi.MTypeB;
        MTypeM = ModuleBSI::ModuleBsi.MTypeM;
        // ModuleBSI::ModuleBsi.Hth = ModuleBSI::ModuleBsi.Hth;
    }
}

void Coma::FillBSI(QList<ModBus::BSISignalStruct> sig, unsigned int sigsize)
{
    unsigned int i;
    int startadr = sig.at(0).SigAdr;

    INFOMSG("FillBSIm()");

    if ((sigsize < sizeof(ModuleBSI::ModuleBsi)) && (startadr == BSIREG))
    {
        for (i = 0; i < sigsize; ++i)
            memcpy(((quint32 *)(&ModuleBSI::ModuleBsi) + (i + startadr - 1)), &sig.at(i).Val, sizeof(sig.at(i).Val));
        MTypeB = ModuleBSI::ModuleBsi.MTypeB;
        MTypeM = ModuleBSI::ModuleBsi.MTypeM;
        // ModuleBSI::ModuleBsi.Hth = ModuleBSI::ModuleBsi.Hth;
    }
}

void Coma::PasswordCheck(QString psw)
{
    if (psw == "se/7520a")
        PasswordValid = true;
    else
        PasswordValid = false;
    emit PasswordChecked();
}

void Coma::StartSettingsDialog()
{
    SettingsDialog *dlg = new SettingsDialog;
    dlg->show();
    SaveSettings();
}

void Coma::ShowErrorDialog()
{
    ErrorDialog *dlg = new ErrorDialog;
    dlg->show();
}

void Coma::SetProgressBar1Size(int size)
{
    fileSize = size;
    SetProgressBarSize("1", size);
}

void Coma::SetProgressBar1(int cursize)
{
    curfileSize = cursize;
    ReceiveTimer->stop();
    ReceiveTimer->setInterval(5000);
    SetProgressBar("1", cursize);
    ReceiveTimer->start();
}

void Coma::FileTimeOut()
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
    if (fileSize != curfileSize && MainInterface != I_USB)
        EMessageBox::information(this, "Ошибка", "Ошибка");
}

void Coma::SetProgressBar2Size(int size)
{
    SetProgressBarSize("2", size);
}

void Coma::SetProgressBar2(int cursize)
{
    SetProgressBar("2", cursize);
}

void Coma::SetCancelled()
{
    Cancelled = true;
}

void Coma::SetProgressBarSize(QString prbnum, int size)
{
    QString prbname = "prb" + prbnum + "prb";
    QString lblname = "prb" + prbnum + "lbl";
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

void Coma::SetProgressBar(QString prbnum, int cursize)
{
    QString prbname = "prb" + prbnum + "prb";
    QString lblname = "prb" + prbnum + "lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb != nullptr)
    {
        prb->setValue(cursize);
        WDFunc::SetLBLText(
            this, lblname, StdFunc::PrbMessage() + QString::number(cursize) + " из " + QString::number(prb->maximum()));
    }
}

void Coma::GetAbout()
{
    setWindowIcon(QPixmap("images/avm-energo.png"));
    QMessageBox::about(this, PROGCAPTION,
        "ООО \"АВМ-Энерго\" \n"
        "2015-2020 гг.\n"
        "info@avmenergo.ru");
}

void Coma::Disconnect()
{
    INFOMSG("Disconnect()");
    if (!StdFunc::IsInEmulateMode())
    {
        if (MainInterface == I_USB)
        {
            BdaTimer->stop();
            TimeFunc::Wait(100);
            if (cn->Connected)
                cn->Disconnect();
        }
        else
        {
            emit StopCommunications();
            while (ActiveThreads) // wait for all threads to finish
                QCoreApplication::processEvents();
        }

        Disconnected = true;
    }
}

void Coma::DisconnectAndClear()
{
    INFOMSG("DisconnectAndClear()");
    TimeTimer->stop();
    if (!Disconnected)
    {
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
        MainTW->hide();
        StdFunc::SetEmulated(false);
        if (Reconnect)
            EMessageBox::information(this, "Разрыв связи", "Связь разорвана");
        else
            EMessageBox::information(this, "Разрыв связи", "Не удалось установить связь");
    }
    Reconnect = false;
}

void Coma::ShowErrorMessageBox(QString message)
{
    EMessageBox::error(this, "Ошибка", message);
}

void Coma::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
}

void Coma::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
        emit Finished();
    if (e->key() == Qt::Key_Escape)
        StdFunc::Cancel();
    QMainWindow::keyPressEvent(e);
}

void Coma::MainTWTabClicked(int tabindex)
{
    if (tabindex == CurTabIndex) // to prevent double function invocation by doubleclicking on tab
        return;
    CurTabIndex = tabindex;
    ChModbus->Tabs(tabindex);
    if (CorD != nullptr)
        CorD->GetCorBd(tabindex);
    if (CheckB != nullptr)
    {
        if (tabindex == CheckIndex)
            BdaTimer->start();
        else
            BdaTimer->stop();
    }
    if (TimeD != nullptr)
    {
        if (tabindex == TimeIndex)
            TimeTimer->start();
        else
            TimeTimer->stop();
    }
    else
        TimeTimer->stop();

    if (ConfM != nullptr)
    {
        if (tabindex == ConfIndex)
            ConfM->ReadConf();
    }
}

void Coma::SetConnection(ConnectDialog::ConnectStruct *st)
{
    ConnectSettings = *st;
    emit CloseConnectDialog();
}

void Coma::Cancel()
{
    Cancelled = true;
    emit CloseConnectDialog();
}

void Coma::ModBusFinished()
{
    ActiveThreads &= ~THREAD::MODBUS;
}

void Coma::Ch104Finished()
{
    ActiveThreads &= ~THREAD::P104;
}

void Coma::USBFinished()
{
    ActiveThreads &= ~THREAD::USB;
}

void Coma::SetDefConf()
{
    SetMainDefConf();
    SetBDefConf();
    SetMDefConf();
    Fill();
    EMessageBox::information(this, "Успешно", "Конфигурация по умолчанию");
}

void Coma::closeEvent(QCloseEvent *event)
{
    DisconnectAndClear();
    event->accept();
}

void Coma::CheckTimeFinish()
{
    TimeThrFinished = true;
}

void Coma::CheckModBusFinish()
{
    TimeThrFinished = true;
}
