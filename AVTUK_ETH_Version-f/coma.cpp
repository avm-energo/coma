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

#include "../check/checkdialogharmonicktf.h"
#include "../check/checkdialogkdv.h"
#include "../check/checkdialogkiv.h"
#include "../check/checkdialogktf.h"
#include "../config/confdialogkdv.h"
#include "../config/confdialogkiv.h"
#include "../config/confdialogktf.h"
#include "../dialogs/errordialog.h"
#include "../dialogs/settingsdialog.h"
#include "../gen/files.h"
#include "../gen/logclass.h"
#include "../gen/modulebsi.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../modbus/modbus.h"
#include "../usb/commands.h"
#include "../widgets/etabwidget.h"
#include "../widgets/waitwidget.h"
#include "../widgets/wd_func.h"

#include <QApplication>
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

    HDEVNOTIFY blub;
    blub = RegisterDeviceNotification((HDEVNOTIFY)winId(), &devInt, DEVICE_NOTIFY_WINDOW_HANDLE);
#else

#endif

    S2Config = new QVector<S2::DataRec>;
    S2ConfigForTune.clear();
    Disconnected = true;
    Reconnect = false;
    timeDialog = nullptr;
    mainConfDialog = nullptr;
    confBDialog = confMDialog = nullptr;
    checkBDialog = checkMDialog = nullptr;
    wPredDialog = wAlarmDialog = nullptr;
    Harm = nullptr;
    corDialog = nullptr;
    CurTabIndex = -1;
    for (int i = 0; i < 20; ++i)
    {
        PredAlarmEvents[i] = 0;
        AlarmEvents[i] = 0;
    }
    ActiveThreads = 0;
    Alarm = new AlarmClass(this);

    NewTimers();
    LoadSettings();

    splash->finish(this);
}

Coma::~Coma()
{
    //    Disconnect();
}

void Coma::SetupUI()
{
    setWindowTitle(PROGCAPTION);
    QString tmps = "QMainWindow {background-color: " + QString(MAINWINCLR) + ";}";
    setStyleSheet(tmps);
    setMinimumSize(QSize(800, 700));
    QWidget *wdgt = new QWidget(this);
    QVBoxLayout *lyout = new QVBoxLayout(wdgt);
    QHBoxLayout *hlyout = new QHBoxLayout;
    QToolBar *tb = new QToolBar(this);

    tb->setStyleSheet("QToolBar {background: 0px; margin: 0px; spacing: 5px; padding: 0px;}");
    tb->setIconSize(QSize(20, 20));
    tb->addAction(QIcon("images/play.png"), "Соединение", this, &Coma::StartWork);
    tb->addAction(QIcon("images/stop.png"), "Разрыв соединения", this, &Coma::DisconnectAndClear);
    tb->addSeparator();
    tb->addAction(QIcon("images/settings.png"), "Настройки", [this]() {
        SettingsDialog *dlg = new SettingsDialog;
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->show();
        this->SaveSettings();
    });
    tb->addAction(QIcon("images/skull-and-bones.png"), "Соединение", [this]() {
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
    QMenuBar *menubar = new QMenuBar(this);
    QString tmps = "QMenuBar {background-color: " + QString(MAINWINCLRA1)
        + ";}"
          "QMenuBar::item {background-color: "
        + QString(MAINWINCLRA1) + ";}";
    menubar->setStyleSheet(tmps);
    QMenu *menu = new QMenu(this);
    menu->setTitle("Главное");

    menu->addAction("Выход", this, &Coma::close);
    menu->addAction(QIcon("images/play.png"), "Соединение", this, &Coma::StartWork);
    menu->addAction(QIcon("images/stop.png"), "Разрыв соединения", this, &Coma::DisconnectAndClear);

    menubar->addMenu(menu);

    menubar->addAction("О программе", this, &Coma::GetAbout);

    menubar->addSeparator();

    setMenuBar(menubar);
}

void Coma::addConfTab(ETabWidget *MainTW, QString str)
{
    MainTW->addTab(corDialog, str);
    corDialog->corDIndex = MainTW->indexOf(corDialog);
    if (MainInterface == I_RS485)
        ChModbus->CorIndex = corDialog->corDIndex;
}

void Coma::StartWork()
{
    New104();
    NewModbus();
    NewUSB();
    if (!Reconnect)
    {
        QEventLoop loop;
        Cancelled = false;
        ConnectDialog *dlg = new ConnectDialog;
        connect(dlg, &ConnectDialog::Accepted, [this, dlg](ConnectDialog::ConnectStruct *st) {
            this->ConnectSettings = *st;
            emit CloseConnectDialog();
        });
        connect(dlg, &ConnectDialog::Cancelled, [this, dlg]() {
            this->Cancelled = true;
            emit CloseConnectDialog();
        });
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
            QMessageBox::critical(this, "Ошибка", "Не удалось установить связь", QMessageBox::Ok);
            QApplication::restoreOverrideCursor();
            ERMSG("cn: can't connect");
            return;
        }
        int res = ModuleBSI::SetupBSI();
        if (res == GENERALERROR)
        {
            QMessageBox::critical(this, "Ошибка", "Не удалось установить связь", QMessageBox::Ok);
            ERMSG("BSI read error");
            return;
        }
        else if (res == NOERROR)
        {
            if (ModuleBSI::ModuleTypeString != "")
                QMessageBox::information(
                    this, "Успешно", "Связь с " + ModuleBSI::ModuleTypeString + " установлена", QMessageBox::Ok);
            // EMessageBox::information(this, "Успешно", "Связь с " + ModuleBSI::ModuleTypeString + " установлена");
            else
            {
                QMessageBox::critical(this, "Ошибка", "Неизвестный тип модуля", QMessageBox::Ok);
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
            QMessageBox::critical(this, "Ошибка", "Не удалось соединиться с прибором", QMessageBox::Ok);
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

    str = (checkMDialog == nullptr) ? "Текущие параметры" : "Текущие параметры\nБазовая";
    if (checkBDialog != nullptr)
    {
        checkBDialog->setMinimumHeight(500);
        MainTW->addTab(checkBDialog, str);
        CheckIndex = MainTW->indexOf(checkBDialog);
        if (MainInterface == I_RS485)
            ChModbus->CheckIndex = CheckIndex;
    }
    str = (checkBDialog == nullptr) ? "Текущие параметры" : "Текущие параметры\nМезонин";
    if (checkMDialog != nullptr)
        MainTW->addTab(checkMDialog, str);

    if (confBDialog != nullptr)
    if (Harm != nullptr)
    {
        MainTW->addTab(Harm, "Гармоники");
        CheckHarmIndex = MainTW->indexOf(Harm);
        if (MainInterface == I_RS485)
            ChModbus->CheckHarmIndex = CheckHarmIndex;
    }

    if (ConfB != nullptr)
    {
        str = (confMDialog == nullptr) ? "Конфигурирование" : "Конфигурирование\nБазовая";
        MainTW->addTab(confBDialog, str);
        if (confBDialog->IsNeededDefConf)
        {
            confBDialog->SetDefConf();
            QMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию", QMessageBox::Ok);
            // EMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию");
        }
        connect(confBDialog, &AbstractConfDialog::NewConfToBeLoaded, this, &Coma::Fill);
        connect(confBDialog, &AbstractConfDialog::DefConfToBeLoaded, this, &Coma::SetDefConf);
    }
    if (confMDialog != nullptr)
    {
        str = (confBDialog == nullptr) ? "Конфигурирование" : "Конфигурирование\nМезонин";
        MainTW->addTab(confMDialog, str);
        ConfIndex = MainTW->indexOf(confMDialog);

        if (confMDialog->IsNeededDefConf)
        {
            confMDialog->SetDefConf();
            QMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию", QMessageBox::Ok);
        }
        connect(confMDialog, &AbstractConfDialog::NewConfToBeLoaded, this, &Coma::Fill);
        connect(confMDialog, &AbstractConfDialog::DefConfToBeLoaded, this, &Coma::SetDefConf);
    }
    if (timeDialog != nullptr)
    {
        MainTW->addTab(timeDialog, "Время");
        TimeIndex = MainTW->indexOf(timeDialog);
        ChModbus->TimeIndex = TimeIndex;
        connect(TimeTimer, &QTimer::timeout, timeDialog, &MNKTime::slot2_timeOut);
    }

    if (corDialog != nullptr)
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
        MainTW->addTab(jourDialog, "Журналы");

    if (ModuleBSI::Health() & HTH_CONFIG) // нет конфигурации
        Error::ShowErMsg(ER_NOCONF);
    if (ModuleBSI::Health() & HTH_REGPARS) // нет коэффициентов
        Error::ShowErMsg(ER_NOTUNECOEF);

    if (MainInterface == I_USB)
    {
        fwUpDialog = new fwupdialog;
        MainTW->addTab(fwUpDialog, "Загрузка ВПО");
    }

    MainTW->addTab(infoDialog, "О приборе");
    infoDialog->FillBsi();

    MainTW->repaint();
    MainTW->show();

    INFOMSG("MainTW created");
    if (MainInterface == I_USB)
        BdaTimer->start();
}

void Coma::PrepareDialogs()
{
    infoDialog = new InfoDialog(this);
    jourDialog = new JournalDialog(Ch104);
    timeDialog = new MNKTime(this);

    AlarmStateAllWidget = new AlarmStateAll(this);
    connect(AlarmW, &AlarmWidget::AlarmButtonPressed, AlarmStateAllWidget, &QWidget::show);

    switch (MTypeB)
    {
    case Config::MTB_A2:
        switch (MTypeM)
        {
        case Config::MTM_84:
            checkBDialog = new CheckDialogKIV(BoardTypes::BT_BASE);

            S2Config->clear();
            if (MainInterface != I_RS485)
                confMDialog = new ConfDialogKIV(S2Config);
            corDialog = new CorDialog;

            WarnAlarmKIVWidget = new WarnAlarmKIV(Alarm);
            connect(AlarmW, &AlarmWidget::ModuleWarnButtonPressed, WarnAlarmKIVWidget, &QWidget::show);
            connect(Alarm, &AlarmClass::SetWarnAlarmColor, WarnAlarmKIVWidget, &WarnAlarmKIV::Update);

            AvarAlarmKIVWidget = new AvarAlarmKIV(Alarm);
            connect(AlarmW, &AlarmWidget::AlarmButtonPressed, AvarAlarmKIVWidget, &QWidget::show);
            connect(Alarm, &AlarmClass::SetAlarmColor, AvarAlarmKIVWidget, &AvarAlarmKIV::Update);

            break;

        case Config::MTM_87:
            checkBDialog = new ChekDialogKTF(BoardTypes::BT_BASE);

            Harm = new CheckDialogHarmonicKTF(BoardTypes::BT_BASE);

            connect(BdaTimer, SIGNAL(timeout()), Harm, SLOT(USBUpdate()));

            S2Config->clear();
            if (MainInterface != I_RS485)
                confMDialog = new ConfDialogKTF(S2Config);
            corDialog = new CorDialogKTF;

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
            CheckB = new CheckDialogKDV(BoardTypes::BT_BASE);
            S2Config->clear();
            if (MainInterface != I_RS485)
                confMDialog = new ConfDialogKDV(S2Config);

            break;
        };

        break;
    };

    NewTimersBda();

    connect(this, &Coma::ClearBsi, infoDialog, &InfoDialog::ClearBsi);
    connect(AlarmW, SIGNAL(SetWarnAlarmColor(QList<bool>)), checkBDialog, SLOT(SetWarnAlarmColor(QList<bool>)));
    connect(AlarmW, SIGNAL(SetAlarmColor(QList<bool>)), checkBDialog, SLOT(SetAlarmColor(QList<bool>)));

    if (MainInterface == I_ETHERNET)
    {
        connect(Ch104, SIGNAL(Floatsignalsready(IEC104Thread::FlSignals104 *)), checkBDialog,
            SLOT(UpdateFlData(IEC104Thread::FlSignals104 *)));
        connect(Ch104, SIGNAL(Sponsignalsready(IEC104Thread::SponSignals *)), checkBDialog,
            SLOT(UpdateSponData(IEC104Thread::SponSignals *)));

        connect(timeDialog, &MNKTime::ethTimeRequest, Ch104, &IEC104::InterrogateTimeGr15);
        connect(Ch104, &IEC104::Bs104signalsready, timeDialog, &MNKTime::FillTimeFrom104);
        connect(timeDialog, &MNKTime::ethWriteTimeToModule, Ch104, &IEC104::com51WriteTime);

        connect(corDialog, &AbstractCorDialog::SendCom45, Ch104, &IEC104::Com45);
        connect(corDialog, &AbstractCorDialog::SendCom50, Ch104, &IEC104::Com50);
        connect(corDialog, &AbstractCorDialog::CorReadRequest, Ch104, &IEC104::CorReadRequest);
        connect(Ch104, &IEC104::SendMessageOk, corDialog, &AbstractCorDialog::MessageOk);
        connect(Ch104, &IEC104::Floatsignalsready, corDialog, &AbstractCorDialog::UpdateFlCorData);

        connect(confMDialog, &AbstractConfDialog::ReadConfig, Ch104, &IEC104::SelectFile);
        connect(Ch104, &IEC104::SendS2fromiec104, confMDialog, &AbstractConfDialog::FillConf);
        connect(confMDialog, &AbstractConfDialog::writeConfFile, Ch104, &IEC104::FileReady);
        connect(Ch104, &IEC104::SendConfMessageOk, confMDialog, &AbstractConfDialog::WriteConfMessageOk);
    }
    else if (MainInterface == I_RS485)
    {
        connect(ChModbus, SIGNAL(ModbusState(ConnectionStates)), checkBDialog,
            SLOT(onModbusStateChanged(ConnectionStates)));
        connect(ChModbus, SIGNAL(SignalsReceived(QList<ModBus::SignalStruct>)), checkBDialog,
            SLOT(UpdateModBusData(QList<ModBus::SignalStruct>)));

        connect(timeDialog, &MNKTime::modBusTimeRequest, ChModbus, &ModBus::ReadTime);
        connect(ChModbus, &ModBus::TimeSignalsReceived, timeDialog, &MNKTime::FillTimeFromModBus);
        connect(timeDialog, &MNKTime::modbusWriteTimeToModule, ChModbus, &ModBus::WriteTime);
        connect(ChModbus, &ModBus::TimeReadError, timeDialog, &MNKTime::ErrorRead);
        connect(ChModbus, &ModBus::TimeWritten, timeDialog, &MNKTime::TimeWritten);

        connect(ChModbus, &ModBus::ErrorRead, corDialog, &AbstractCorDialog::ErrorRead);
        connect(ChModbus, &ModBus::CorSignalsReceived, corDialog, &AbstractCorDialog::ModBusUpdateCorData);
        connect(ChModbus, &ModBus::CorSignalsWritten, corDialog, &AbstractCorDialog::MessageOk);
        connect(corDialog, &AbstractCorDialog::RS485WriteCorBd, ChModbus,
            &ModBus::ModWriteCor); //, int*)));
        connect(corDialog, &AbstractCorDialog::RS485ReadCorBd, ChModbus, &ModBus::ModReadCor);
    }
}

void Coma::CloseDialogs()
{
    QList<QWidget *> widgets = this->findChildren<QWidget *>("Dialog");
    // this->findChildren
    for (auto &i : widgets)
        i->close();
    //    if (TimeD != nullptr)
    //        TimeD->close();
    //    if (CheckB != nullptr)
    //        CheckB->close();
    //    if (CheckM != nullptr)
    //        CheckM->close();
    //    if (MainConfDialog != nullptr)
    //        MainConfDialog->close();
    //    if (ConfB != nullptr)
    //        ConfB->close();
    //    if (ConfM != nullptr)
    //        ConfM->close();
    //    if (Wpred != nullptr)
    //        Wpred->close();
    //    if (Walarm != nullptr)
    //        Walarm->close();
    //    if (CorD != nullptr)
    //        CorD->close();
    //    if (IDialog != nullptr)
    //        IDialog->close();
    //    if (JourD != nullptr)
    //        JourD->close();
}

void Coma::New104()
{
    Ch104 = new IEC104(S2Config);
    connect(this, &Coma::StopCommunications, Ch104, &IEC104::StopAllThreads);
    connect(Ch104, &IEC104::Finished, [this]() { ActiveThreads &= ~THREAD::P104; });
    // connect(Ch104,SIGNAL(Sponsignalsready(IEC104Thread::SponSignals*)),this,SLOT(UpdatePredAlarmEvents(IEC104Thread::SponSignals*)));
    connect(Ch104, &IEC104::SetDataSize, this, &Coma::SetProgressBar1Size);
    connect(Ch104, &IEC104::SetDataCount, this, &Coma::SetProgressBar1);
    connect(Ch104, &IEC104::ReconnectSignal, this, &Coma::ReConnect);
    connect(Ch104, SIGNAL(Sponsignalsready(IEC104Thread::SponSignals *)), this,
        SLOT(UpdateAlarm104(IEC104Thread::SponSignals *)));
    connect(Ch104, SIGNAL(Bs104signalsready(IEC104Thread::BS104Signals *)), this,
        SLOT(FillBSI(IEC104Thread::BS104Signals *)));
}

void Coma::NewModbus()
{
    ChModbus = new ModBus;
    connect(this, &Coma::StopCommunications, ChModbus, &ModBus::Stop);
    connect(ChModbus, &ModBus::Finished, [this]() { ActiveThreads &= ~THREAD::MODBUS; });
    //  connect(ChModbus,SIGNAL(CoilSignalsReady(ModBus::Coils)), this,
    //  SLOT(ModBusUpdatePredAlarmEvents(ModBus::Coils)));
    connect(ChModbus, &ModBus::ReconnectSignal, this, &Coma::ReConnect);
    connect(ChModbus, SIGNAL(CoilSignalsReady(ModBus::Coils)), Alarm, SLOT(UpdateAlarmModbus(ModBus::Coils)));
    connect(ChModbus, SIGNAL(BsiFromModbus(QList<ModBus::BSISignalStruct>, unsigned int)), this,
        SLOT(FillBSI(QList<ModBus::BSISignalStruct>, unsigned int)));
}

void Coma::NewUSB()
{
    connect(this, &Coma::StopCommunications, EUsbHid::GetInstance(), &EAbstractProtocomChannel::Disconnect);
    connect(EUsbHid::GetInstance()->workerThread(), &QThread::finished, [=]() { ActiveThreads &= ~THREAD::USB; });
    connect(EUsbHid::GetInstance(), &EAbstractProtocomChannel::SetDataSize, this, &Coma::SetProgressBar1Size);
    connect(EUsbHid::GetInstance(), &EAbstractProtocomChannel::SetDataCount, this, &Coma::SetProgressBar1);
    connect(EUsbHid::GetInstance(), &EAbstractProtocomChannel::ShowError,
        [this](const QString &msg) { QMessageBox::critical(this, "Ошибка", msg, QMessageBox::Ok); });
}

void Coma::NewTimers()
{
    TimeTimer = new QTimer;
    TimeTimer->setInterval(1000);

    BdaTimer = new QTimer;
    BdaTimer->setInterval(ANMEASINT);

    ReceiveTimer = new QTimer;
    ReceiveTimer->setInterval(ANMEASINT);
    connect(ReceiveTimer, &QTimer::timeout, this, &Coma::FileTimeOut);

    ReconnectTimer = new QTimer;
    ReconnectTimer->setInterval(INTERVAL::RECONNECT);
    ReconnectTimer->setSingleShot(true);
    connect(ReconnectTimer, &QTimer::timeout, this, &Coma::AttemptToRec);
}

void Coma::NewTimersBda()
{
    connect(BdaTimer, &QTimer::timeout, Alarm, &AlarmClass::UpdateAlarmUSB);

    if (checkBDialog != nullptr)
        connect(BdaTimer, &QTimer::timeout, checkBDialog, &EAbstractCheckDialog::USBUpdate);
}

bool Coma::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result);
    if (eventType == "windows_generic_MSG")
    {
        MSG *msg = static_cast<MSG *>(message);
        int msgType = msg->message;
        if (msgType == WM_DEVICECHANGE)
        {
            msgType = msg->wParam;
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
                // Here will be reconnection event
                auto devs = EUsbHid::GetInstance()->DevicesFound();
                if (devs.indexOf(EUsbHid::GetInstance()->deviceName()) != -1)
                    qDebug("Device arrived again");
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
            {
                qDebug("DBT_DEVICEREMOVECOMPLETE");
                // Ивенты должны происходить только если отключен подключенный раннее прибор
                if (!Disconnected)
                    DisconnectAndClear();
                // if (EUsbHid::GetInstance()->isConnected())
                QMessageBox::critical(nullptr, "Ошибка", "Связь с прибором была разорвана", QMessageBox::Ok);
                break;
            }
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
        msgBox.setText("Связь разорвана.\nПопытка переподключения");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.show();
        AttemptToRec();
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
    connect(dlg, &KeyPressDialog::Finished, this, &Coma::PasswordCheck);
    connect(this, &Coma::PasswordChecked, &PasswordLoop, &QEventLoop::quit);
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
        QMessageBox::critical(this, "Неправильно", "Пароль введён неверно", QMessageBox::Ok);
        return GENERALERROR;
    }
    return NOERROR;
}

void Coma::setConf(unsigned char typeConf)
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

void Coma::Fill()
{
    if (mainConfDialog != nullptr)
        mainConfDialog->Fill();
    if (confBDialog != nullptr)
        confBDialog->Fill();
    if (confMDialog != nullptr)
        confMDialog->Fill();
}

void Coma::FillBSI(IEC104Thread::BS104Signals *sig)
{
    unsigned int signum;
    int startadr = 0;

    memcpy(&startadr, &(sig->BS.SigAdr[0]), sizeof(sig->BS.SigAdr));
    signum = sig->SigNumber;
    INFOMSG("FillBSIe(): address=" + QString::number(startadr));

    if ((signum < sizeof(ModuleBSI::ModuleBsi)) && (startadr >= BSIREG && startadr <= BSIENDREG))
    {
        for (size_t i = 0; i < signum; ++i)
            memcpy(((quint32 *)(&ModuleBSI::ModuleBsi) + (i + startadr - 1)), (((quint32 *)(&sig->BS.SigVal) + 4 * i)),
                sizeof(sig->BS.SigVal));
        MTypeB = ModuleBSI::ModuleBsi.MTypeB;
        MTypeM = ModuleBSI::ModuleBsi.MTypeM;
    }
}

void Coma::FillBSI(QList<ModBus::BSISignalStruct> sig, unsigned int sigsize)
{
    int startadr = sig.at(0).SigAdr;

    INFOMSG("FillBSIm()");

    if ((sigsize < sizeof(ModuleBSI::ModuleBsi)) && (startadr == BSIREG))
    {
        for (size_t i = 0; i < sigsize; ++i)
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

void Coma::SetProgressBar1Size(int size)
{
    fileSize = size;
    SetProgressBarSize(1, size);
}

void Coma::SetProgressBar1(int cursize)
{
    curfileSize = cursize;
    ReceiveTimer->stop();
    ReceiveTimer->setInterval(5000);
    SetProgressBar(1, cursize);
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
        QMessageBox::information(this, "Ошибка", "Ошибка", QMessageBox::Ok);
}

void Coma::SetProgressBar2Size(int size)
{
    SetProgressBarSize(2, size);
}

void Coma::SetProgressBar2(int cursize)
{
    SetProgressBar(2, cursize);
}

void Coma::SetProgressBarSize(int prbnum, int size)
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

void Coma::SetProgressBar(int prbnum, int cursize)
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
            if (EUsbHid::GetInstance()->isConnected())
                EUsbHid::GetInstance()->Disconnect();
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
        // Проверить после отключения алармов
        // if (Reconnect)
        //    QMessageBox::information(this, "Разрыв связи", "Связь разорвана", QMessageBox::Ok, QMessageBox::Ok);
        //        else
        //            EMessageBox::information(this, "Разрыв связи", "Не удалось установить связь");
        MainTW->hide();
        StdFunc::SetEmulated(false);
    }

    Reconnect = false;
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
    if (corDialog != nullptr)
        corDialog->GetCorBd(tabindex);
    if (checkBDialog != nullptr)
    {
        if (tabindex == CheckIndex)
            BdaTimer->start();
        else
            BdaTimer->stop();
    }
    if (timeDialog != nullptr)
    if (Harm != nullptr)
    {
        if (tabindex == CheckHarmIndex)
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

    if (confMDialog != nullptr)
    {
        if (tabindex == ConfIndex)
            confMDialog->ReadConf();
    }
}

void Coma::SetDefConf()
{
    for (unsigned char i = 0x00; i != 0x03;)
        setConf(++i);
    Fill();
    QMessageBox::information(this, "Успешно", "Конфигурация по умолчанию", QMessageBox::Ok);
}

void Coma::closeEvent(QCloseEvent *event)
{
    DisconnectAndClear();
    event->accept();
}
