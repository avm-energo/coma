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
#include <QToolBar>
#include <QDialog>
#include <QCursor>
#include <QSplashScreen>
#include <QApplication>
#include <QSettings>
#include <QGroupBox>
#include <QStandardPaths>
#include <QStringListModel>
#include <QTextEdit>
#include "coma.h"
#include "../gen/maindef.h"
#include "../config/confdialogKTF.h"
#include "../config/confdialogKIV.h"
#include "../config/confdialogKDV.h"
#include "../check/checkdialogKTF.h"
#include "../check/checkdialogKIV.h"
#include "../check/checkdialogKDV.h"
#include "../tune/tuneKTF.h"
#include "../tune/tunedialogKIV.h"
#include "../tune/tuneKDV.h"
#include "../dialogs/settingsdialog.h"
#include "../dialogs/keypressdialog.h"
#include "../dialogs/errordialog.h"
#include "../widgets/etabwidget.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "../widgets/waitwidget.h"
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "../gen/logclass.h"
#include "../gen/stdfunc.h"
#include "../gen/files.h"

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
    S2Config = new QVector<S2::DataRec>;
    S2ConfigForTune = new QVector<S2::DataRec>;
    FullName = "";
    Disconnected = true;
    Reconnect = false;
    TimeD = nullptr;
    IDialog = nullptr;
    JourD = nullptr;
    CorD = nullptr;
    MainConfDialog = nullptr;
    ConfB = ConfM = nullptr;
    CheckM = CheckM = nullptr;
    TuneM = nullptr;
    Wpred = Walarm = nullptr;
    CurTabIndex = -1;
    Cancelled = false;
    for (int i = 0; i < 20; ++i)
    {
       PredAlarmEvents[i] = 0;
       AlarmEvents[i] = 0;
    }
    ActiveThreads = 0;
    NewUSB();
    NewTimers();
    LoadSettings();

    splash->finish(this);
}

Coma::~Coma()
{
    Reconnect = false;
    Disconnect();
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

    menu = new QMenu;
    act = new QAction(this);
    act->setText("О программе");
    connect(act,SIGNAL(triggered()),this,SLOT(GetAbout()));
    menubar->addAction(act);

    menubar->addSeparator();

    setMenuBar(menubar);
}

void Coma::StartWork()
{
    if (!Reconnect)
    {
        ShowConnectDialog();
        if(Cancelled)
        {
            ERMSG("Отмена подключения");
            return;
        }
        S2ConfigForTune->clear();
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
    connect(MainTW,SIGNAL(tabClicked(int)),this,SLOT(MainTWTabClicked(int)));

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
            if(ModuleBSI::ModuleTypeString != "" && ModuleBSI::ModuleTypeString != "Отсутствует")
                EMessageBox::information(this, "Успешно", "Связь с "+ModuleBSI::ModuleTypeString+" установлена");
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

    // MTypeB & MTypeM are acquired
    MTypeB <<= 8;

    Disconnected = false;
    Reconnect = true;
    PrepareDialogs();
    str = "Текущие параметры";
    if (CheckM != nullptr)
    {
        CheckM->setMinimumHeight(500);
        MainTW->addTab(CheckM, str);
        CheckIndex = MainTW->indexOf(CheckM);
    }

    if (ConfM != nullptr)
    {
        ConfM->checkIndex = CheckIndex;
        str = (ConfB == nullptr) ? "Конфигурирование" : "Конфигурирование\nМезонин";
        MainTW->addTab(ConfM, str);

        ConfM->confIndex = ConfIndex = MainTW->indexOf(ConfM);

        if(ConfM->IsNeededDefConf)
        {
            ConfM->SetDefConf();
            EMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию");
        }
        connect(ConfM,SIGNAL(NewConfToBeLoaded()),this,SLOT(Fill()));
        connect(ConfM,SIGNAL(DefConfToBeLoaded()),this,SLOT(SetDefConf()));

        connect(ConfM,SIGNAL(ConfReconnectSignal()),this,SLOT(ReConnect()));
        //connect(ConfM, SIGNAL(stopRead(int)), this,SLOT(Stop_BdaTimer(int)));
        //connect(ConfM, SIGNAL(stopRead(int)), this,SLOT(Stop_TimeTimer(int)));
    }

    /*if (ConfB != nullptr)
    {
        str = (ConfM == nullptr) ? "Конфигурирование" : "Конфигурирование\nБазовая";
        MainTW->addTab(ConfB, str);
        if(ConfB->IsNeededDefConf)
        {
            ConfB->SetDefConf();
            EMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию");
        }
        connect(ConfB,SIGNAL(NewConfToBeLoaded()),this,SLOT(Fill()));
        connect(ConfB,SIGNAL(DefConfToBeLoaded()),this,SLOT(SetDefConf()));
    }*/
    if (TuneM != nullptr)
    {
        MainTW->addTab(TuneM, "Регулировка");
        TuneM->TuneIndex = MainTW->indexOf(TuneM);
        TuneM->CheckIndex = CheckM->checkIndex;
        TuneM->setMinimumHeight(500);
       // connect(TuneM, SIGNAL(stopRead(int)), this,SLOT(Stop_BdaTimer(int)));
       // connect(TuneM, SIGNAL(stopRead(int)), this,SLOT(Stop_TimeTimer(int)));
        connect(MainTW, SIGNAL(tabClicked(int)), TuneM, SLOT(TuneReadCoefs(int))); //tabClicked
        connect(MainTW, SIGNAL(tabClicked(int)), TuneM, SLOT(TuneMode(int))); //tabClicked
        connect(TuneM,SIGNAL(LoadDefConf()),this,SLOT(SetDefConf()));
    }
    if (TimeD != nullptr)
    {
        MainTW->addTab(TimeD, "Время");

        if (ConfM != nullptr)
        ConfM->timeIndex = TimeIndex = TimeD->timeIndex = MainTW->indexOf(TimeD);


        if(TuneM != nullptr)
        TuneM->TimeIndex = TimeD->timeIndex;

        connect(TimeTimer,SIGNAL(timeout()),TimeD,SLOT(slot2_timeOut()));
    }

    if (CorD != nullptr)
    {
        MainTW->addTab(CorD, "Начальные значения");
        CorD->corDIndex = MainTW->indexOf(CorD);
    }

    MainTW->addTab(JourD, "Журналы");

    if (ModuleBSI::Health() & HTH_CONFIG) // нет конфигурации
        Error::ShowErMsg(ER_NOCONF);
    if (ModuleBSI::Health() & HTH_REGPARS) // нет коэффициентов
        Error::ShowErMsg(ER_NOTUNECOEF);

     FwUpD = new fwupdialog;
     MainTW->addTab(FwUpD, "Загрузка ВПО");


    MainTW->addTab(IDialog, "О приборе");
    IDialog->FillBsi();

    MainTW->repaint();
    MainTW->show();

    INFOMSG("MainTW created");

    BdaTimer->start();

    SetSlideWidget();
}

void Coma::PrepareDialogs()
{
    IDialog = new InfoDialog;
    JourD = new JournalDialog;

    TimeD = new MNKTime;
    CorD = new CorDialog;
    S2Config->clear();
    S2ConfigForTune->clear();

    switch(MTypeB)
    {
    case Config::MTB_A3:
       CheckM = new CheckDialogKDV(BoardTypes::BT_BASE);
       ConfM = new ConfDialogKDV(S2Config);
       TuneM = new TuneDialogKDV(S2ConfigForTune);
        break;
    };
    switch(MTypeM)
    {

    case Config::MTM_84:
       CheckM = new CheckDialogKIV(BoardTypes::BT_BASE);
       ConfM = new ConfDialogKIV(S2Config);
       TuneM = new TuneDialogKIV(S2ConfigForTune);
        break;
       \
    case Config::MTM_87:
       CheckM = new CheckDialogKTF(BoardTypes::BT_BASE);
       ConfM = new ConfDialogKTF(S2Config);
       TuneM = new TuneDialogKTF(S2ConfigForTune);
        break;
    };

    if(IDialog != nullptr)
    {
      connect(this,SIGNAL(ClearBsi()),IDialog,SLOT(ClearBsi()));
    }

    if(CheckM != nullptr)
    {
      connect(this, SIGNAL(SetPredAlarmColor(quint8*)), CheckM,SLOT(SetPredAlarmColor(quint8*)));
      connect(this, SIGNAL(SetAlarmColor(quint8*)), CheckM,SLOT(SetAlarmColor(quint8*)));
    }



}


void Coma::NewUSB()
{
    cn = new EUsbHid;
    //connect(this, SIGNAL(StopCommunications()), cn, SLOT(Disconnect()));
    connect(cn,SIGNAL(Finished()),this,SLOT(USBFinished()));
    connect(cn,SIGNAL(readbytessignal(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
    connect(cn,SIGNAL(writebytessignal(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
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
    connect(BdaTimer,SIGNAL(timeout()),this,SLOT(UpdateUSB()));
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
        StdFunc::SetEmulated(true);
    SetupUI();
    show();
}

void Coma::ReConnect()
{
    QDialog *dlg = new QDialog;

    INFOMSG("Reconnect()");
    Reconnect = true;
    TimeTimer->stop();
    if(!Disconnected)
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
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");  // color: rgba(220,220,220,255);
    hlyout->addWidget(WDFunc::NewLBLT(w, "Связь разорвана.\nПопытка переподключения будет выполнена через 3 секунды", "", "", ""), 1);
    vlayout->addLayout(hlyout);
    w->setLayout(vlayout);
    connect(ReconnectTimer,SIGNAL(timeout()), dlg,SLOT(close()));
    lyout->addWidget(w);
    dlg->setLayout(lyout);
    ReconnectTimer->start();
    dlg->show();
    StdFunc::Wait(RECONNECTINTERVAL);
    dlg->close();
}

void Coma::AttemptToRec()
{
    if(Reconnect != false)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        SaveSettings();
        QApplication::restoreOverrideCursor();
        StartWork();
    }
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
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");  // color: rgba(220,220,220,255);
    hlyout->addWidget(WDFunc::NewLBLT(w, "Связь с "+ModuleBSI::ModuleTypeString+" установлена", "", "", ""), 1);
    vlayout->addLayout(hlyout);
    w->setLayout(vlayout);
    lyout->addWidget(w);
    dlg->setLayout(lyout);
    dlg->show();
    StdFunc::Wait(WAITINTERVAL);
    dlg->close();
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
    QWidget *w = new QWidget;
    QStringList Discription =  QStringList() << "Состояние устройства" << "Предупредительная сигнализация" << "Аварийная сигнализация";
    w->setStyleSheet("QComa {background-color: "+QString(MAINWINCLR)+";}");
    QPixmap *pmgrn = new QPixmap("images/greenc.png");

    QPushButton *pb = new QPushButton("Состояние устройства");
    pb->setMinimumSize(QSize(230,30));
    connect(pb,SIGNAL(clicked()),this,SLOT(DeviceState()));
    QGroupBox *gb = new QGroupBox("");

    hlyout->addWidget(pb,Qt::AlignRight);
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(950), pmgrn), 1);
    gb->setLayout(hlyout);
    hlyout2->addWidget(gb);
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

    INFOMSG("DeviceState()");
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
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
    const QStringList events = QStringList() << "Отсутствует сигнал напряжения фазы A                   "
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
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", "3033", (PredAlarmEvents[16]) ? pmred : pmgrn));
    hlyout->addWidget(WDFunc::NewLBLT(w, events.at(16), "", "", ""), 1);
    vlayout->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", "3034", (PredAlarmEvents[17]) ? pmred : pmgrn));
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

    INFOMSG("AlarmState()");
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
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3024+i), (AlarmEvents[i]) ? pmred : pmgrn));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    for (int i = 0; i < 3; ++i)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3030+i), (AlarmEvents[3+i]) ? pmred : pmgrn));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(3+i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", "3035", (AlarmEvents[6]) ? pmred : pmgrn));
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
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE != nullptr)
        MainTE->clear();
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

   ReceiveTimer->stop();
   //if(fileSize != curfileSize && MainInterface != I_USB)
   //    EMessageBox::information(this, "Ошибка", "Ошибка");
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

void Coma::ShowConnectDialog()
{
    QByteArray ba;
    QString Str;

    if(Cancelled)
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
    l2yout->addWidget(WDFunc::NewLBL(this, "2015-2020 гг."));
    l2yout->addWidget(WDFunc::NewLBL(this, "info@avmenergo.ru"));
    l2yout->addStretch(10);
    hlyout->addWidget(WDFunc::NewLBL(this, "", "", "", new QPixmap("images/avm-energo.png")), 1, Qt::AlignVCenter);
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
    INFOMSG("Disconnect()");
    if (!StdFunc::IsInEmulateMode())
    {
            BdaTimer->stop();
            TimeFunc::Wait(100);
            if(cn->Connected)
            cn->Disconnect();

        while (ActiveThreads) // wait for all threads to finish
            QCoreApplication::processEvents();
        Disconnected = true;
    }
}

void Coma::DisconnectAndClear()
{
    INFOMSG("DisconnectAndClear()");
    TimeTimer->stop();
    if(!Disconnected)
    {
        Disconnect();
        TimeD = nullptr;
        CheckM = CheckM = nullptr;
        MainConfDialog = nullptr;
        ConfB = ConfM = nullptr;
        CheckM = CheckM = nullptr;
        Wpred = Walarm = nullptr;
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
        if(Reconnect)
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

    if(CheckM != nullptr)
    {
        if(tabindex == CheckIndex)
            BdaTimer->start();
        else
            BdaTimer->stop();
    }
    if(TimeD != nullptr)
    {
        if(tabindex == TimeIndex)
            TimeTimer->start();
        else
            TimeTimer->stop();
    }
    else
        TimeTimer->stop();

    CurTabIndex = tabindex;
    if (ConfM != nullptr)
    {
       if(tabindex == ConfIndex)
       ConfM->ReadConf(tabindex);
    }
    if (CorD != nullptr)
        CorD->GetCorBd(tabindex);
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
    if(ConfM != nullptr)
    ConfM->TheEnd = 1;
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

void Coma::UpdateUSB()
{
    int i = 0, predalarmcount = 0, alarmcount = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    Bd11 Bd_block11;

    INFOMSG("UpdateUSB()");
    if (CheckM != nullptr)
       CheckM->USBUpdate();

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
        WDFunc::SetLBLImage(this, "951", (predalarmcount == 0) ? pmgrn : pmred);
        WDFunc::SetLBLImage(this, "952", (alarmcount == 0) ? pmgrn : pmred);
        emit SetPredAlarmColor(PredAlarmEvents);
        emit SetAlarmColor(AlarmEvents);
    }
}

void Coma::USBSetAlarms()
{
    int i = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    Bd11 Bd_block11;

    INFOMSG("USBSetAlarms()");
    if (Commands::GetBd(11, &Bd_block11, sizeof(Bd11)) == NOERROR)
    {
        if(Wpred != nullptr)
        {
            for(i=0; i<13; i++)
            {
                quint32 alarm = Bd_block11.predAlarm & ((quint32)0x00000001 << i);
                WDFunc::SetLBLImage(Walarm, (QString::number(3011+i)), (alarm) ? pmred : pmgrn);
            }
            for(i=13; i<16; i++)
            {
                quint32 alarm = Bd_block11.predAlarm & ((quint32)0x00000001 << i);
                WDFunc::SetLBLImage(Walarm, (QString::number(3027+i-13)), (alarm) ? pmred : pmgrn);
            }
            quint32 alarm = Bd_block11.predAlarm & ((quint32)0x00000001 << 16);
            WDFunc::SetLBLImage(Walarm, "3033", (alarm) ? pmred : pmgrn);
            alarm = Bd_block11.predAlarm & ((quint32)0x00000001 << 17);
            WDFunc::SetLBLImage(Walarm, "3034", (alarm) ? pmred : pmgrn);
        }
        if(Walarm != nullptr)
        {
            for(i=0; i<3; i++)
            {
                quint32 alarm = Bd_block11.alarm & ((quint32)0x00000001 << i);
                WDFunc::SetLBLImage(Walarm, (QString::number(3024+i)), (alarm) ? pmred : pmgrn);
            }

            for(i=3; i<6; i++)
            {
                quint32 alarm = Bd_block11.alarm & ((quint32)0x00000001 << i);
                WDFunc::SetLBLImage(Walarm, (QString::number(3030+i-3)), (alarm) ? pmred : pmgrn);
            }

            quint32 alarm = Bd_block11.alarm & ((quint32)0x00000001 << 6);
            if(Bd_block11.alarm & ((quint32)0x00000001 << 6))
                WDFunc::SetLBLImage(Walarm, "3035", (alarm) ? pmred : pmgrn);
        }
    }
}

void Coma::SetSlideWidget()
{
    QWidget *SlideWidget = new QWidget(this);
    SlideWidget->setWindowFlags(Qt::FramelessWindowHint);
    SlideWidget->setObjectName("slidew");
    SlideWidget->setStyleSheet("QWidget {background-color: rgba(110,234,145,255);}");
    QVBoxLayout *slyout = new QVBoxLayout;
    QCheckBox *chb = WDFunc::NewChB(this, "teenablechb", "Включить протокол");
    connect(chb,SIGNAL(toggled(bool)),this,SLOT(SetTEEnabled(bool)));
    slyout->addWidget(chb, 0, Qt::AlignLeft);
    QTextEdit *MainTE = new QTextEdit;
    MainTE->setObjectName("mainte");
    MainTE->setReadOnly(true);
    MainTE->setUndoRedoEnabled(false);
    MainTE->setWordWrapMode(QTextOption::WrapAnywhere);
    MainTE->document()->setMaximumBlockCount(C_TE_MAXSIZE);
    slyout->addWidget(MainTE, 40);
    SlideWidget->setLayout(slyout);
    SlideWidget->setMinimumWidth(250);
    SlideWidget->hide();
    SWGeometry = SlideWidget->geometry();
    SWHide = true;
}

void Coma::UpdateMainTE(QByteArray ba)
{
    if (!TEEnabled)
        return;
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE != nullptr)
        MainTE->append(ba.toHex());
}

void Coma::SetTEEnabled(bool enabled)
{
    TEEnabled = enabled;
}
