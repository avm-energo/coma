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

#include <QLabel>
#include <QProgressBar>
#include <QDir>
#include <QMenu>
#include <QMenuBar>
#include <QTextEdit>
#include <QToolBar>
#include <QDialog>
#include <QCursor>
#include <QSplashScreen>
#include <QApplication>
#include <QThread>
#include "coma.h"
#include "../config/confdialog84.h"
#include "../check/checkdialog84.h"
#include "../dialogs/fwupdialog.h"
#include "../dialogs/settingsdialog.h"
#include "../dialogs/errordialog.h"
#include "../widgets/etabwidget.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "../widgets/waitwidget.h"
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "../gen/logclass.h"
#include "../gen/stdfunc.h"
#include "../modbus/modbus.h"
#include "../gen/timefunc.h"
#include "../gen/commands.h"

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
    S2Config.clear();
    S2ConfigForTune.clear();
    FullName = "";
    Reconnect = false;
    for (int i = 0; i < 20; ++i)
    {
       PredAlarmEvents[i] = 0;
       AlarmEvents[i] = 0;
    }
    ActiveThreads = 0;
    TheEnd = 0;

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
    QString tmps = "QComa {background-color: "+QString(MAINWINCLR)+";}";
    setStyleSheet(tmps);
    setMinimumSize(QSize(800, 700));
    QWidget *wdgt = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QAction *act;

    QHBoxLayout *hlyout = new QHBoxLayout;
    QToolBar *tb = new QToolBar;
    tb->setStyleSheet("QToolBar {background: 0px; margin: 0px; spacing: 5px; padding: 0px;}");
    tb->setIconSize(QSize(20,20));
    act = new QAction(this);
    act->setToolTip("Соединение");
    act->setIcon(QIcon("images/play.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(StartWork()));
    tb->addAction(act);
    act = new QAction(this);
    act->setToolTip("Разрыв соединения");
    act->setIcon(QIcon("images/stop.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(DisconnectAndClear()));
    tb->addAction(act);
    tb->addSeparator();
    act = new QAction(this);
    act->setToolTip("Настройки");
    act->setIcon(QIcon("images/settings.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(StartSettingsDialog()));
    tb->addAction(act);
    act = new QAction(this);
    act->setToolTip("Протокол ошибок");
    act->setIcon(QIcon("images/skull-and-bones.png"));
    connect(act,SIGNAL(triggered(bool)),this,SLOT(ShowErrorDialog()));
    tb->addAction(act);
    hlyout->addWidget(tb); 
    hlyout->addWidget(ReleWidget(), Qt::AlignCenter);
    //hlyout->addWidget(HthWidget());
    lyout->addLayout(hlyout);
    lyout->addWidget(Least());
    wdgt->setLayout(lyout);
    setCentralWidget(wdgt);
    SetupMenubar();
}

void Coma::SetupMenubar()
{
    QMenuBar *menubar = new QMenuBar;
    QString tmps = "QMenuBar {background-color: "+QString(MAINWINCLRA1)+";}"\
            "QMenuBar::item {background-color: "+QString(MAINWINCLRA1)+";}";
    menubar->setStyleSheet(tmps);
    QMenu *menu = new QMenu;
    menu->setTitle("Главное");
    QAction *act = new QAction(this);
    act->setText("Выход");
    connect(act,SIGNAL(triggered()),this,SLOT(close()));
    menu->addAction(act);
    act = new QAction(this);
    act->setText("Соединение");
    act->setIcon(QIcon("images/play.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(StartWork()));
    menu->addAction(act);
    act = new QAction(this);
    act->setText("Разрыв соединения");
    act->setIcon(QIcon("images/stop.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(DisconnectAndClear()));
    menu->addAction(act);
    menubar->addMenu(menu);
   /* menu = new QMenu;
    menu->setTitle("Секретные операции");
    act = new QAction(this);
    act->setText("Работа с Hidden Block");
    connect(act,SIGNAL(triggered()),this,SLOT(OpenBhbDialog()));
    menu->addAction(act);
    menubar->addMenu(menu);*/

    menu = new QMenu;
    /*menu->setTitle("Настройки");
    act = new QAction(this);
    act->setText("Настройки");
    act->setIcon(QIcon("images/settings.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(StartSettingsDialog()));
    menu->addAction(act);
    menubar->addMenu(menu);*/

    act = new QAction(this);
    act->setText("О программе");
    connect(act,SIGNAL(triggered()),this,SLOT(GetAbout()));
    menubar->addAction(act);

    menubar->addSeparator();

    setMenuBar(menubar);
    Disconnected = 0;
}

void Coma::StartWork()
{
    if (!Reconnect)
    {
        Disconnected = 0;
        ShowInterfaceDialog();
        ShowConnectDialog();
        if(Cancelled)
        {
            ERMSG("Отмена подключения");
            return;
        }
        StopRead = 0;
        S2ConfigForTune.clear();
        S2Config.clear();
        SaveSettings();
    }

    // Stage3

    DisconnectAndClear();
    TheEnd = 0;
    QString str;
    MTypeB = 0;
    MTypeM = 0;
    MainConfDialog = nullptr;
    ConfB = ConfM = nullptr;
    CheckB = CheckM = nullptr;
    Wpred = Walarm = nullptr;
    ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
    if (MainTW == nullptr)
    {
        ERMSG("MainTW is empty");
        return;
    }
    connect(MainTW,SIGNAL(tabClicked(int)),this,SLOT(MainTWTabClicked(int)));

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
            if(ModuleBSI::ModuleTypeString != "")
                EMessageBox::information(this, "Успешно", "Связь с "+ModuleBSI::ModuleTypeString+" установлена");
            else
            {
                EMessageBox::error(this, "Ошибка", "Неизвестный тип модуля");
                ERMSG("Unknown module type");
                return;
            }
        }
        /*else if (res == Error::ER_RESEMPTY)
        {
            if (OpenBhbDialog() != NOERROR)
            {
                EMessageBox::error(this, "Ошибка", "Ошибка при работе с Hidden block");
                return;
            }
        }*/
        ActiveThreads |= THREADUSB;
        MTypeB = ModuleBSI::GetMType(BoardTypes::BT_BASE);
        MTypeM = ModuleBSI::GetMType(BoardTypes::BT_MEZONIN);
        emit USBBsiRefresh();
    }
    else
    {
        if(MainInterface == I_ETHERNET)
        {
            if (!Ch104->Working())
                Ch104->Connect(IPtemp);
            ActiveThreads |= THREAD104;
        }
        else
        {
            if (ChModbus->Connect(Settings) != NOERROR)
            {
                ERMSG("Modbus not connected");
                return;
            }
            ActiveThreads |= THREADMBS;
        }
        while ((MTypeB == 0) && !Cancelled)
        {
            QElapsedTimer tmr;
            tmr.start();
            while (tmr.elapsed() < 5000)
                QCoreApplication::processEvents();
            if(Reconnect)
                ReConnect();
            else
                DisconnectAndClear();
            ERMSG("Не получили BSI");
            return;
        }
    }
    // MTypeB & MTypeM are acquired
    MTypeB <<= 8;

    Reconnect = true;
    PrepareDialogs();
    str = (CheckM == nullptr) ? "Текущие параметры" : "Текущие параметры\nБазовая";
    if (CheckB != nullptr)
    {
        CheckB->setMinimumHeight(500);
        MainTW->addTab(CheckB, str);
        CheckIndex = MainTW->indexOf(CheckB);

        if(MainInterface == I_RS485)
        ChModbus->CheckIndex = CheckIndex;

        if(MainInterface == I_USB)
        {
//            connect(MainTW, SIGNAL(tabClicked(int)), this,SLOT(Start_BdaTimer(int))); //tabClicked
//            connect(MainTW, SIGNAL(tabClicked(int)), this,SLOT(Stop_BdaTimer(int)));
//            connect(ConfM, SIGNAL(StopRead(int)), BdaTimer,SLOT(stop()));
//            ConfM->checkIndex = CheckIndex;
//            connect(BdaTimer,SIGNAL(timeout()),CheckB,SLOT(BdTimerTimeout()));
            connect(BdaTimer,SIGNAL(timeout()),this,SLOT(UpdateUSBAlarm()));
        }
    }
    str = (CheckB == nullptr) ? "Текущие параметры" : "Текущие параметры\nМезонин";
    if (CheckM != nullptr)
        MainTW->addTab(CheckM, str);

    if (ConfB != nullptr)
    {
        str = (ConfM == nullptr) ? "Конфигурирование" : "Конфигурирование\nБазовая";
        MainTW->addTab(ConfB, str);
        if(ConfB->IsNeededDefConf)
        {
            ConfB->SetDefConf();
            ConfB->Fill();
            EMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию");

        }
        connect(ConfB,SIGNAL(NewConfToBeLoaded()),this,SLOT(Fill()));
        connect(ConfB,SIGNAL(DefConfToBeLoaded()),this,SLOT(SetDefConf()));
        //AbstractConfDialog::PrereadConf();
    }
    if (ConfM != nullptr)
    {
        str = (ConfB == nullptr) ? "Конфигурирование" : "Конфигурирование\nМезонин";
        MainTW->addTab(ConfM, str);

        if(ConfM->IsNeededDefConf)
        {
            ConfM->SetDefConf();
            ConfM->Fill();
            EMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию");

        }

        connect(ConfM,SIGNAL(NewConfToBeLoaded()),this,SLOT(Fill()));
        connect(ConfM,SIGNAL(DefConfToBeLoaded()),this,SLOT(SetDefConf()));
//        ConfM->confIndex = MainTW->indexOf(ConfM);
        connect(MainTW, SIGNAL(tabClicked(int)), ConfM, SLOT(ReadConf(int))); //tabClicked
//        connect(ConfM, SIGNAL(StopRead(int)), this,SLOT(Stop_TimeTimer(int)));
    }

        Time = new MNKTime();
//        connect(MainTW, SIGNAL(tabClicked(int)), Time,SLOT(Start_Timer(int))); //tabClicked
        //connect(MainTW, SIGNAL(tabClicked(int)), Time,SLOT(Stop_Timer(int)));
//        connect(MainTW, SIGNAL(tabClicked(int)), this,SLOT(Start_TimeTimer(int))); //tabClicked
//        connect(MainTW, SIGNAL(tabClicked(int)), this,SLOT(Stop_TimeTimer(int)));
        MainTW->addTab(Time, "Время");
        Time->timeIndex = MainTW->indexOf(Time);
        if(ConfM != nullptr)

        if(MainInterface == I_RS485)
        ChModbus->TimeIndex = Time->timeIndex;

        connect(TimeTimer,SIGNAL(timeout()),Time,SLOT(slot2_timeOut()));
        if(Ch104 != nullptr)
        {
            connect(Time,SIGNAL(ethTimeRequest()),Ch104,SLOT(InterrogateTimeGr15()));
            connect(Ch104,SIGNAL(bs104signalsready(Parse104::BS104Signals*)),Time,SLOT(FillTimeFrom104(Parse104::BS104Signals*)));
            connect(Time,SIGNAL(ethWriteTimeToModule(uint)),Ch104,SLOT(com51WriteTime(uint)));
        }
        if(ChModbus != nullptr)
        {
            connect(Time,SIGNAL(modBusTimeRequest()),ChModbus,SLOT(ReadTime()));
            connect(ChModbus,SIGNAL(TimeSignalsReceived(QList<ModBus::BSISignalStruct>)),Time,SLOT(FillTimeFromModBus(QList<ModBus::BSISignalStruct>)));
            connect(Time,SIGNAL(modbusWriteTimeToModule(uint)),ChModbus,SLOT(WriteTime(uint)));
            connect(ChModbus,SIGNAL(TimeReadError()),Time,SLOT(ErrorRead()));
            connect(ChModbus, SIGNAL(TimeWritten()), Time, SLOT(TimeWritten()));
        }

    if (CorD != nullptr)
    {
        MainTW->addTab(CorD, "Начальные значения");
        CorD->corDIndex = MainTW->indexOf(CorD);
        connect(MainTW, SIGNAL(tabClicked(int)), CorD, SLOT(GetCorBd(int))); //tabClicked

        if(MainInterface == I_RS485)
        ChModbus->CorIndex = CorD->corDIndex;
    }

    if (MainInterface != I_RS485)
        MainTW->addTab(JourD, "Журналы");

    if (ModuleBSI::Health() & HTH_CONFIG) // нет конфигурации
        Error::ShowErMsg(ER_NOCONF);
    if (ModuleBSI::Health() & HTH_REGPARS) // нет коэффициентов
        Error::ShowErMsg(ER_NOTUNECOEF);

    if(MainInterface == I_USB)
    {
        FwUpD = new fwupdialog;
        MainTW->addTab(FwUpD, "Загрузка ВПО");
    }

    MainTW->addTab(IDialog, "О приборе");

    MainTW->repaint();
    MainTW->show();

    if (MainInterface == I_ETHERNET || MainInterface == I_RS485)
        ConnectMessage();

    if(MainInterface == I_USB)
        BdaTimer->start();
}

void Coma::PrepareDialogs()
{
    IDialog = new InfoDialog;
    JourD = new JournalDialog;
    CheckB = new CheckDialog84(BoardTypes::BT_BASE);

    switch(MTypeB)
    {
    case Config::MTB_A2:
        break;
    };
    switch(MTypeM)
    {
    case Config::MTM_84:
        break;
    };

    connect(this,SIGNAL(ClearBsi()),IDialog,SLOT(ClearBsi()));
    connect(this,SIGNAL(USBBsiRefresh()),IDialog,SLOT(FillBsi()));
    connect(this, SIGNAL(SetPredAlarmColor(quint8*)), CheckB,SLOT(SetPredAlarmColor(quint8*)));
    connect(this, SIGNAL(SetAlarmColor(quint8*)), CheckB,SLOT(SetAlarmColor(quint8*)));
    if (MainInterface == I_ETHERNET)
    {
        Ch104->BaseAdr = AdrBaseStation;
        connect(Ch104,SIGNAL(bs104signalsready(Parse104::BS104Signals*)),IDialog,SLOT(FillBsiFrom104(Parse104::BS104Signals*)));
        connect(Ch104,SIGNAL(floatsignalsready(Parse104::FlSignals104*)),CheckB,SLOT(UpdateFlData(Parse104::FlSignals104*)));
        connect(Ch104,SIGNAL(sponsignalsready(Parse104::SponSignals104*)),CheckB,SLOT(UpdateSponData(Parse104::SponSignals104*)));
        connect(Ch104,SIGNAL(sponsignalWithTimereceived(Parse104::SponSignalsWithTime*)),CheckB,SLOT(UpdateSponDataWithTime(Parse104::SponSignalsWithTime*)));
        connect(Ch104,SIGNAL(bs104signalsready(Parse104::BS104Signals*)),CheckB,SLOT(UpdateBS104Data(Parse104::BS104Signals*)));
    }
    else if (MainInterface == I_RS485)
    {

        connect(ChModbus,SIGNAL(BsiFromModbus(QList<ModBus::BSISignalStruct>, int)),IDialog,SLOT(FillBsiFromModBus(QList<ModBus::BSISignalStruct>, int)));
        connect(ChModbus,SIGNAL(CoilSignalsReady(ModBus::Coils)),this,SLOT(ModbusUpdateStatePredAlarmEvents(ModBus::Coils)));
        ChModbus->BSIrequest();
    }

    if (MTypeB == Config::MTB_A2)
    {
        if(MainInterface == I_ETHERNET)
        {
            CorD = new CorDialog;
            if (!Ch104->Working())
                Ch104->Connect(IPtemp);
            connect(Ch104,SIGNAL(EthConnected()), this, SLOT(ConnectMessage()));
            //connect(ch104,SIGNAL(readConf()), ch104,SIGNAL(readConf()));
            connect(Ch104,SIGNAL(ethdisconnected()), this, SLOT(DisconnectMessage()));
            //connect(ch104,SIGNAL(ethNoconnection()), this, SLOT(DisconnectAndClear()));

            connect(CorD,SIGNAL(sendCom45(quint32)), Ch104, SLOT(Com45(quint32)));
            connect(CorD,SIGNAL(sendCom50(quint32*, float*)), Ch104, SLOT(Com50(quint32*,float*)));
            connect(CorD,SIGNAL(CorReadRequest()), Ch104, SLOT(CorReadRequest()));

            connect(Ch104,SIGNAL(sendMessageOk()), CorD, SLOT(MessageOk()));
            connect(Ch104,SIGNAL(sendCorMesOk()), CorD, SLOT(WriteCorMessageOk()));

            connect(Ch104,SIGNAL(relesignalsready(Parse104::SponSignals104*)), this, SLOT(UpdateReleWidget(Parse104::SponSignals104*)));

            connect(Ch104,SIGNAL(floatsignalsready(Parse104::FlSignals104*)),CorD,SLOT(UpdateFlCorData(Parse104::FlSignals104*)));

            connect(Ch104,SIGNAL(SetDataSize(int)),this,SLOT(SetProgressBar1Size(int)));
            connect(Ch104,SIGNAL(SetDataCount(int)),this,SLOT(SetProgressBar1(int)));

            connect(Ch104,SIGNAL(sponsignalsready(Parse104::SponSignals104*)),this,SLOT(UpdateStatePredAlarmEvents(Parse104::SponSignals104*)));
            connect(Ch104,SIGNAL(sponsignalWithTimereceived(Parse104::SponSignalsWithTime*)),this,SLOT(UpdateStatePredAlarmEventsWithTime(Parse104::SponSignalsWithTime*)));

            setMinimumSize(QSize(800, 650));
            //CheckB = new CheckDialog84(BoardTypes::BT_BASE, this, ch104);
            connect(CheckB,SIGNAL(BsiRefresh(ModuleBSI::Bsi*)),this,SIGNAL(BsiRefresh(ModuleBSI::Bsi*)));

            //ConfM = new ConfDialog84(S2Config);
            connect(JourD,SIGNAL(ReadJour(char)), Ch104, SLOT(SelectFile(char)));
            connect(Ch104,SIGNAL(sendJourSysfromiec104(QByteArray)), JourD, SLOT(FillSysJour(QByteArray)));
            connect(Ch104,SIGNAL(sendJourWorkfromiec104(QByteArray)), JourD, SLOT(FillWorkJour(QByteArray)));
            connect(Ch104,SIGNAL(sendJourMeasfromiec104(QByteArray)), JourD, SLOT(FillMeasJour(QByteArray)));
            Ch104->Parse->DR = &S2Config;
            connect(Ch104,SIGNAL(errorCh104()), this, SLOT(ReConnect()));
        }
        else if(MainInterface == I_RS485)
        {
            connect(ChModbus, SIGNAL(ErrorRead()), CorD, SLOT(ErrorRead()));
            connect(ChModbus, SIGNAL(ModbusState(ConnectionStates)), CheckB, SLOT(onModbusStateChanged(ConnectionStates)));
            connect(ChModbus, SIGNAL(SignalsReceived(QList<ModBus::SignalStruct>)), CheckB, SLOT(UpdateModBusData(QList<ModBus::SignalStruct>)));
            connect(ChModbus, SIGNAL(CorSignalsReceived(QList<ModBus::SignalStruct>)), CorD, SLOT(ModBusUpdateCorData(QList<ModBus::SignalStruct>)));
            connect(ChModbus, SIGNAL(CorSignalsWritten()), CorD, SLOT(ModbusCorDataWritten()));
            connect(CorD, SIGNAL(RS485WriteCorBd(ModBus::Information, float *)), ChModbus, SLOT(ModWriteCor(ModBus::Information, float *)));//, int*)));
            connect(CorD, SIGNAL(RS485ReadCorBd(ModBus::Information)), ChModbus, SLOT(ModReadCor(ModBus::Information)));
            connect(ChModbus,SIGNAL(ReconnectSignal()), this, SLOT(ReConnect()));
        }
    }

    if(MainInterface == I_USB)
    {
       MTypeM = ModuleBSI::GetMType(BoardTypes::BT_MEZONIN);
    }

    if (MTypeM == Config::MTM_84)
    {
        setMinimumSize(QSize(800, 650));

        if(MainInterface != I_RS485)
            ConfM = new ConfDialog84(S2Config);

        if(Ch104 != nullptr && MainInterface == I_ETHERNET)
        {
            connect(ConfM,SIGNAL(ReadConfig(char)), Ch104, SLOT(SelectFile(char)));
            connect(Ch104,SIGNAL(sendS2fromiec104(QVector<S2::DataRec>*)), ConfM, SLOT(FillConf(QVector<S2::DataRec>*)));
            connect(ConfM,SIGNAL(writeConfFile(QVector<S2::DataRec>*)), Ch104, SLOT(FileReady(QVector<S2::DataRec>*)));
            //connect(ch104,SIGNAL(sendMessageiec104()), ConfM, SLOT(Message()));
            connect(Ch104,SIGNAL(sendConfMessageOk()), ConfM, SLOT(WriteConfMessageOk()));
        }
    }
}

void Coma::New104()
{
    Ch104 = new IEC104;
    connect(this,SIGNAL(StopCommunications()),Ch104,SLOT(Stop()));
    connect(Ch104,SIGNAL(Finished()),this,SLOT(Ch104Finished()));
    connect(Ch104,SIGNAL(sponsignalWithTimereceived(Parse104::SponSignalsWithTime*)), this, SLOT(UpdatePredAlarmEvents(Parse104::SponSignalsWithTime*)));
}

void Coma::NewModbus()
{
    ChModbus = new ModBus;
    connect(this,SIGNAL(StopCommunications()),ChModbus,SLOT(Stop()));
    connect(ChModbus,SIGNAL(Finished()),this,SLOT(ModBusFinished()));
    connect(ChModbus,SIGNAL(CoilSignalsReady(ModBus::Coils)), this, SLOT(ModBusUpdatePredAlarmEvents(ModBus::Coils)));
}

void Coma::NewUSB()
{
    cn = new EUsbHid;
    connect(this, SIGNAL(StopCommunications()), cn, SLOT(Disconnect()));
    connect(cn,SIGNAL(Finished()),this,SLOT(USBFinished()));
    connect(cn,SIGNAL(SetDataSize(int)),this,SLOT(SetProgressBar1Size(int)));
    connect(cn,SIGNAL(SetDataCount(int)),this,SLOT(SetProgressBar1(int)));
    connect(cn, SIGNAL(ShowError(QString)), this, SLOT(ShowErrorMessageBox(QString)));
    connect(cn,SIGNAL(ReconnectSignal()),this,SLOT(ReConnect()));
}

void Coma::NewTimers()
{
    TimeTimer = new QTimer;
    TimeTimer->setInterval(1000);

    BdaTimer = new QTimer;
    BdaTimer->setInterval(ANMEASINT);
    connect(BdaTimer,SIGNAL(timeout()), this, SLOT(USBSetAlarms()));

    ReceiveTimer = new QTimer;
    ReceiveTimer->setInterval(ANMEASINT);
    connect(ReceiveTimer, SIGNAL(timeout()), this,SLOT(FileTimeOut()));

    ReconnectTimer = new QTimer;
    ReconnectTimer->setInterval(RECONNECTINTERVAL);
    ReconnectTimer->setSingleShot(true);
    connect(ReconnectTimer,SIGNAL(timeout()), this, SLOT(AttemptToRec()));
}

void Coma::SetMode(int mode)
{
    Mode = mode;
}

void Coma::Go(const QString &parameter)
{
    Q_UNUSED(parameter)
    if (Mode != COMA_GENERALMODE)
    {
        StdFunc::SetEmulated(true);
    }
    SetupUI();
    show();
}

void Coma::ReConnect()
{
    QDialog *dlg = new QDialog;

    Reconnect = true;

    if(TimeTimer != nullptr)
    {
      TimeTimer->stop();
    }

    if(Time->timer != nullptr)
    {
      Time->timer->stop();
    }


    TheEnd = 1;
    if(!Disconnected)
    {
        StopRead = 1;
        //disconnected = 1;
        Disconnect();

        CheckB = CheckM = nullptr;
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
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");  // color: rgba(220,220,220,255);
    hlyout->addWidget(WDFunc::NewLBLT(w, "Связь разорвана.\nПопытка переподключения будет выполнена через 3 секунды", "", "", ""), 1);
    vlayout->addLayout(hlyout);

    w->setLayout(vlayout);
    connect(ReconnectTimer,SIGNAL(timeout()), dlg,SLOT(close()));

    //hlyout->addLayout(l2yout,100);
    lyout->addWidget(w);
    dlg->setLayout(lyout);

    ReconnectTimer->start();
    dlg->show();
    StdFunc::Wait(RECONNECTINTERVAL);
    dlg->close();
}

void Coma::AttemptToRec()
{
    if(MainInterface == I_USB)
    {
       Disconnected = 0;
       SetPortSlot(SavePort);
       if(Cancelled)
       {
           ERMSG("Отмена");
           return;
       }
       StopRead = 0;
       if(cn->Cancelled)
       {
           ERMSG("Отмена");
           return;
       }
       if (Commands::Connect() != NOERROR) // cn->Connect()
       {
           ReConnect();
           ERMSG("Реконект");
           return;
       }
       int res = ModuleBSI::SetupBSI();
       if (res == GENERALERROR)
       {
           ReConnect();
           return;
       }
       else if (res == NOERROR)
       {
         if(ModuleBSI::ModuleTypeString != "")
         ConnectMessage();
       }
    }

    if(Reconnect != false)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        S2Config.clear();
        SaveSettings();
        QApplication::restoreOverrideCursor();
        StopRead = 0;
        StartWork();
    }
}

void Coma::ConnectMessage()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    QString tmps = QString(PROGCAPTION);
    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");  // color: rgba(220,220,220,255);
    if(MainInterface == I_USB)
        hlyout->addWidget(WDFunc::NewLBLT(w, "Связь с "+ModuleBSI::ModuleTypeString+" установлена", "", "", ""), 1);
    else
        hlyout->addWidget(WDFunc::NewLBLT(w, "Связь с "+FullName+" установлена", "", "", ""), 1);
    vlayout->addLayout(hlyout);
    w->setLayout(vlayout);
    lyout->addWidget(w);
    dlg->setLayout(lyout);
    dlg->show();
    StdFunc::Wait(WAITINTERVAL);
    dlg->close();
}

/*void Coma::stopTimer()
{
    connectTimer->stop();
    connectTimer->deleteLater();
}*/

QWidget *Coma::HthWidget()
{
    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");
    QHBoxLayout *hlyout = new QHBoxLayout;
    for (int i = (MAXERRORFLAGNUM-1); i >= 0; i--)
        hlyout->addWidget(WDFunc::NewLBLT(w, Hth().at(i), "hth"+QString::number(i), \
                                          "QLabel {background-color: rgba(255,50,50,0); color: rgba(220,220,220,255);" \
                                          "background: 0px; margin: 0px; spacing: 0; padding: 0px;}", HthToolTip().at(i)));
    w->setLayout(hlyout);
    connect(this,SIGNAL(BsiRefresh(ModuleBSI::Bsi*)), this, SLOT(UpdateHthWidget(ModuleBSI::Bsi*)));
    return w;
}

QWidget *Coma::ReleWidget()
{

    QMenu *menu = new QMenu;
    QString tmps = "QMenuBar {background-color: "+QString(MAINWINCLR)+";}"\
            "QMenuBar::item {background-color: "+QString(MAINWINCLR)+";}";
    menu->setStyleSheet(tmps);
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QHBoxLayout *hlyout2 = new QHBoxLayout;
    QWidget *w = new QWidget();
    QStringList Discription =  QStringList() << "Состояние устройства" << "Предупредительная сигнализация" << "Аварийная сигнализация";
    w->setStyleSheet("QComa {background-color: "+QString(MAINWINCLR)+";}");
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QLabel lab;

        QPushButton *pb = new QPushButton("Состояние устройства");
        pb->setMinimumSize(QSize(230,30));
        connect(pb,SIGNAL(clicked()),this,SLOT(DeviceState()));
        QGroupBox *gb = new QGroupBox("");

        //setMenuBar(menubar);
        //hlyout->addWidget(WDFunc::NewLBLT(w, "                 Реле №"+ QString::number(i+1) +": ", "", "", Discription.at(i)));
        hlyout->addWidget(pb,Qt::AlignRight);
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(950), pmgrn), 1);
        gb->setLayout(hlyout);
        hlyout2->addWidget(gb);
        /*if ((i>0)&&!((i+1)%8))
        {
            vlyout->addLayout(hlyout);
            hlyout = new QHBoxLayout;
        }*/
    //}
        gb = new QGroupBox("");
        hlyout = new QHBoxLayout;
        pb = new QPushButton("Предупредительная сигнализация");
        pb->setMinimumSize(QSize(230,30));
        connect(pb,SIGNAL(clicked()),this,SLOT(PredAlarmState()));

        hlyout->addWidget(pb,Qt::AlignRight);
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(951), pmgrn), 1);
        gb->setLayout(hlyout);
        hlyout2->addWidget(gb);

        menu = new QMenu;
        gb = new QGroupBox("");
        hlyout = new QHBoxLayout;
        pb = new QPushButton("Аварийная сигнализация");
        pb->setMinimumSize(QSize(230,30));
        connect(pb,SIGNAL(clicked()),this,SLOT(AlarmState()));

        hlyout->addWidget(pb,Qt::AlignRight);
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(952), pmgrn), 1);
        gb->setLayout(hlyout);
        hlyout2->addWidget(gb);



    if (hlyout2->count())
        vlyout->addLayout(hlyout2);
    w->setLayout(vlyout);
    return w;
}

void Coma::DeviceState()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    QString tmps = QString(PROGCAPTION);

    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    //QPixmap *pm[2] = {pmred, pmgrn};                        ";


    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");  // color: rgba(220,220,220,255);

    for (int i = 0; i < HthToolTip().size(); ++i)
    {
        hlyout = new QHBoxLayout;

        if(ModuleBSI::ModuleBsi.Hth & (0x00000001<<i))
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(i), pmred));
        else
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(i), pmgrn));

        hlyout->addWidget(WDFunc::NewLBLT(w, HthToolTip().at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    w->setLayout(vlayout);
    connect(this,SIGNAL(BsiRefresh(ModuleBSI::Bsi*)), this, SLOT(UpdateHthWidget(ModuleBSI::Bsi*)));

    //hlyout->addLayout(l2yout,100);
    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb,0);
    dlg->setLayout(lyout);
    dlg->show();
}

void Coma::PredAlarmState()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;

    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    QStringList events = QStringList() << "Отсутствует сигнал напряжения фазы A                   "
                                       << "Отсутствует сигнал напряжения фазы B                   "
                                       << "Отсутствует сигнал напряжения фазы С                   "
                                       << "Отсутствует ток ввода фазы А (ток меньше 2мА)          "
                                       << "Отсутствует ток ввода фазы B (ток меньше 2мА)          "
                                       << "Отсутствует ток ввода фазы C (ток меньше 2мА)          "
                                       << "Не заданы начальные значения                           "
                                       << "Низкое напряжение фазы A                               "
                                       << "Низкое напряжение фазы B                               "
                                       << "Низкое напряжение фазы C                               "
                                       << "Сигнализация по приращению тангенса дельта ввода фазы А"
                                       << "Сигнализация по приращению тангенса дельта ввода фазы B"
                                       << "Сигнализация по приращению тангенса дельта ввода фазы C"
                                       << "Сигнализация по приращению C ввода фазы А              "
                                       << "Сигнализация по приращению C ввода фазы B              "
                                       << "Сигнализация по приращению C ввода фазы C              "
                                       << "Не заданы паспортные значения                          "
                                       << "Сигнализация по повышенному небалансу токов            ";


    QWidget *w = new QWidget;
    Wpred = w;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");  // color: rgba(220,220,220,255);

    for (int i = 0; i < 13; ++i)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3011+i), (PredAlarmEvents[i]) ? pmred : pmgrn));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }
    for (int i = 0; i < 3; ++i)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3027+i), (PredAlarmEvents[13+i]) ? pmred : pmgrn));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(13+i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3033), (PredAlarmEvents[16]) ? pmred : pmgrn));
    hlyout->addWidget(WDFunc::NewLBLT(w, events.at(16), "", "", ""), 1);
    vlayout->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3034), (PredAlarmEvents[17]) ? pmred : pmgrn));
    hlyout->addWidget(WDFunc::NewLBLT(w, events.at(17), "", "", ""), 1);
    vlayout->addLayout(hlyout);

    w->setLayout(vlayout);

    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb,0);
    dlg->setLayout(lyout);
    dlg->show();
}


void Coma::AlarmState()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    QString tmps = QString(PROGCAPTION);

    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    //QPixmap *pm[2] = {pmred, pmgrn};
    QStringList events = QStringList() << "Авария по приращению тангенса дельта ввода фазы А"
                                       << "Авария по приращению тангенса дельта ввода фазы B"
                                       << "Авария по приращению тангенса дельта ввода фазы C"
                                       << "Авария по приращению C ввода фазы А              "
                                       << "Авария по приращению C ввода фазы B              "
                                       << "Авария по приращению C ввода фазы C              "
                                       << "Авария по недопустимому небалансу токов          ";

    QWidget *w = new QWidget;
    Walarm = w;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");  // color: rgba(220,220,220,255);

    for (int i = 0; i < 3; ++i)
    {
        hlyout = new QHBoxLayout;

        if(AlarmEvents[i])
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3024+i), pmred));
        else
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3024+i), pmgrn));

        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    for (int i = 0; i < 3; ++i)
    {
        hlyout = new QHBoxLayout;

        if(AlarmEvents[3+i])
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3030+i), pmred));
        else
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3030+i), pmgrn));

        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(3+i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    hlyout = new QHBoxLayout;
    if(AlarmEvents[6])
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3035), pmred));
    else
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3035), pmgrn));

    hlyout->addWidget(WDFunc::NewLBLT(w, events.at(6), "", "", ""), 1);
    vlayout->addLayout(hlyout);

    w->setLayout(vlayout);

    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb,0);
    dlg->setLayout(lyout);


            dlg->show();
}


void Coma::UpdateReleWidget(Parse104::SponSignals104* Signal)
{
    int i = 0;
    //Parse104::SponSignals104 sig = *new Parse104::SponSignals104;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    QPixmap *pm[2] = {pmgrn, pmred};

    //for(j=0; j<Signal->SigNumber; j++)
    //{
       //sig = *(Signal+j);
    if(!(Signal->Spon[i].SigVal & 0x80))
    {
       int signal = ((Signal->Spon[i].SigVal & (0x00000001)) ? 1 : 0);
       WDFunc::SetLBLImage(this, (QString::number(Signal->Spon[i].SigAdr)), pm[signal]);
    }
    //}

}

void Coma::UpdatePredAlarmEvents(Parse104::SponSignalsWithTime* Signal)
{
    int i = 0;
    //Parse104::SponSignalsWithTime* sig = new Parse104::SponSignalsWithTime;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    QPixmap *pm[2] = {pmgrn, pmred};

    for(i=0; i<Signal->SigNumber; i++)
    {
       //sig->Spon->SigAdr = (Signal->Spon[i].SigAdr);
       //sig->Spon->SigVal = (Signal->Spon[i].SigVal);
       if(!(Signal->Spon[i].SigVal & 0x80))
       {
        quint8 signal = ((Signal->Spon[i].SigVal & (0x00000001)) ? 1 : 0);

        if((((Signal->Spon[i].SigAdr >= 3024) && (Signal->Spon[i].SigAdr < 3027))) || ((Signal->Spon[i].SigAdr >= 3030) && (Signal->Spon[i].SigAdr < 3033)) || (Signal->Spon[i].SigAdr == 3035))
        WDFunc::SetLBLImage(Walarm, (QString::number(Signal->Spon[i].SigAdr)), pm[signal]);
        else
        WDFunc::SetLBLImage(Wpred, (QString::number(Signal->Spon[i].SigAdr)), pm[signal]);
       }
    }


}


void Coma::UpdateStatePredAlarmEvents(Parse104::SponSignals104 *Signal)
{
    int i = 0, predalarmcount = 0, alarmcount = 0;
//    Parse104::SponSignals104 sig = *new Parse104::SponSignals104;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    QPixmap *pm[2] = {pmred, pmgrn};

    for(i=0; i<Signal->SigNumber; i++)
    {
       //sig = *(Signal+i);
       if(!(Signal->Spon[i].SigVal & 0x80))
       {
           quint8 signal = ((Signal->Spon[i].SigVal & (0x00000001)) ? 1 : 0);
           quint32 sigadr = Signal->Spon[i].SigAdr;

           if((sigadr >= 3011) && (sigadr < 3024))
           PredAlarmEvents[sigadr - 3011] = signal;
           else if((sigadr >= 3024) && (sigadr < 3027))
           AlarmEvents[sigadr - 3024] = signal;
           else if((sigadr >= 3027) && (sigadr < 3030))
           PredAlarmEvents[sigadr - 3014] = signal;
           else if((sigadr >= 3030) && (sigadr < 3033))
           AlarmEvents[sigadr - 3027] = signal;
           else if(sigadr == 3033)
           PredAlarmEvents[16] = signal;
           else if(sigadr == 3034)
           PredAlarmEvents[17] = signal;
           else if(sigadr == 3035)
           AlarmEvents[6] = signal;
       }
    }

    for(i=0; i<18; i++)
    {
        if(PredAlarmEvents[i])
        {
           WDFunc::SetLBLImage(this, QString::number(951), pm[0]);
           predalarmcount++;
        }
    }

    if(predalarmcount == 0)
    WDFunc::SetLBLImage(this,  QString::number(951), pm[1]);

    for(i=0; i<7; i++)
    {
        if(AlarmEvents[i])
        {
           WDFunc::SetLBLImage(this, QString::number(952), pm[0]);
           alarmcount++;
        }
    }

    if(alarmcount == 0)
    WDFunc::SetLBLImage(this,  QString::number(952), pm[1]);

}

void Coma::UpdateStatePredAlarmEventsWithTime(Parse104::SponSignalsWithTime *Signal)
{
    int i = 0, PredArarmcount = 0, Ararmcount = 0;
    //Parse104::SponSignals104 sig = *new Parse104::SponSignals104;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    QPixmap *pm[2] = {pmred, pmgrn};

    for(i=0; i<Signal->SigNumber; i++)
    {
       //sig = *(Signal+i);
       if(!(Signal->Spon[i].SigVal & 0x80))
       {
           quint8 signal = ((Signal->Spon[i].SigVal & (0x00000001)) ? 1 : 0);

           if((Signal->Spon[i].SigAdr >= 3011) && (Signal->Spon[i].SigAdr < 3024))
           PredAlarmEvents[Signal->Spon[i].SigAdr - 3011] = signal;
           else if((Signal->Spon[i].SigAdr >= 3024) && (Signal->Spon[i].SigAdr < 3027))
           AlarmEvents[Signal->Spon[i].SigAdr - 3024] = signal;
           else if((Signal->Spon[i].SigAdr >= 3027) && (Signal->Spon[i].SigAdr < 3030))
           PredAlarmEvents[Signal->Spon[i].SigAdr - 3014] = signal;
           else if((Signal->Spon[i].SigAdr >= 3030) && (Signal->Spon[i].SigAdr < 3033))
           AlarmEvents[Signal->Spon[i].SigAdr - 3027] = signal;
           else if(Signal->Spon[i].SigAdr == 3033)
           PredAlarmEvents[16] = signal;
           else if(Signal->Spon[i].SigAdr == 3034)
           PredAlarmEvents[17] = signal;
           else if(Signal->Spon[i].SigAdr == 3035)
           AlarmEvents[6] = signal;
       }
    }

    for(i=0; i<18; i++)
    {
        if(PredAlarmEvents[i])
        {
           WDFunc::SetLBLImage(this, QString::number(951), pm[0]);
           PredArarmcount++;
        }
    }

    if(PredArarmcount == 0)
    WDFunc::SetLBLImage(this,  QString::number(951), pm[1]);

    for(i=0; i<7; i++)
    {
        if(AlarmEvents[i])
        {
           WDFunc::SetLBLImage(this, QString::number(952), pm[0]);
           Ararmcount++;
        }
    }

    if(Ararmcount == 0)
    WDFunc::SetLBLImage(this,  QString::number(952), pm[1]);

}

void Coma::ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)
{
    int i = 0, PredArarmcount = 0, Ararmcount = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    QPixmap *pm[2] = {pmred, pmgrn};

    for(i=0; i<Signal.countBytes; i++)
    {
        if(i == 3)
        {
            //for(int j = 0; j<8; j++)
            //{
                quint8 signal = ((Signal.Bytes[i] & (0x00000001)) ? 1 : 0);
                AlarmEvents[6] = signal;

        }
        if(i == 2)
        {
            for(int j = 0; j<8; j++)
            {
                quint8 signal = ((Signal.Bytes[i] & (0x00000001 << j)) ? 1 : 0);

                if(j<3)
                PredAlarmEvents[13+j] = signal;
                else if(j>=3 && j<6)
                AlarmEvents[j] = signal;
                if(j==6)
                PredAlarmEvents[16] = signal;
                if(j==7)
                PredAlarmEvents[17] = signal;

            }

        }
        else if(i == 0)
        {
            for(int j = 0; j<8; j++)
            {
                quint8 signal = ((Signal.Bytes[i] & (0x00000001 << j)) ? 1 : 0);
                PredAlarmEvents[j] = signal;
            }
        }
        else if(i == 1)
        {
            for(int j = 0; j<8; j++)
            {
                quint8 signal = ((Signal.Bytes[i] & (0x00000001 << j)) ? 1 : 0);

                if(j<5)
                PredAlarmEvents[8+j] = signal;
                if(j >= 5)
                AlarmEvents[j-5] = signal;
            }
        }
    }

    for(i=0; i<18; i++)
    {
        if(PredAlarmEvents[i])
        {
           WDFunc::SetLBLImage(this, QString::number(951), pm[0]);
           PredArarmcount++;
        }
    }

    if(PredArarmcount == 0)
    WDFunc::SetLBLImage(this,  QString::number(951), pm[1]);

    emit SetPredAlarmColor(PredAlarmEvents);

    for(i=0; i<7; i++)
    {
        if(AlarmEvents[i])
        {
           WDFunc::SetLBLImage(this, QString::number(952), pm[0]);
           Ararmcount++;
        }
    }

    if(Ararmcount == 0)
    WDFunc::SetLBLImage(this,  QString::number(952), pm[1]);

    emit SetAlarmColor(AlarmEvents);

}

void Coma::ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)
{
    int i = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    QPixmap *pm[2] = {pmgrn, pmred};

    for(i=0; i<Signal.countBytes; i++)
    {
        if(i == 3)
        {
           quint8 signal = ((Signal.Bytes[i] & (0x00000001)) ? 1 : 0);
           WDFunc::SetLBLImage(Walarm, (QString::number(3035)), pm[signal]);
        }

        if(i == 2)
        {
            for(int j = 0; j<8; j++)
            {
                quint8 signal = ((Signal.Bytes[i] & (0x00000001 << j)) ? 1 : 0);

                if(j<3)
                WDFunc::SetLBLImage(Wpred, (QString::number(3027+j)), pm[signal]);
                else if(j>=3 && j<6)
                WDFunc::SetLBLImage(Walarm, (QString::number(3030+j-3)), pm[signal]);
                if(j==6)
                WDFunc::SetLBLImage(Wpred, (QString::number(3033)), pm[signal]);
                if(j==7)
                WDFunc::SetLBLImage(Wpred, (QString::number(3034)), pm[signal]);

            }

        }
        else if(i == 0)
        {
            for(int j = 0; j<8; j++)
            {
                quint8 signal = ((Signal.Bytes[i] & (0x00000001 << j)) ? 1 : 0);
                WDFunc::SetLBLImage(Wpred, (QString::number(3011+j)), pm[signal]);
            }
        }
        else if(i == 1)
        {
            for(int j = 0; j<8; j++)
            {
                quint8 signal = ((Signal.Bytes[i] & (0x00000001 << j)) ? 1 : 0);

                if(j<5)
                WDFunc::SetLBLImage(Wpred, (QString::number(3019+j)), pm[signal]);
                if(j >= 5)
                WDFunc::SetLBLImage(Walarm, (QString::number(3024+j-5)), pm[signal]);
            }
        }
    }

}

QWidget *Coma::Least()
{
    QWidget *w = new QWidget;
//    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *inlyout = new QHBoxLayout;
    ETabWidget *MainTW = new ETabWidget;
    MainTW->setObjectName("maintw");
    MainTW->setTabPosition(QTabWidget::West);
//    MainTW->tabBar()->setStyleSheet("QTabBar::tab {background-color: yellow;}");
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
    QString HomeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/"+PROGNAME+"/";
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    StdFunc::SetHomeDir(sets->value("Homedir", HomeDir).toString());
}

void Coma::SaveSettings()
{
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    sets->setValue("Homedir", StdFunc::GetHomeDir());
}

void Coma::ClearTW()
{
    S2Config.clear();
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
    connect(dlg,SIGNAL(Finished(QString)),this,SLOT(PasswordCheck(QString)));
    connect(this,SIGNAL(PasswordChecked()),&PasswordLoop,SLOT(quit()));
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

void Coma::UpdateHthWidget(ModuleBSI::Bsi* bsi)
{
    //ModuleBSI::Bsi bsi = ModuleBSI::GetBsi();
    ModuleBSI::ModuleBsi = *bsi;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    QPixmap *pm[2] = {pmred, pmgrn};

    for (int i = 0; i < MAXERRORFLAGNUM; i++)
    {
        QLabel *lbl = this->findChild<QLabel *>(QString::number(i));  //hth+ (i+1)
        if (lbl == nullptr)
        {
           ERMSG("Нулевой lbl");
           return;
        }
        quint32 tmpui = (0x00000001 << i) & bsi->Hth;

        WDFunc::SetLBLImage(this, (QString::number(i)), pm[tmpui]);
        /*if (tmpui)
            lbl->setStyleSheet("QLabel {background-color: rgba(255,10,10,255); color: rgba(255,255,255,255);}");
        else
            lbl->setStyleSheet("QLabel {background-color: rgba(255,50,50,0); color: rgba(220,220,220,255);}");*/
    }
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

void Coma::PasswordCheck(QString psw)
{
    if (psw == "se/7520a")
        Ok = true;
    else
        Ok = false;
    emit PasswordChecked();
}

/*int Coma::OpenBhbDialog()
{
    if (!Commands::isConnected())
    {
        QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модулем" : "прибором");
        EMessageBox::information(this, "Подтверждение", "Для работы данной функции необходимо сначала установить связь с "+tmps);
        return GENERALERROR;
    }
    if (CheckPassword() == GENERALERROR)
        return GENERALERROR;

    HiddenDialog *dlg = new HiddenDialog();
    dlg->Fill(); // заполняем диалог из недавно присвоенных значений
    dlg->exec();
    if (!dlg->ResultOk)
        return GENERALERROR;
    Disconnect();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (Commands::Connect() != NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Не удалось установить связь");
        QApplication::restoreOverrideCursor();
        return GENERALERROR;
    }
    QApplication::restoreOverrideCursor();
    int res;
    if ((res = ModuleBSI::SetupBSI()) == Error::ER_CANAL)
    {
        EMessageBox::error(this, "Ошибка", "Блок Bsi не может быть прочитан, ошибка " + QString::number(res));
        Commands::Disconnect();
        return GENERALERROR;
    }
    //emit BsiRefresh();
    return NOERROR;
}*/

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
    WDFunc::SetLBLText(this, lblname,StdFunc::PrbMessage() + QString::number(0), false);
    //prb->setMinimum(0);
    //prb->setMaximum(0);

   ReceiveTimer->stop();
   if(fileSize != curfileSize && MainInterface != I_USB)
   {
     EMessageBox::information(this, "Ошибка", "Ошибка");
   }
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
    //int res, i;
    QDialog *dlg = new QDialog(this);
    QString Str;
    Cancelled = false;
    //QStringList device = QStringList() << "KDV" << "2" << "1" << "2";
    QStringList inter;
    inter.append("USB");
    inter.append("Ethernet");
    inter.append("RS485");
    QStringListModel *tmpmodel = new QStringListModel;
    tmpmodel->deleteLater();
    dlg->setMinimumWidth(150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("connectdlg");
    QVBoxLayout *lyout = new QVBoxLayout;

    lyout->addWidget(WDFunc::NewLBL(this, "Выберите интерфейс связи"));

    //tmpmodel = new QStringListModel;
    tmpmodel->setStringList(inter);
    QComboBox *portscb = new QComboBox;
    connect(portscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(ParseInter(QString)));
    portscb->setModel(tmpmodel);
    lyout->addWidget(portscb);
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Далее");
    connect(pb, SIGNAL(clicked(bool)),dlg,SLOT(close()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb, SIGNAL(clicked(bool)),this,SLOT(SetCancelled()));
    connect(pb, SIGNAL(clicked(bool)),dlg, SLOT(close()));
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

    if(Cancelled)
    {
        ERMSG("Отмена ConnectDialog");
        return;
    }

    //QStringList device = QStringList() << "KDV" << "2" << "1" << "2";
    //QStringList inter = QStringList() << "ETH" << "MODBUS";

    QDialog *dlg = new QDialog(this);
    dlg->setMinimumWidth(150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("connectdlg");
    QVBoxLayout *lyout = new QVBoxLayout;
    QStringListModel *tmpmodel = new QStringListModel;

    if(MainInterface == I_USB)
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
       connect(portscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetPortSlot(QString)));
       portscb->setModel(tmpmodel);
       lyout->addWidget(portscb);
    }
    else
    {
       if(!HaveAlreadyRed)
       {
           sl.clear();
           res= Files::LoadFromFile(Files::ChooseFileForOpen(this, "IP files (*.txt)"), ba);
           if (res != Files::ER_NOERROR)
           {
              WARNMSG("Ошибка при загрузке файла");
              return;
           }

           Str = ba;
           sl.append(Str.split("\r\n"));

           /*for(i=0; i<sl.size(); i++)
           {
             insl.clear();
             insl.append(sl.at(i).split(" "));

              if((insl.at(0) == SaveDevice) && (insl.at(1) == interface))
              {
                slfinal.append(sl.at(i));
              }
           }*/

           HaveAlreadyRed = 1;

       }


       if (sl.size() == 0)
       {
           lyout->addWidget(WDFunc::NewLBL(this, "Ошибка, устройства не найдены"));
           Error::ShowErMsg(CN_NOPORTSERROR);
           ERMSG("Ошибка, устройства не найдены");
       }

       //dlg = new QDialog(this);

       //tmpmodel = new QStringListModel;

        tmpmodel->deleteLater();
        tmpmodel->setStringList(sl);

        QComboBox *portscb = new QComboBox;
        connect(portscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(ParseString(QString)));
        portscb->setModel(tmpmodel);
        lyout->addWidget(portscb);
    }
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Далее");
    connect(pb, SIGNAL(clicked(bool)),dlg,SLOT(close()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb, SIGNAL(clicked(bool)),this,SLOT(SetCancelled()));
    connect(pb, SIGNAL(clicked(bool)),dlg, SLOT(close()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    dlg->exec();
}

void Coma::SetProgressBarSize(QString prbnum, int size)
{
    QString prbname = "prb"+prbnum+"prb";
    QString lblname = "prb"+prbnum+"lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb == nullptr)
    {
        DBGMSG;
        return;
    }
    WDFunc::SetLBLText(this, lblname,StdFunc::PrbMessage() + QString::number(size), false);
    prb->setMinimum(0);
    prb->setMaximum(size);
}

void Coma::SetProgressBar(QString prbnum, int cursize)
{
    QString prbname = "prb"+prbnum+"prb";
    QString lblname = "prb"+prbnum+"lbl";
    QProgressBar *prb = this->findChild<QProgressBar *>(prbname);
    if (prb != nullptr)
    {
        prb->setValue(cursize);
        WDFunc::SetLBLText(this, lblname, StdFunc::PrbMessage() + QString::number(cursize) + " из " + QString::number(prb->maximum()));
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
    l2yout->addWidget(WDFunc::NewLBL(this, "2015-2019 гг."));
    l2yout->addWidget(WDFunc::NewLBL(this, "info@avmenergo.ru"));
    l2yout->addStretch(10);
    hlyout->addWidget(WDFunc::NewLBL(this, "", "", "", new QPixmap("images/evel.png")), 1, Qt::AlignVCenter);
    hlyout->addLayout(l2yout,100);
    lyout->addLayout(hlyout,1);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb,0);
    dlg->setLayout(lyout);
    dlg->show();
}

void Coma::Disconnect()
{
    if (!StdFunc::IsInEmulateMode())
    {
        if(MainInterface == I_USB)
        {
            BdaTimer->stop();
            TimeFunc::Wait(100);
            if(cn->Connected)
            cn->Disconnect();
        }
        else
        {
            emit StopCommunications();
        }
        while (ActiveThreads) // wait for all threads to finish
            QCoreApplication::processEvents();
    }
}

void Coma::GetDeviceFromTable(QModelIndex idx)
{
    Q_UNUSED(idx)
    ETableView *tv = this->findChild<ETableView *>("devicetv");
    if (tv == nullptr)
    {
        ERMSG("Пустой tv");
        DBGMSG;
        return;
    }
    QString tmps = tv->model()->index(tv->currentIndex().row(), 0, QModelIndex()).data(Qt::DisplayRole).toString();
    DevInfo.vendor_id = tmps.toUShort(nullptr, 16);
//    quint16 vid = tmps.toInt(nullptr, 16);
    tmps = tv->model()->index(tv->currentIndex().row(), 1, QModelIndex()).data(Qt::DisplayRole).toString();
    DevInfo.product_id = tmps.toUShort(nullptr, 16);
//    quint16 pid = tmps.toInt(nullptr, 16);
    tmps = tv->model()->index(tv->currentIndex().row(), 3, QModelIndex()).data(Qt::DisplayRole).toString();
    tmps.toWCharArray(DevInfo.serial);
}

void Coma::DisconnectAndClear()
{
    TheEnd = 1;
    if(TimeTimer != nullptr)
    {
      TimeTimer->stop();
    }

    if(Time->timer != nullptr)
    {
      Time->timer->stop();
    }

    if(!Disconnected)
    {
        StopRead = 1;
        Disconnected = 1;
        Disconnect();

        CheckB = CheckM = nullptr;
        //CheckModBus = nullptr;
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

        if(MainInterface == I_USB)
        {
            if(Reconnect)
            {
                Reconnect = false;
                if(ModuleBSI::ModuleTypeString != "")
                EMessageBox::information(this, "Разрыв связи", "Связь с "+ModuleBSI::ModuleTypeString+" разорвана");
                else
                EMessageBox::information(this, "Разрыв связи", "Связь разорвана");
            }
            else
            {
                if(ModuleBSI::ModuleTypeString != "")
                EMessageBox::information(this, "Разрыв связи", "Не удалось установить связь с "+ModuleBSI::ModuleTypeString+"");
                else
                EMessageBox::information(this, "Разрыв связи", "Не удалось установить связь");
            }
        }
        else
        {
            if(Reconnect)
            {
                Reconnect = false;
                if(FullName != "")
                    EMessageBox::information(this, "Разрыв связи", "Связь с "+FullName+" разорвана");
                else
                    EMessageBox::information(this, "Разрыв связи", "Связь разорвана");
            }
            else
            {
                if(FullName != "")
                    EMessageBox::information(this, "Разрыв связи", "Не удалось установить связь с "+FullName+"");
                else
                    EMessageBox::information(this, "Разрыв связи", "Не удалось установить связь");
            }
        }

        Reconnect = false;
        //disconnected = 0;
    }


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
        if(sl.size() > 6)
        {
            bool ok;
            FullName = sl.at(0);
            Settings.Baud =  sl.at(2).toUInt(&ok);
            if (!ok)
            {
                EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
                ERMSG("Некорректная запись в файле");
                return;
            }
            Settings.Parity =  sl.at(3);
            Settings.Stop =  sl.at(4);
            Settings.Address =  sl.at(5).toUInt(&ok);
            if (!ok)
            {
                ERMSG("Некорректная запись в файле");
                EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
                return;
            }
            Settings.Port =  sl.at(6);
        }
        else
        {
            EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
        }
    }
    else
        EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
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
    ChModbus->Tabs(tabindex);
    if(CheckB != nullptr)
    {
        if(tabindex == CheckIndex)
            BdaTimer->start();
        else
            BdaTimer->stop();
    }
    if(Time != nullptr)
    {
        if(tabindex == Time->timeIndex)
            TimeTimer->start();
        else
            TimeTimer->stop();
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
    //EMessageBox::information(this, "Информация", "В модуле нет конфигурации. \nНеобходимо записать конфигурацию по умолчанию.");
    //if(ConfB != nullptr)
    //ConfB->WriteConf();
    //if(ConfM != nullptr)
    //ConfM->WriteConf();
    EMessageBox::information(this, "Успешно", "Конфигурация по умолчанию");
}


void Coma::DisconnectMessage()
{
    EMessageBox::information(this, "Провал", "Нет подключения");
}

void Coma::FinishHim()
{
    ConfM->confIndex = -1;
    TimeFunc::Wait(1000);
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

void Coma::UpdateUSBAlarm()
{
    int i = 0, predalarmcount = 0, alarmcount = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");

    if (CheckB != nullptr)
        CheckB->USBUpdate();
    if (Commands::GetBd(11, &Bd_block11, sizeof(Bd11)) == NOERROR)
    {
        for(i=0; i<18; i++)
        {
           if(Bd_block11.predAlarm & (0x00000001 << i))
           {
               PredAlarmEvents[i] = 1;
               ++predalarmcount;
           }
           else
               PredAlarmEvents[i] = 0;
        }

        for(i=0; i<7; i++)
        {
           if(Bd_block11.alarm & (0x00000001 << i))
           {
               AlarmEvents[i] = 1;
               ++alarmcount;
           }
           else
               AlarmEvents[i] = 0;
        }
        WDFunc::SetLBLImage(this,  QString::number(951), (predalarmcount == 0) ? pmgrn : pmred);
        WDFunc::SetLBLImage(this,  QString::number(952), (alarmcount == 0) ? pmgrn : pmred);
        emit SetPredAlarmColor(PredAlarmEvents);
        emit SetAlarmColor(AlarmEvents);
    }
}

void Coma::USBSetAlarms()
{
    int i = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    QPixmap *pm[2] = {pmred, pmgrn};
    Bd_block11 = *new Bd11;

    if (Commands::GetBd(11, &Bd_block11, sizeof(Bd11)) == NOERROR)
    {
        if(Wpred != nullptr)
        {
            for(i=0; i<13; i++)
            {
               if(Bd_block11.predAlarm & ((quint32)0x00000001 << i))
               WDFunc::SetLBLImage(Wpred, (QString::number(3011+i)), pm[0]);
               else
               WDFunc::SetLBLImage(Wpred, (QString::number(3011+i)), pm[1]);
            }

            for(i=13; i<16; i++)
            {
               if(Bd_block11.predAlarm & ((quint32)0x00000001 << i))
               WDFunc::SetLBLImage(Wpred, (QString::number(3027+i-13)), pm[0]);
               else
               WDFunc::SetLBLImage(Wpred, (QString::number(3027+i-13)), pm[1]);
            }

            if(Bd_block11.predAlarm & ((quint32)0x00000001 << 16))
            WDFunc::SetLBLImage(Wpred, (QString::number(3033)), pm[0]);
            else
            WDFunc::SetLBLImage(Wpred, (QString::number(3033)), pm[1]);

            if(Bd_block11.predAlarm & ((quint32)0x00000001 << 17))
            WDFunc::SetLBLImage(Wpred, (QString::number(3034)), pm[0]);
            else
            WDFunc::SetLBLImage(Wpred, (QString::number(3034)), pm[1]);
        }


        if(Walarm != nullptr)
        {
            for(i=0; i<3; i++)
            {
               if(Bd_block11.alarm & ((quint32)0x00000001 << i))
               WDFunc::SetLBLImage(Walarm, (QString::number(3024+i)), pm[0]);
               else
               WDFunc::SetLBLImage(Walarm, (QString::number(3024+i)), pm[1]);
            }

            for(i=3; i<6; i++)
            {
               if(Bd_block11.alarm & ((quint32)0x00000001 << i))
               WDFunc::SetLBLImage(Walarm, (QString::number(3030+i-3)), pm[0]);
               else
               WDFunc::SetLBLImage(Walarm, (QString::number(3030+i-3)), pm[1]);
            }

            if(Bd_block11.alarm & ((quint32)0x00000001 << 6))
            WDFunc::SetLBLImage(Walarm, (QString::number(3035)), pm[0]);
            else
            WDFunc::SetLBLImage(Walarm, (QString::number(3035)), pm[1]);
        }
    }
}
