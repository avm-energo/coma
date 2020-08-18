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

#include "../check/checkdialogkdv.h"
#include "../check/checkdialogkiv.h"
#include "../check/checkdialogktf.h"
#include "../config/confdialogkdv.h"
#include "../config/confdialogkiv.h"
#include "../config/confdialogktf.h"
#include "../dialogs/errordialog.h"
#include "../dialogs/settingsdialog.h"
#include "../gen/colors.h"
#include "../gen/commands.h"
#include "../gen/files.h"
#include "../gen/logclass.h"
#include "../gen/modulebsi.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../modbus/modbus.h"
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
    QPixmap StartWindowSplashPixmap("images/2.1.x.png");
    QSplashScreen *splash = new QSplashScreen(StartWindowSplashPixmap);
    splash->show();
    splash->showMessage("Подготовка окружения...", Qt::AlignRight, Qt::white);
    // http://stackoverflow.com/questions/2241808/checking-if-a-folder-exists-and-creating-folders-in-qt-c
    QDir dir(StdFunc::GetHomeDir());
    if (!dir.exists())
        dir.mkpath(".");
    StdFunc::Init();
    Error::Init();
    S2Config = new QVector<S2::DataRec>;
    S2ConfigForTune.clear();
    FullName = "";
    Disconnected = true;
    Reconnect = false;
    TimeD = nullptr;
    // MainConfDialog = nullptr;
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
    QAction *act;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QToolBar *tb = new QToolBar;

    tb->setStyleSheet("QToolBar {background: 0px; margin: 0px; spacing: 5px; padding: 0px;}");
    tb->setIconSize(QSize(20, 20));
    act = new QAction(this);
    act->setToolTip("Соединение");
    act->setIcon(QIcon("images/play.png"));
    connect(act, SIGNAL(triggered()), this, SLOT(StartWork()));
    tb->addAction(act);
    act = new QAction(this);
    act->setToolTip("Разрыв соединения");
    act->setIcon(QIcon("images/stop.png"));
    connect(act, SIGNAL(triggered()), this, SLOT(DisconnectAndClear()));
    tb->addAction(act);
    tb->addSeparator();
    act = new QAction(this);
    act->setToolTip("Настройки");
    act->setIcon(QIcon("images/settings.png"));
    connect(act, SIGNAL(triggered()), this, SLOT(StartSettingsDialog()));
    tb->addAction(act);
    act = new QAction(this);
    act->setToolTip("Протокол ошибок");
    act->setIcon(QIcon("images/skull-and-bones.png"));
    connect(act, SIGNAL(triggered(bool)), this, SLOT(ShowErrorDialog()));
    tb->addAction(act);
    hlyout->addWidget(tb);

    AlarmW = new AlarmWidget(Alarm);
    hlyout->addWidget(AlarmW, Qt::AlignCenter);

    lyout->addLayout(hlyout);
    lyout->addWidget(Least());
    wdgt->setLayout(lyout);
    setCentralWidget(wdgt);
    SetupMenubar();
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
    connect(act, SIGNAL(triggered()), this, SLOT(close()));
    menu->addAction(act);
    act = new QAction(this);
    act->setText("Соединение");
    act->setIcon(QIcon("images/play.png"));
    connect(act, SIGNAL(triggered()), this, SLOT(StartWork()));
    menu->addAction(act);
    act = new QAction(this);
    act->setText("Разрыв соединения");
    act->setIcon(QIcon("images/stop.png"));
    connect(act, SIGNAL(triggered()), this, SLOT(DisconnectAndClear()));
    menu->addAction(act);
    menubar->addMenu(menu);
    menu = new QMenu;
    act = new QAction(this);
    act->setText("О программе");
    connect(act, SIGNAL(triggered()), this, SLOT(GetAbout()));
    menubar->addAction(act);

    menubar->addSeparator();

    setMenuBar(menubar);
}

void Coma::StartWork()
{
    if (!Reconnect)
    {
        ShowInterfaceDialog();
        ShowConnectDialog();
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
    connect(MainTW, SIGNAL(tabClicked(int)), this, SLOT(MainTWTabClicked(int)));

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
        ActiveThreads |= THREADUSB;
        MTypeB = ModuleBSI::GetMType(BoardTypes::BT_BASE);
        MTypeM = ModuleBSI::GetMType(BoardTypes::BT_MEZONIN);
        //        emit USBBsiRefresh();
    }
    else
    {
        if (MainInterface == I_ETHERNET)
        {
            if (!Ch104->Working())
                Ch104->Connect(IPtemp, AdrBaseStation);
            ActiveThreads |= THREAD104;
        }
        else
        {
            if (ChModbus->Connect(Settings) != NOERROR)
            {
                ERMSG("Modbus not connected");
                return;
            }
            ChModbus->BSIrequest();
            ActiveThreads |= THREADMBS;
        }
        QElapsedTimer tmr;
        tmr.start();
        while ((MTypeB == 0) && (tmr.elapsed() < WAITINTERVAL) && !Cancelled)
            QCoreApplication::processEvents();
        if (MTypeB == 0)
        {
            DisconnectAndClear();
            ERMSG("Не получили BSI");
            Disconnect();
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
        connect(ConfB, SIGNAL(NewConfToBeLoaded()), this, SLOT(Fill()));
        connect(ConfB, SIGNAL(DefConfToBeLoaded()), this, SLOT(SetDefConf()));
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
        connect(ConfM, SIGNAL(NewConfToBeLoaded()), this, SLOT(Fill()));
        connect(ConfM, SIGNAL(DefConfToBeLoaded()), this, SLOT(SetDefConf()));
    }
    if (TimeD != nullptr)
    {
        MainTW->addTab(TimeD, "Время");
        TimeIndex = MainTW->indexOf(TimeD);
        ChModbus->TimeIndex = TimeIndex;
        connect(TimeTimer, SIGNAL(timeout()), TimeD, SLOT(slot2_timeOut()));
    }

    if (CorD != nullptr)
    {
        switch (MTypeB)
        {
        case Config::MTB_A2:
            switch (MTypeM)
            {
            case Config::MTM_84:
                MainTW->addTab(CorD, "Начальные значения");
                CorD->corDIndex = MainTW->indexOf(CorD);
                if (MainInterface == I_RS485)
                    ChModbus->CorIndex = CorD->corDIndex;
                break;

            case Config::MTM_87:
                MainTW->addTab(CorD, "Текущее значение");
                CorD->corDIndex = MainTW->indexOf(CorD);
                if (MainInterface == I_RS485)
                    ChModbus->CorIndex = CorD->corDIndex;
                break;
            }
            break;
        case Config::MTB_A3:

            switch (MTypeM)
            {
            case Config::MTM_87:
                MainTW->addTab(CorD, "Начальные значения");
                CorD->corDIndex = MainTW->indexOf(CorD);
                if (MainInterface == I_RS485)
                    ChModbus->CorIndex = CorD->corDIndex;
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
    connect(AlarmW, SIGNAL(AlarmButtonPressed()), AlarmStateAllWidget, SLOT(show()));

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
            connect(AlarmW, SIGNAL(ModuleWarnButtonPressed()), WarnAlarmKIVWidget, SLOT(show()));
            connect(Alarm, SIGNAL(SetWarnAlarmColor(QList<bool>)), WarnAlarmKIVWidget, SLOT(Update(QList<bool>)));

            AvarAlarmKIVWidget = new AvarAlarmKIV(Alarm);
            connect(AlarmW, SIGNAL(AlarmButtonPressed()), AvarAlarmKIVWidget, SLOT(show()));
            connect(Alarm, SIGNAL(SetAlarmColor(QList<bool>)), AvarAlarmKIVWidget, SLOT(Update(QList<bool>)));

            break;

        case Config::MTM_87:
            CheckB = new CheckDialogKTF(BoardTypes::BT_BASE);
            S2Config->clear();
            if (MainInterface != I_RS485)
                ConfM = new ConfDialogKTF(S2Config);

            //            CheckB = new CheckDialogKDV(BoardTypes::BT_BASE);
            //            S2Config->clear();
            //            if (MainInterface != I_RS485)
            //                ConfM = new ConfDialogKDV(S2Config);

            CorD = new CorDialogKTF;

            WarnAlarmKTFWidget = new WarnAlarmKTF(Alarm);
            connect(AlarmW, SIGNAL(ModuleWarnButtonPressed()), WarnAlarmKTFWidget, SLOT(show()));
            connect(Alarm, SIGNAL(SetWarnAlarmColor(QList<bool>)), WarnAlarmKTFWidget, SLOT(Update(QList<bool>)));

            AvarAlarmKTFWidget = new AvarAlarmKTF(Alarm);
            connect(AlarmW, SIGNAL(ModuleAlarmButtonPressed()), AvarAlarmKTFWidget, SLOT(show()));
            connect(Alarm, SIGNAL(SetAlarmColor(QList<bool>)), AvarAlarmKTFWidget, SLOT(Update(QList<bool>)));

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
                ConfM = new ConfDialogKDV(S2Config);

            break;
        };

        break;
    };

    NewTimersBda();

    connect(this, SIGNAL(ClearBsi()), IDialog, SLOT(ClearBsi()));
    connect(AlarmW, SIGNAL(SetWarnAlarmColor(QList<bool>)), CheckB, SLOT(SetWarnAlarmColor(QList<bool>)));
    connect(AlarmW, SIGNAL(SetAlarmColor(QList<bool>)), CheckB, SLOT(SetAlarmColor(QList<bool>)));

    if (MainInterface == I_ETHERNET)
    {
        connect(Ch104, SIGNAL(Floatsignalsready(IEC104Thread::FlSignals104 *)), CheckB,
            SLOT(UpdateFlData(IEC104Thread::FlSignals104 *)));
        connect(Ch104, SIGNAL(Sponsignalsready(IEC104Thread::SponSignals *)), CheckB,
            SLOT(UpdateSponData(IEC104Thread::SponSignals *)));

        connect(TimeD, SIGNAL(ethTimeRequest()), Ch104, SLOT(InterrogateTimeGr15()));
        connect(Ch104, SIGNAL(Bs104signalsready(IEC104Thread::BS104Signals *)), TimeD,
            SLOT(FillTimeFrom104(IEC104Thread::BS104Signals *)));
        connect(TimeD, SIGNAL(ethWriteTimeToModule(uint)), Ch104, SLOT(com51WriteTime(uint)));

        connect(CorD, SIGNAL(SendCom45(quint32)), Ch104, SLOT(Com45(quint32)));
        connect(CorD, SIGNAL(SendCom50(quint32, float)), Ch104, SLOT(Com50(quint32, float)));
        connect(CorD, SIGNAL(CorReadRequest()), Ch104, SLOT(CorReadRequest()));
        connect(Ch104, SIGNAL(SendMessageOk()), CorD, SLOT(MessageOk()));
        connect(Ch104, SIGNAL(Floatsignalsready(IEC104Thread::FlSignals104 *)), CorD,
            SLOT(UpdateFlCorData(IEC104Thread::FlSignals104 *)));

        connect(ConfM, SIGNAL(ReadConfig(char)), Ch104, SLOT(SelectFile(char)));
        connect(Ch104, SIGNAL(SendS2fromiec104(QVector<S2::DataRec> *)), ConfM, SLOT(FillConf(QVector<S2::DataRec> *)));
        connect(ConfM, SIGNAL(writeConfFile(QVector<S2::DataRec> *)), Ch104, SLOT(FileReady(QVector<S2::DataRec> *)));
        connect(Ch104, SIGNAL(SendConfMessageOk()), ConfM, SLOT(WriteConfMessageOk()));
    }
    else if (MainInterface == I_RS485)
    {
        connect(ChModbus, SIGNAL(ModbusState(ConnectionStates)), CheckB, SLOT(onModbusStateChanged(ConnectionStates)));
        connect(ChModbus, SIGNAL(SignalsReceived(QList<ModBus::SignalStruct>)), CheckB,
            SLOT(UpdateModBusData(QList<ModBus::SignalStruct>)));

        connect(TimeD, SIGNAL(modBusTimeRequest()), ChModbus, SLOT(ReadTime()));
        connect(ChModbus, SIGNAL(TimeSignalsReceived(QList<ModBus::BSISignalStruct>)), TimeD,
            SLOT(FillTimeFromModBus(QList<ModBus::BSISignalStruct>)));
        connect(TimeD, SIGNAL(modbusWriteTimeToModule(uint)), ChModbus, SLOT(WriteTime(uint)));
        connect(ChModbus, SIGNAL(TimeReadError()), TimeD, SLOT(ErrorRead()));
        connect(ChModbus, SIGNAL(TimeWritten()), TimeD, SLOT(TimeWritten()));

        connect(ChModbus, SIGNAL(ErrorRead()), CorD, SLOT(ErrorRead()));
        connect(ChModbus, SIGNAL(CorSignalsReceived(QList<ModBus::SignalStruct>)), CorD,
            SLOT(ModBusUpdateCorData(QList<ModBus::SignalStruct>)));
        connect(ChModbus, SIGNAL(CorSignalsWritten()), CorD, SLOT(MessageOk()));
        connect(CorD, SIGNAL(RS485WriteCorBd(ModBus::Information, float *)), ChModbus,
            SLOT(ModWriteCor(ModBus::Information, float *))); //, int*)));
        connect(CorD, SIGNAL(RS485ReadCorBd(ModBus::Information)), ChModbus, SLOT(ModReadCor(ModBus::Information)));
    }
}

void Coma::CloseDialogs()
{
    if (TimeD != nullptr)
        TimeD->close();
    //        TimeD = nullptr;
    if (CheckB != nullptr)
        CheckB->close();
    if (CheckM != nullptr)
        CheckM->close();
    //    CheckB = CheckM = nullptr;
    //    if (MainConfDialog != nullptr)
    //        MainConfDialog->close();
    //    MainConfDialog = nullptr;
    if (ConfB != nullptr)
        ConfB->close();
    if (ConfM != nullptr)
        ConfM->close();
    //    ConfB = ConfM = nullptr;

    if (Wpred != nullptr)
        Wpred->close();
    if (Walarm != nullptr)
        Walarm->close();
    //    Wpred = Walarm = nullptr;

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
    connect(this, SIGNAL(StopCommunications()), ChModbus, SLOT(Stop()));
    connect(ChModbus, SIGNAL(Finished()), this, SLOT(ModBusFinished()));
    //  connect(ChModbus,SIGNAL(CoilSignalsReady(ModBus::Coils)), this,
    //  SLOT(ModBusUpdatePredAlarmEvents(ModBus::Coils)));
    connect(ChModbus, SIGNAL(ReconnectSignal()), this, SLOT(ReConnect()));
    connect(ChModbus, SIGNAL(CoilSignalsReady(ModBus::Coils)), Alarm, SLOT(UpdateAlarmModbus(ModBus::Coils)));
    connect(ChModbus, SIGNAL(BsiFromModbus(QList<ModBus::BSISignalStruct>, unsigned int)), this,
        SLOT(FillBSI(QList<ModBus::BSISignalStruct>, unsigned int)));
}

void Coma::NewUSB()
{
    cn = new EUsbHid;
    connect(this, SIGNAL(StopCommunications()), cn, SLOT(Disconnect()));
    connect(cn, SIGNAL(Finished()), this, SLOT(USBFinished()));
    connect(cn, SIGNAL(SetDataSize(int)), this, SLOT(SetProgressBar1Size(int)));
    connect(cn, SIGNAL(SetDataCount(int)), this, SLOT(SetProgressBar1(int)));
    connect(cn, SIGNAL(ShowError(QString)), this, SLOT(ShowErrorMessageBox(QString)));
    connect(cn, SIGNAL(ReconnectSignal()), this, SLOT(ReConnect()));
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
    ReconnectTimer->setInterval(RECONNECTINTERVAL);
    ReconnectTimer->setSingleShot(true);
    connect(ReconnectTimer, SIGNAL(timeout()), this, SLOT(AttemptToRec()));
}

void Coma::NewTimersBda()
{
    connect(BdaTimer, SIGNAL(timeout()), Alarm, SLOT(UpdateAlarmUSB()));

    if (CheckB != nullptr)
        connect(BdaTimer, SIGNAL(timeout()), CheckB, SLOT(USBUpdate()));
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
        QDialog *dlg = new QDialog;

        INFOMSG("Reconnect()");
        TimeTimer->stop();
        if (!Disconnected)
        {
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

        QVBoxLayout *lyout = new QVBoxLayout;
        QHBoxLayout *hlyout = new QHBoxLayout;
        QVBoxLayout *vlayout = new QVBoxLayout;
        QString tmps = QString(PROGCAPTION);
        QWidget *w = new QWidget;
        w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};"); // color:
                                                                                // rgba(220,220,220,255);
        hlyout->addWidget(WDFunc::NewLBLT(w,
                              "Связь разорвана.\nПопытка переподключения будет "
                              "выполнена через 3 секунды",
                              "", "", ""),
            1);
        vlayout->addLayout(hlyout);
        w->setLayout(vlayout);
        connect(ReconnectTimer, SIGNAL(timeout()), dlg, SLOT(close()));
        lyout->addWidget(w);
        dlg->setLayout(lyout);
        ReconnectTimer->start();
        dlg->show();
        StdFunc::Wait(RECONNECTINTERVAL);
        dlg->close();
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
    QDialog *dlg = new QDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    QString tmps = QString(PROGCAPTION);
    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};"); // color:
                                                                            // rgba(220,220,220,255);
    if (MainInterface == I_USB)
        hlyout->addWidget(WDFunc::NewLBLT(w, "Связь с " + ModuleBSI::ModuleTypeString + " установлена", "", "", ""), 1);
    else
        hlyout->addWidget(WDFunc::NewLBLT(w, "Связь с " + FullName + " установлена", "", "", ""), 1);
    vlayout->addLayout(hlyout);
    w->setLayout(vlayout);
    lyout->addWidget(w);
    dlg->setLayout(lyout);
    dlg->show();
    StdFunc::Wait(WAITINTERVAL);
    dlg->close();
}

QWidget *Coma::Least()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *inlyout = new QHBoxLayout;
    ETabWidget *MainTW = new ETabWidget;
    MainTW->setObjectName("maintw");
    MainTW->setTabPosition(QTabWidget::West);
    inlyout->addWidget(MainTW, 60);
    MainTW->hide();
    lyout->addLayout(inlyout, 90);
    lyout->addStretch(500);
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
    prb->setMinimumWidth(500);
    prb->setMaximumHeight(10);
    inlyout->addWidget(prb);
    lyout->addLayout(inlyout);
    inlyout = new QHBoxLayout;
    inlyout->addWidget(WDFunc::NewLBLT(this, "Отсчёт"));
    inlyout->addWidget(WDFunc::NewLBLT(this, "", "prb2lbl"));
    prb = new QProgressBar;
    prb->setObjectName("prb2prb");
    prb->setOrientation(Qt::Horizontal);
    prb->setMinimumWidth(500);
    prb->setMaximumHeight(10);
    inlyout->addWidget(prb);
    lyout->addLayout(inlyout);
    w->setLayout(lyout);
    return w;
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
    Ok = false;
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

    if (!Ok)
    {
        ERMSG("Пароль введён неверно");
        EMessageBox::error(this, "Неправильно", "Пароль введён неверно");
        return GENERALERROR;
    }
    return NOERROR;
}

void Coma::SetMainDefConf()
{
    //    if (MainConfDialog != nullptr)
    //        MainConfDialog->SetDefConf();
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
    //    if (MainConfDialog != nullptr)
    //        MainConfDialog->Fill();
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
        ModuleBSI::ModuleBsi.Hth = ModuleBSI::ModuleBsi.Hth;
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
        ModuleBSI::ModuleBsi.Hth = ModuleBSI::ModuleBsi.Hth;
    }
}

void Coma::PasswordCheck(QString psw)
{
    if (psw == "se/7520a")
        Ok = true;
    else
        Ok = false;
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

void Coma::ShowInterfaceDialog()
{
    QByteArray ba;
    QDialog *dlg = new QDialog(this);
    QString Str;
    Cancelled = false;
    QStringList inter;
    inter.append("USB");
    inter.append("Ethernet");
    inter.append("RS485");
    QStringListModel *tmpmodel = new QStringListModel;
    dlg->setMinimumWidth(150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("connectdlg");
    QVBoxLayout *lyout = new QVBoxLayout;

    lyout->addWidget(WDFunc::NewLBL(this, "Выберите интерфейс связи"));

    tmpmodel->setStringList(inter);
    QComboBox *portscb = new QComboBox;
    connect(portscb, SIGNAL(currentIndexChanged(QString)), this, SLOT(ParseInter(QString)));
    portscb->setModel(tmpmodel);
    lyout->addWidget(portscb);
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Далее");
    connect(pb, SIGNAL(clicked(bool)), dlg, SLOT(close()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb, SIGNAL(clicked(bool)), this, SLOT(SetCancelled()));
    connect(pb, SIGNAL(clicked(bool)), dlg, SLOT(close()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    dlg->exec();
}

void Coma::SetCancelled()
{
    Cancelled = true;
}

void Coma::ShowConnectDialog()
{
    QByteArray ba;
    int res;
    QString Str;

    if (Cancelled)
    {
        ERMSG("Отмена ConnectDialog");
        return;
    }
    QDialog *dlg = new QDialog(this);
    dlg->setMinimumWidth(150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("connectdlg");
    QVBoxLayout *lyout = new QVBoxLayout;
    QStringListModel *tmpmodel = new QStringListModel;

    if (MainInterface == I_USB)
    {
        USBsl = cn->DevicesFound();
        if (USBsl.size() == 0)
        {
            lyout->addWidget(WDFunc::NewLBL(this, "Ошибка, устройства не найдены"));
            Error::ShowErMsg(CN_NOPORTSERROR);
        }
        tmpmodel->deleteLater();
        tmpmodel->setStringList(USBsl);
        QComboBox *portscb = new QComboBox;
        connect(portscb, SIGNAL(currentIndexChanged(QString)), this, SLOT(SetPortSlot(QString)));
        portscb->setModel(tmpmodel);
        lyout->addWidget(portscb);
    }
    else
    {
        if (!HaveAlreadyRed)
        {
            sl.clear();
            res = Files::LoadFromFile(Files::ChooseFileForOpen(this, "IP files (*.txt)"), ba);
            if (res != Files::ER_NOERROR)
            {
                WARNMSG("Ошибка при загрузке файла");
                return;
            }
            Str = ba;
            sl.append(Str.split("\r\n"));
            HaveAlreadyRed = 1;
        }
        if (sl.size() == 0)
        {
            lyout->addWidget(WDFunc::NewLBL(this, "Ошибка, устройства не найдены"));
            Error::ShowErMsg(CN_NOPORTSERROR);
            ERMSG("Ошибка, устройства не найдены");
        }
        tmpmodel = new QStringListModel;
        tmpmodel->setStringList(sl);

        QComboBox *portscb = new QComboBox;
        connect(portscb, SIGNAL(currentIndexChanged(QString)), this, SLOT(ParseString(QString)));
        portscb->setModel(tmpmodel);
        lyout->addWidget(portscb);
    }
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Далее");
    connect(pb, SIGNAL(clicked(bool)), dlg, SLOT(close()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb, SIGNAL(clicked(bool)), this, SLOT(SetCancelled()));
    connect(pb, SIGNAL(clicked(bool)), dlg, SLOT(close()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    dlg->exec();
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
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *l2yout = new QVBoxLayout;
    QString tmps = QString(PROGCAPTION);

    l2yout->addWidget(WDFunc::NewLBL(this, tmps));
    l2yout->addWidget(WDFunc::NewLBL(this, "ООО \"АВМ-Энерго\""));
    l2yout->addWidget(WDFunc::NewLBL(this, "2015-2020 гг."));
    l2yout->addWidget(WDFunc::NewLBL(this, "info@avmenergo.ru"));
    l2yout->addStretch(10);
    hlyout->addWidget(WDFunc::NewLBL(this, "", "", "", new QPixmap("images/avm-energo.png")), 1, Qt::AlignVCenter);
    hlyout->addLayout(l2yout, 100);
    lyout->addLayout(hlyout, 1);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb, SIGNAL(clicked()), dlg, SLOT(close()));
    lyout->addWidget(pb, 0);
    dlg->setLayout(lyout);
    dlg->show();
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
        }
        while (ActiveThreads) // wait for all threads to finish
            QCoreApplication::processEvents();
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

void Coma::ParseString(QString Str)
{
    QStringList sl = Str.split(" ");

    if (sl.size() < 3)
    {
        EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
        ERMSG("Некорректная запись в файле");
        return;
    }
    if ((sl.at(1) == "ETH") && (MainInterface == I_ETHERNET))
    {
        FullName = sl.at(0);
        AdrBaseStation = sl.at(2).toUShort();
        IPtemp = Str.split(" ").last();
    }
    else if ((sl.at(1) == "MODBUS") && (MainInterface == I_RS485))
    {
        if (sl.size() > 6)
        {
            bool ok;
            FullName = sl.at(0);
            Settings.Baud = sl.at(2).toUInt(&ok);
            if (!ok)
            {
                EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
                ERMSG("Некорректная запись в файле");
                return;
            }
            Settings.Parity = sl.at(3);
            Settings.Stop = sl.at(4);
            Settings.Address = sl.at(5).toUInt(&ok);
            if (!ok)
            {
                ERMSG("Некорректная запись в файле");
                EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
                return;
            }
            Settings.Port = sl.at(6);
        }
        else
        {
            EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
        }
    }
}

void Coma::ParseInter(QString str)
{
    if (str == "USB")
        MainInterface = I_USB;
    else if (str == "Ethernet")
        MainInterface = I_ETHERNET;
    else if (str == "RS485")
        MainInterface = I_RS485;
    else
        MainInterface = I_UNKNOWN;
}

void Coma::MainTWTabClicked(int tabindex)
{
    if (tabindex == CurTabIndex) // to prevent double function invocation by
                                 // doubleclicking on tab
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

void Coma::ModBusFinished()
{
    ActiveThreads &= ~THREADMBS;
}

void Coma::Ch104Finished()
{
    ActiveThreads &= ~THREAD104;
}

void Coma::USBFinished()
{
    ActiveThreads &= ~THREADUSB;
}

void Coma::SetPortSlot(QString port)
{
    SavePort = port;
    cn->TranslateDeviceAndSave(port);
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
