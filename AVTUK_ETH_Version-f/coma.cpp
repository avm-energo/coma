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
#include <QMenu>
#include <QMenuBar>
#include <QTextEdit>
#include <QToolBar>
#include <QDialog>
#include <QCursor>
#include <QThread>
#include "coma.h"
#include "../config/confdialog21.h"
#include "../config/confdialog22.h"
#include "../config/confdialog31.h"
#include "../config/confdialog35.h"
#include "../config/confdialog80.h"
#include "../config/confdialog84.h"
#include "../config/confdialog85.h"
#include "../config/confdialog87.h"
#include "../config/confdialoga1.h"
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
#include "../modbus/modbus.h"
#include "../gen/timefunc.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

Coma::Coma(QWidget *parent)
    : MainWindow(parent)
{
    StartWindowSplashScreen->finish(this);
    Autonomous = false;
}

Coma::~Coma()
{
}

/*void Coma::SetMode(int mode)
{
    Mode = mode;
}

int Coma::GetMode()
{
    return Mode;
}*

void Coma::Go(const QString &parameter)
{
    if (Mode != COMA_GENERALMODE)
    {
        StdFunc::SetEmulated(true);
        Autonomous = true;
    }
    SetupUI();
    show();
    switch (Mode)
    {
    case COMA_AUTON_OSCMODE:
    {
        LoadOscFromFile(parameter);
        break;
    }
    case COMA_AUTON_PROTMODE:
    {
        StartA1Dialog(parameter);
        break;
    }
    case COMA_AUTON_SWJMODE:
    {
        LoadSwjFromFile(parameter);
        break;
    }
    default:
        break;
    }
}*/

void Coma::Emul2x()
{
    QMenu *menu = new QMenu(this);
    for (int i=0; i<MTBs.size(); ++i)
    {
        QMenu *inmenu = new QMenu(this);
        QString prefix = QString::number(MTBs.at(i), 16);
        inmenu->setTitle(prefix);
        for (int j=0; j<MTBs.size(); ++j)
        {
            QAction *act = new QAction(this);
            QString suffix = QString::number(MTBs.at(j), 16);
            QString tmps = prefix + suffix;
            act->setText(tmps);
            act->setObjectName(tmps);
            act->setIcon(QIcon(QPixmap::fromImage(*(WDFunc::TwoImages(prefix, suffix)))));
            connect(act,SIGNAL(triggered()),this,SLOT(StartEmul()));
            inmenu->addAction(act);
        }
        menu->addMenu(inmenu);
    }
    menu->popup(QCursor::pos());
}

void Coma::Emul8x()
{
    QMenu *menu = new QMenu(this);
    for (int i=81; i<=87; ++i)
    {
        QString tmps = QString::number(i);
        QAction *act = new QAction(this);
        act->setText(tmps);
        if (i < 84)
        {
            act->setObjectName("80"+tmps);
            act->setIcon(QIcon(QPixmap::fromImage(*(WDFunc::TwoImages("80", tmps)))));
        }
        else
        {
            act->setObjectName(tmps+tmps);
            act->setIcon(QIcon(QPixmap::fromImage(*(WDFunc::TwoImages(tmps, tmps)))));
        }
        connect(act,SIGNAL(triggered()),this,SLOT(StartEmul()));
        menu->addAction(act);
    }
    menu->popup(QCursor::pos());
}

void Coma::SetupUI()
{
    setWindowTitle(PROGCAPTION);
    QString tmps = "QMainWindow {background-color: "+QString(MAINWINCLR)+";}";
    setStyleSheet(tmps);
    setMinimumSize(QSize(800, 700));
    QWidget *wdgt = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QAction *act;

    QHBoxLayout *hlyout = new QHBoxLayout;
    QToolBar *tb = new QToolBar;
    tb->setStyleSheet("QToolBar {background: 0px; margin: 0px; spacing: 5px; padding: 0px;}");
    tb->setIconSize(QSize(20,20));
    if (!Autonomous)
    {
    /*#if PROGSIZE != PROGSIZE_EMUL
        ch104 = new iec104;
        connect(ch104,SIGNAL(Retry()),this,SLOT(ShowConnectDialog()));
        connect(ch104,SIGNAL(SetDataSize(int)),this,SLOT(SetProgressBar1Size(int)));
        connect(ch104,SIGNAL(SetDataCount(int)),this,SLOT(SetProgressBar1(int)));
        connect(ch104,SIGNAL(readbytessignal(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
        connect(ch104,SIGNAL(writebytessignal(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
        connect(ch104, SIGNAL(ShowError(QString)), this, SLOT(ShowErrorMessageBox(QString)));
        connect(this,SIGNAL(Retry()),this,SLOT(Stage1_5()));
        #endif
*/
        act = new QAction(this);
        act->setToolTip("Соединение");
        act->setIcon(QIcon("images/play.png"));
        connect(act,SIGNAL(triggered()),this,SLOT(Stage1_5()));
        tb->addAction(act);
    }
    act = new QAction(this);
    act->setToolTip("Разрыв соединения");
    act->setIcon(QIcon("images/stop.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(DisconnectAndClear()));
    tb->addAction(act);
    tb->addSeparator();
#if PROGSIZE == PROGSIZE_EMUL
    if (Autonomous)
        AddEmulToToolbar(tb);
#endif
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

void Coma::AddEmulToToolbar(QToolBar *tb)
{
    QAction *act = new QAction(this);
    act->setToolTip("Эмуляция 21");
    act->setIcon(QIcon("images/2x.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(Emul2x()));
    tb->addAction(act);
    act = new QAction(this);
    act->setToolTip("Эмуляция 8x");
    act->setIcon(QIcon("images/8x.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(Emul8x()));
    tb->addAction(act);
    act = new QAction(this);
    act->setToolTip("Эмуляция A1");
    act->setIcon(QIcon("images/a1.png"));
//    quint16 MType = MTB_A1;
//    MType = (MType << 8) | MTM_00;
    act->setObjectName(QString::number(Config::MTB_A1, 16)); // для слота StartEmul
    connect(act,SIGNAL(triggered()),this,SLOT(StartEmul()));
    tb->addAction(act);
    tb->addSeparator();
}

void Coma::AddActionsToMenuBar(QMenuBar *menubar)
{
    QMenu *menu = new QMenu;
    menu->setTitle("Модуль А1");
    QAction *act = new QAction(this);
    act->setText("Протокол из файла (A1)");
    //connect(act,SIGNAL(triggered()),this,SLOT(ProtocolFromFile()));
    menu->addAction(act);
    menubar->addMenu(menu);
}

void Coma::Stage3()
{
    TheEnd = 0;
    QString str;
    int count = 0;
    MTypeB = 0;
    MTypeM = 0;
    ConfB = ConfM = nullptr;
    JourD = nullptr;
    ch104 = nullptr;
    CorD = nullptr;
    modBus = nullptr;
    ClearTW();
    ETabWidget *MainTW = this->findChild<ETabWidget *>("maintw");
    //MainTW->setMinimumSize(QSize(800,600));
    if (MainTW == nullptr)
        return;

    idlg = new InfoDialog;
    connect(this,SIGNAL(ClearBsi()),idlg,SLOT(ClearBsi()));

    CorD = new CorDialog();
    //connect(this,SIGNAL(ConnectMes(QString*)),CheckB,SLOT(ConnectMessage(QString*)));

    if(MainInterface == I_USB)
    {
        connect(this,SIGNAL(USBBsiRefresh()),idlg,SLOT(FillBsi()));
        MTypeB = ModuleBSI::GetMType(BoardTypes::BT_BASE);
        MTypeM = ModuleBSI::GetMType(BoardTypes::BT_MEZONIN);
        //CorD = new CorDialog();
        JourD = new JournalDialog();
    }
    else
    {
         if (MainInterface == I_ETHERNET)
         {
            ch104 = new iec104(&IPtemp, this);
            ch104->BaseAdr = AdrBaseStation;
            connect(ch104,SIGNAL(bs104signalsready(Parse104::BS104Signals*)),idlg,SLOT(FillBsiFrom104(Parse104::BS104Signals*)));           
            connect(this,SIGNAL(StopCommunications()),ch104,SLOT(Stop()));
            CheckB = new CheckDialog84(BoardTypes::BT_BASE, this, ch104);
         }
         else if (MainInterface == I_RS485)
         {
             modBus = new ModBus(Settings,this);
             if (modBus->Connect() != ModBus::ModbusDeviceState::ConnectedState)
                 return;
             connect(this,SIGNAL(StopCommunications()),modBus,SLOT(StopModSlot()));

             connect(Modthr, SIGNAL(finished()), this, SLOT(CheckModBusFinish()));

             CheckB = new CheckDialog84(BoardTypes::BT_BASE, this, nullptr);
             connect(modBus,SIGNAL(BsiFromModbus(ModBusBSISignal*, int)),idlg,SLOT(FillBsiFromModBus(ModBusBSISignal*, int)));
             connect(modBus,SIGNAL(CoilSignalsReady(Coils*)),this,SLOT(ModbusUpdateStatePredAlarmEvents(Coils*)));
             connect(MainTW, SIGNAL(tabClicked(int)), modBus,SLOT(tabs(int)));
             modBus->BSIrequest();
             //TimeTimer->setInterval(3000);

         }

         while(MTypeB == 0)
         {
           TimeFunc::Wait(100);
           count++;
           if(count == 50)
           {
             count = 0;
             if(reconnect)
             {
               if(MainInterface == I_ETHERNET || MainInterface == I_RS485)
               ReConnect();
             }
             else
             DisconnectAndClear();
             return;
           }
         }

         //emit ConnectMes(&FullName);
    }

    reconnect = true;

    if(MainInterface == I_ETHERNET)
    {
        if (insl.size() < 2)
            return;
        if(insl.at(1) != "MODBUS")
        {
          JourD = new JournalDialog();
        }
    }

    if (MainInterface == I_USB)
        emit USBBsiRefresh();

    PrepareDialogs();

    str = (CheckM == nullptr) ? "Текущие параметры" : "Текущие параметры\nБазовая";
    if (CheckB != nullptr)
    {
        CheckB->setMinimumHeight(500);
        //MainTW->setFixedHeight(500);
        MainTW->addTab(CheckB, str);
        CheckB->checkIndex = MainTW->indexOf(CheckB);

        if(MainInterface == I_RS485)
        modBus->CheckIndex = CheckB->checkIndex;

        if(MainInterface == I_USB)
        {
            connect(MainTW, SIGNAL(tabClicked(int)), this,SLOT(Start_BdaTimer(int))); //tabClicked
            connect(MainTW, SIGNAL(tabClicked(int)), this,SLOT(Stop_BdaTimer(int)));
            connect(ConfM, SIGNAL(stopRead(int)), this,SLOT(Stop_BdaTimer(int)));
            ConfM->checkIndex = CheckB->checkIndex;
            connect(BdaTimer,SIGNAL(timeout()),CheckB,SLOT(BdTimerTimeout()));
            connect(BdaTimer,SIGNAL(timeout()),this,SLOT(GetUSBAlarmTimerTimeout()));
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
        ConfM->confIndex = MainTW->indexOf(ConfM);
        connect(MainTW, SIGNAL(tabClicked(int)), ConfM, SLOT(ReadConf(int))); //tabClicked
        connect(ConfM, SIGNAL(stopRead(int)), this,SLOT(Stop_TimeTimer(int)));
    }

        Time = new MNKTime();
        connect(MainTW, SIGNAL(tabClicked(int)), Time,SLOT(Start_Timer(int))); //tabClicked
        //connect(MainTW, SIGNAL(tabClicked(int)), Time,SLOT(Stop_Timer(int)));
        connect(MainTW, SIGNAL(tabClicked(int)), this,SLOT(Start_TimeTimer(int))); //tabClicked
        connect(MainTW, SIGNAL(tabClicked(int)), this,SLOT(Stop_TimeTimer(int)));
        MainTW->addTab(Time, "Время");
        Time->timeIndex = MainTW->indexOf(Time);
        if(ConfM != nullptr)
        ConfM->timeIndex = Time->timeIndex;

        if(MainInterface == I_RS485)
        modBus->TimeIndex = Time->timeIndex;

        connect(TimeTimer,SIGNAL(timeout()),Time,SLOT(slot2_timeOut()));
        if(ch104 != nullptr)
        {
            connect(Time,SIGNAL(ethTimeRequest()),ch104,SLOT(InterrogateTimeGr15()));
            connect(ch104,SIGNAL(bs104signalsready(Parse104::BS104Signals*)),Time,SLOT(FillTimeFrom104(Parse104::BS104Signals*)));
            connect(Time,SIGNAL(ethWriteTimeToModule(uint)),ch104,SLOT(com51WriteTime(uint)));
        }
        if(modBus != nullptr)
        {
            connect(Time,SIGNAL(modBusTimeRequest()),modBus,SLOT(ReadTime()));
            connect(modBus,SIGNAL(TimeSignalsReceived(ModBusBSISignal*)),Time,SLOT(FillTimeFromModBus(ModBusBSISignal*)));
            connect(Time,SIGNAL(modbusWriteTimeToModule(uint)),modBus,SLOT(WriteTime(uint)));
            connect(modBus,SIGNAL(timeReadError()),Time,SLOT(ErrorRead()));
        }



    if (CorD != nullptr)
    {
        MainTW->addTab(CorD, "Начальные значения");
        CorD->corDIndex = MainTW->indexOf(CorD);
        connect(MainTW, SIGNAL(tabClicked(int)), CorD, SLOT(GetCorBd(int))); //tabClicked

        if(MainInterface == I_RS485)
        modBus->CorIndex = CorD->corDIndex;
    }

    if (JourD != nullptr)
    MainTW->addTab(JourD, "Журналы");

    if (ModuleBSI::Health() & HTH_CONFIG) // нет конфигурации
        Error::ShowErMsg(ER_NOCONF);
    if (ModuleBSI::Health() & HTH_REGPARS) // нет коэффициентов
        Error::ShowErMsg(ER_NOTUNECOEF);
    //connect(MainTW, SIGNAL(currentChanged()))
   // connect(this,SIGNAL(FinishAll()),this,SLOT(FinishHim()));

    if(MainInterface == I_USB)
    {
        FwUpD = new fwupdialog;
        MainTW->addTab(FwUpD, "Загрузка ВПО");
    }

    MainTW->addTab(idlg, "О приборе");

    MainTW->repaint();
    MainTW->show();
#if PROGSIZE >= PROGSIZE_LARGE
   // SetSlideWidget();
#endif

    if(MainInterface == I_ETHERNET || MainInterface == I_RS485)
    ConnectMessage();

    if(MainInterface == I_RS485)
    Modthr->start();

    if(MainInterface == I_USB)
    BdaTimer->start();

}

void Coma::PrepareDialogs()
{
    if (MainInterface == I_USB)
    MTypeB = ModuleBSI::GetMType(BoardTypes::BT_BASE);

    MTypeB =  MTypeB<<8;

    /*if (insl.size() != 0)
    {
        if(insl.at(1) == "MODBUS")
        {
            MTypeB = Config::MTB_A2;
        }
    }*/
     switch(MTypeB)
    {
    case Config::MTB_21:
    {
        ConfB = new ConfDialog21(S2Config, true);

        break;
    }
    case Config::MTB_22:
    {
        ConfB = new ConfDialog22(S2Config, true);

        break;
    }
    case Config::MTB_31:
    {
        ConfB = new ConfDialog31(S2Config, true);
        break;
    }
    case Config::MTB_35:
    {
        ConfB = new ConfDialog35(S2Config, true);
        break;
    }
    case Config::MTB_80:
    {

        break;
    }

    case Config::MTB_A2:
    {
       if(insl.size() != 0)
       {
        if(insl.at(1) == "ETH")
        {
            //ch104 = new iec104(&IPtemp, this);

            //ch104->IP = IPtemp;
            connect(ch104,SIGNAL(ethconnected()), this, SLOT(ConnectMessage()));
            //connect(ch104,SIGNAL(readConf()), ch104,SIGNAL(readConf()));
            //connect(ch104,SIGNAL(ethdisconnected()), this, SLOT(DisconnectMessage()));
            //connect(ch104,SIGNAL(ethNoconnection()), this, SLOT(DisconnectAndClear()));

            connect(CorD,SIGNAL(sendCom45(quint32)), ch104, SLOT(Com45(quint32)));
            connect(CorD,SIGNAL(sendCom50(quint32*, float*)), ch104, SLOT(Com50(quint32*,float*)));
            connect(CorD,SIGNAL(CorReadRequest()), ch104, SLOT(CorReadRequest()));

            connect(ch104,SIGNAL(sendMessageOk()), CorD, SLOT(MessageOk()));
            connect(ch104,SIGNAL(sendCorMesOk()), CorD, SLOT(WriteCorMessageOk()));

            connect(ch104,SIGNAL(relesignalsready(Parse104::SponSignals104*)), this, SLOT(UpdateReleWidget(Parse104::SponSignals104*)));

            connect(ch104,SIGNAL(floatsignalsready(Parse104::FlSignals104*)),CorD,SLOT(UpdateFlCorData(Parse104::FlSignals104*)));

            connect(ch104,SIGNAL(SetDataSize(int)),this,SLOT(SetProgressBar1Size(int)));
            connect(ch104,SIGNAL(SetDataCount(int)),this,SLOT(SetProgressBar1(int)));

            connect(ch104,SIGNAL(sponsignalsready(Parse104::SponSignals104*)),this,SLOT(UpdateStatePredAlarmEvents(Parse104::SponSignals104*)));
            connect(ch104,SIGNAL(sponsignalWithTimereceived(Parse104::SponSignalsWithTime*)),this,SLOT(UpdateStatePredAlarmEventsWithTime(Parse104::SponSignalsWithTime*)));

            setMinimumSize(QSize(800, 650));
            //CheckB = new CheckDialog84(BoardTypes::BT_BASE, this, ch104);
            connect(CheckB,SIGNAL(BsiRefresh(ModuleBSI::Bsi*)),this,SIGNAL(BsiRefresh(ModuleBSI::Bsi*)));

            //ConfM = new ConfDialog84(S2Config);
            connect(JourD,SIGNAL(ReadJour(char)), ch104, SLOT(SelectFile(char)));
/*            connect(ch104,SIGNAL(sendJourSysfromiec104(QVector<S2::DataRec>*)), JourD, SLOT(FillSysJour(QVector<S2::DataRec>*)));
            connect(ch104,SIGNAL(sendJourWorkfromiec104(QVector<S2::DataRec>*)), JourD, SLOT(FillWorkJour(QVector<S2::DataRec>*)));
            connect(ch104,SIGNAL(sendJourMeasfromiec104(QVector<S2::DataRec>*)), JourD, SLOT(FillMeasJour(QVector<S2::DataRec>*))); */
            connect(ch104,SIGNAL(sendJourSysfromiec104(QByteArray)), JourD, SLOT(FillSysJour(QByteArray)));
            connect(ch104,SIGNAL(sendJourWorkfromiec104(QByteArray)), JourD, SLOT(FillWorkJour(QByteArray)));
            connect(ch104,SIGNAL(sendJourMeasfromiec104(QByteArray)), JourD, SLOT(FillMeasJour(QByteArray)));
            ch104->Parse->DR = &S2Config;
            connect(ch104,SIGNAL(errorCh104(int)), this, SLOT(ReConnect(int)));
         }
         else if(insl.at(1) == "MODBUS")
         {
            connect(modBus, SIGNAL(ErrorRead()), CorD, SLOT(ErrorRead()));
            connect(modBus, SIGNAL(ModbusState(ModBus::ModbusDeviceState)), CheckB, SLOT(onModbusStateChanged(ModBus::ModbusDeviceState)));
            connect(modBus, SIGNAL(SignalsReceived(ModBusSignal*, int)), CheckB, SLOT(UpdateModBusData(ModBusSignal*, int)));
            connect(modBus, SIGNAL(CorSignalsReceived(ModBusSignal*, int)), CorD, SLOT(ModBusUpdateCorData(ModBusSignal*, int)));
            connect(CorD, SIGNAL(RS485WriteCorBd(ModBus::Information*, float*)), modBus, SLOT(ModWriteCor(ModBus::Information*, float*)));//, int*)));
            connect(CorD, SIGNAL(RS485ReadCorBd(Information &)), modBus, SLOT(ModReadCor(Information &)));
            connect(modBus,SIGNAL(ReconnectSignal()), this, SLOT(ReConnect()));
         }

       }

       if(MainInterface == I_USB)
       {
         CheckB = new CheckDialog84(BoardTypes::BT_BASE, this, nullptr);
       }
         break;
    }

    case Config::MTB_87:
        ConfB = new ConfDialog87(S2Config, true);

        break;

    case Config::MTB_A1:
    {
        ConfB = new ConfDialogA1(S2Config);

        break;
    }
    default:
        break;
    }
    INFOMSG("Mezonin parsing");

    //MTypeM = Config::MTM_84;

    if(MainInterface == I_USB)
    {
       MTypeM = ModuleBSI::GetMType(BoardTypes::BT_MEZONIN);
    }

    switch(MTypeM)
    {
    case Config::MTM_21:
    {
        ConfM = new ConfDialog21(S2Config, false);

        break;
    }
    case Config::MTM_22:
    {
        ConfM = new ConfDialog22(S2Config, false);

        break;
    }
    case Config::MTM_31:
    {
        INFOMSG("Mezonin 0x31 start");
        ConfM = new ConfDialog31(S2Config, false);
        INFOMSG("Mezonin 0x31 stop");
        break;
    }
    case Config::MTM_35:
    {
        ConfM = new ConfDialog35(S2Config, false);
        break;
    }
    case Config::MTM_81:
    case Config::MTM_82:
    case Config::MTM_83:
    {
        ConfM = new ConfDialog80(S2Config);
        break;
    }
    case Config::MTM_84:
    {
        setMinimumSize(QSize(800, 650));

        if(MainInterface != I_RS485)
        ConfM = new ConfDialog84(S2Config);

        if(ch104 != nullptr && MainInterface == I_ETHERNET)
        {
          connect(ConfM,SIGNAL(ReadConfig(char)), ch104, SLOT(SelectFile(char)));
          connect(ch104,SIGNAL(sendS2fromiec104(QVector<S2::DataRec>*)), ConfM, SLOT(FillConf(QVector<S2::DataRec>*)));
          connect(ConfM,SIGNAL(writeConfFile(QVector<S2::DataRec>*)), ch104, SLOT(FileReady(QVector<S2::DataRec>*)));
          //connect(ch104,SIGNAL(sendMessageiec104()), ConfM, SLOT(Message()));
          connect(ch104,SIGNAL(sendConfMessageOk()), ConfM, SLOT(WriteConfMessageOk()));

        }

        break;
    }
    case Config::MTM_85:
    {
        //setMinimumSize(QSize(1200, 800));
        ConfM = new ConfDialog85(S2Config);

        break;
    }
    default: // 0x00
        break;
    }

 }



