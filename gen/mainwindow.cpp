#include <QHBoxLayout>
#include <QTextEdit>
//#include <QPushButton>
#include <QDir>
#include <QMenu>
#include <QApplication>
#include <QTimer>
#include <QMenuBar>
#include <QEventLoop>
#include <QScrollBar>
#include <QProgressBar>
#include <QSettings>
#include <QGroupBox>
#include <QCursor>
#include <QStringListModel>
#include <QStandardPaths>
#include <QPropertyAnimation>
#include <QtSerialPort/QSerialPortInfo>
#include <QFileDialog>
#include "mainwindow.h"
#include "../widgets/wd_func.h"
#include "../widgets/etabwidget.h"
#include "../widgets/emessagebox.h"
#include "../dialogs/errordialog.h"
#include "../dialogs/settingsdialog.h"
#include "../dialogs/keypressdialog.h"
#include "../gen/error.h"
#include "../gen/colors.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "../widgets/etablemodel.h"
#include "../widgets/etableview.h"
#include "../gen/timefunc.h"
#include "modulebsi.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "commands.h"
#endif

int MainInterface;
quint32 MainWindow::MTypeB;
quint32 MainWindow::MTypeM;
int MainWindow::TheEnd;
int MainWindow::StopRead;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QPixmap StartWindowSplashPixmap("images/2.1.x.png");
    StartWindowSplashScreen = new QSplashScreen(StartWindowSplashPixmap);
    StartWindowSplashScreen->show();
    StartWindowSplashScreen->showMessage("Подготовка окружения...", Qt::AlignRight, Qt::white);
    // http://stackoverflow.com/questions/2241808/checking-if-a-folder-exists-and-creating-folders-in-qt-c
    QDir dir(StdFunc::GetHomeDir());
    if (!dir.exists())
        dir.mkpath(".");
    StdFunc::Init();
    Error::Init();
    S2Config.clear();
    S2ConfigForTune.clear();
    MainConfDialog = nullptr;
    ConfB = ConfM = nullptr;
    CheckB = CheckM = nullptr;
    Wpred = Walarm = nullptr;
    Ch104 = new IEC104;
    connect(this,SIGNAL(StopCommunications()),Ch104,SLOT(Stop()));
    connect(Ch104,SIGNAL(sponsignalWithTimereceived(Parse104::SponSignalsWithTime*)), this, SLOT(UpdatePredAlarmEvents(Parse104::SponSignalsWithTime*)));
    ChModbus = new ModBus;
    connect(this,SIGNAL(StopCommunications()),ChModbus,SLOT(Finish()));
    connect(ChModbus,SIGNAL(CoilSignalsReady(ModBus::Coils)), this, SLOT(ModBusUpdatePredAlarmEvents(ModBus::Coils)));
    cn = new EUsbHid;
    connect(this, SIGNAL(StopCommunications()), cn, SLOT(Disconnect()));
    FullName = "";
    Reconnect = false;

    TimeTimer = new QTimer;
    TimeTimer->setInterval(1000);

    BdaTimer = new QTimer;
    BdaTimer->setInterval(ANMEASINT);
    connect(BdaTimer,SIGNAL(timeout()), this, SLOT(GetUSBAlarmInDialog()));

    ReceiveTimer = new QTimer;
    ReceiveTimer->setInterval(ANMEASINT);
    connect(ReceiveTimer, SIGNAL(timeout()), this,SLOT(FileTimeOut()));

    ReconnectTimer = new QTimer;
    ReconnectTimer->setInterval(RECONNECTINTERVAL);
    connect(ReconnectTimer,SIGNAL(timeout()), this, SLOT(attemptToRec()));

    for (int i = 0; i < 20; ++i)
    {
       PredAlarmEvents[i] = 0;
       AlarmEvents[i] = 0;
    }
    TheEnd = 0;

    PrepareTimers();
    LoadSettings();

    StartWindowSplashScreen->finish(this);

}

MainWindow::~MainWindow()
{
    //DisconnectAndClear();
#if PROGSIZE != PROGSIZE_EMUL
    Disconnect();
#endif
}

void MainWindow::SetMode(int mode)
{
    Mode = mode;
}

int MainWindow::GetMode()
{
    return Mode;
}

void MainWindow::Go(const QString &parameter)
{
    Q_UNUSED(parameter)
    if (Mode != COMA_GENERALMODE)
    {
        StdFunc::SetEmulated(true);
        Autonomous = true;
    }
    SetupUI();
    show();

}

void MainWindow::ReConnect()
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
    #if PROGSIZE != PROGSIZE_EMUL
        Disconnect();
    #endif

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

void MainWindow::attemptToRec()
{
    ReconnectTimer->stop();

    //if(MainInterface->size() != 0)
    //{
        if(MainInterface == I_USB)
        {

           Disconnected = 0;

               //#ifdef USBENABLE
               //connect(cn,SIGNAL(Retry()),this,SLOT(ShowConnectDialog()));
               //#else
               //#ifdef COMPORTENABLE
               //    cn = new EUsbCom;
               //    connect(cn,SIGNAL(Retry()),this,SLOT(ShowConnectDialog()));
               //#endif
               //#endif
               /*connect(cn,SIGNAL(SetDataSize(int)),this,SLOT(SetProgressBar1Size(int)));
               connect(cn,SIGNAL(SetDataCount(int)),this,SLOT(SetProgressBar1(int)));
               connect(cn,SIGNAL(readbytessignal(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
               connect(cn,SIGNAL(writebytessignal(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
               connect(cn, SIGNAL(ShowError(QString)), this, SLOT(ShowErrorMessageBox(QString)));
               connect(this,SIGNAL(Retry()),this,SLOT(Stage1_5()));
               connect(cn,SIGNAL(ReconnectSignal()),this,SLOT(ReConnect()));*/

              //cn->DevicesFound();
              SetPortSlot(SavePort);

              if(cancel)
              {
                  ERMSG("Отмена");
                  return;
              }

              StopRead = 0;

                 insl.clear();

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
          #if PROGSIZE >= PROGSIZE_LARGE
                  else if (res == NOERROR)
                  {
                    if(ModuleBSI::ModuleTypeString != "")
                    ConnectMessage();
                  }
          #endif

         }
    //}


    if(Reconnect != false)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        S2Config.clear();
        SaveSettings();
        QApplication::restoreOverrideCursor();
        StopRead = 0;
        Stage3();
    }
}

void MainWindow::ConnectMessage()
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

    //hlyout->addLayout(l2yout,100);
    lyout->addWidget(w);
    dlg->setLayout(lyout);

    dlg->show();
    StdFunc::Wait(WAITINTERVAL);
    dlg->close();
}

/*void MainWindow::stopTimer()
{
    connectTimer->stop();
    connectTimer->deleteLater();
}*/

QWidget *MainWindow::HthWidget()
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

QWidget *MainWindow::ReleWidget()
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
    w->setStyleSheet("QMainWindow {background-color: "+QString(MAINWINCLR)+";}");
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

void MainWindow::DeviceState()
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

void MainWindow::PredAlarmState()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    QString tmps = QString(PROGCAPTION);

    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    //QPixmap *pm[2] = {pmred, pmgrn};
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

        if(PredAlarmEvents[i])
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3011+i), pmred));
        else
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3011+i), pmgrn));

        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    for (int i = 0; i < 3; ++i)
    {
        hlyout = new QHBoxLayout;

        if(PredAlarmEvents[13+i])
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3027+i), pmred));
        else
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3027+i), pmgrn));

        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(13+i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    hlyout = new QHBoxLayout;
    if(PredAlarmEvents[16])
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3033), pmred));
    else
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3033), pmgrn));

    hlyout->addWidget(WDFunc::NewLBLT(w, events.at(16), "", "", ""), 1);
    vlayout->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    if(PredAlarmEvents[17])
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3034), pmred));
    else
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3034), pmgrn));

    hlyout->addWidget(WDFunc::NewLBLT(w, events.at(17), "", "", ""), 1);
    vlayout->addLayout(hlyout);

    w->setLayout(vlayout);

    if(MainInterface == I_ETHERNET && Ch104 != nullptr)
    connect(Ch104,SIGNAL(sponsignalWithTimereceived(Parse104::SponSignalsWithTime*)), this, SLOT(UpdatePredAlarmEvents(Parse104::SponSignalsWithTime*)));

    if(MainInterface == I_USB)
    connect(BdaTimer,SIGNAL(timeout()), this, SLOT(GetUSBAlarmInDialog()));

    if(MainInterface == I_RS485 && ChModbus != nullptr)
    connect(ChModbus,SIGNAL(coilsignalsready(Coils*)), this, SLOT(ModBusUpdatePredAlarmEvents(Coils*)));

    //hlyout->addLayout(l2yout,100);
    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb,0);
    dlg->setLayout(lyout);
    dlg->show();
}


void MainWindow::AlarmState()
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

    if(MainInterface ==  I_ETHERNET && Ch104 != nullptr)
    connect(Ch104,SIGNAL(sponsignalWithTimereceived(Parse104::SponSignalsWithTime*)), this, SLOT(UpdatePredAlarmEvents(Parse104::SponSignalsWithTime*)));

    if(MainInterface == I_USB)
    connect(BdaTimer,SIGNAL(timeout()), this, SLOT(GetUSBAlarmInDialog()));

    if(MainInterface == I_RS485 && ChModbus != nullptr)
    connect(ChModbus,SIGNAL(coilsignalsready(Coils*)), this, SLOT(ModBusUpdatePredAlarmEvents(Coils*)));

    //hlyout->addLayout(l2yout,100);
    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb,0);
    dlg->setLayout(lyout);

    
            dlg->show();
}


void MainWindow::UpdateReleWidget(Parse104::SponSignals104* Signal)
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

void MainWindow::UpdatePredAlarmEvents(Parse104::SponSignalsWithTime* Signal)
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


void MainWindow::UpdateStatePredAlarmEvents(Parse104::SponSignals104 *Signal)
{
    int i = 0, PredArarmcount = 0, Ararmcount = 0;
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

void MainWindow::UpdateStatePredAlarmEventsWithTime(Parse104::SponSignalsWithTime *Signal)
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

void MainWindow::ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)
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

void MainWindow::ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)
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

QWidget *MainWindow::Least()
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

#if PROGSIZE >= PROGSIZE_LARGE
void MainWindow::SetSlideWidget()
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
#endif

void MainWindow::SetupMenubar()
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
    if (!Autonomous)
    {
        act = new QAction(this);
        act->setText("Соединение");
        act->setIcon(QIcon("images/play.png"));
        connect(act,SIGNAL(triggered()),this,SLOT(Stage1_5()));
        menu->addAction(act);
        act = new QAction(this);
        act->setText("Разрыв соединения");
        act->setIcon(QIcon("images/stop.png"));
        connect(act,SIGNAL(triggered()),this,SLOT(DisconnectAndClear()));
        menu->addAction(act);
    }
    menubar->addMenu(menu);
#if PROGSIZE >= PROGSIZE_LARGE
   /* menu = new QMenu;
    menu->setTitle("Секретные операции");
    act = new QAction(this);
    act->setText("Работа с Hidden Block");
    connect(act,SIGNAL(triggered()),this,SLOT(OpenBhbDialog()));
    menu->addAction(act);
    menubar->addMenu(menu);*/
#endif

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
    //AddActionsToMenuBar(menubar);
}

#if PROGSIZE >= PROGSIZE_LARGE
void MainWindow::PrepareTimers()
{
    QTimer *MouseTimer = new QTimer;
    connect(MouseTimer,SIGNAL(timeout()),this,SLOT(MouseMove()));
    MouseTimer->start(50);
}
#endif

void MainWindow::LoadSettings()
{
    QString HomeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/"+PROGNAME+"/";
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    StdFunc::SetHomeDir(sets->value("Homedir", HomeDir).toString());

    TEEnabled = sets->value("TEEnabled", "0").toBool();
}

void MainWindow::SaveSettings()
{
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    sets->setValue("Homedir", StdFunc::GetHomeDir());

    sets->setValue("TEEnabled", TEEnabled);
}

void MainWindow::ClearTW()
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
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE != nullptr)
        MainTE->clear();
}

#if PROGSIZE >= PROGSIZE_LARGE
void MainWindow::ShowOrHideSlideSW()
{
    QWidget *w = this->findChild<QWidget *>("slidew");
    if (w == nullptr)
    {
      ERMSG("Пустой виджет");
      return;
    }
    if (w->isHidden())
        w->show();
    if (SWHide)
        w->setGeometry(SWGeometry);
    QPropertyAnimation *ani = new QPropertyAnimation(w, "geometry");
    ani->setDuration(500);
    QRect startRect(width(), 30, 0, height()-30);
    QRect endRect(width() - w->width(), 30, w->width(), height()-30);
    if (SWHide)
    {
        ani->setStartValue(startRect);
        ani->setEndValue(endRect);
    }
    else
    {
        ani->setStartValue(endRect);
        ani->setEndValue(startRect);
    }
    ani->start();
    SWHide = !SWHide;
}
#endif
#if PROGSIZE != PROGSIZE_EMUL
int MainWindow::CheckPassword()
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
#endif


#if PROGSIZE != PROGSIZE_EMUL
void MainWindow::Stage1_5()
{
    Reconnect = false;
    Disconnected = 0;
    ShowInterfaceDialog();
    ShowConnectDialog();

    if(cancel)
    {
        ERMSG("Отмена подключения");
        return;
    }

    StopRead = 0;

    if((insl.size() == 0) && ((MainInterface == I_ETHERNET) || (MainInterface == I_RS485)))
    {
        DisconnectAndClear();
        ERMSG("Разрыв связи");
        return;
    }

    if((MainInterface == I_ETHERNET && insl.at(1) != "ETH") ||
       (MainInterface == I_RS485 && insl.at(1) != "MODBUS"))
    {
       EMessageBox::information(this, "Ошибка", "Выбранный интерфейс не соотвествует выбранной строке в файле");
       DisconnectAndClear();
       insl.clear();
       ERMSG("Выбранный интерфейс не соотвествует выбранной строке в файле");
       return;
    }


    if (MainInterface == I_USB)
    {
       insl.clear();

       if(cn->Cancelled)
       return;

       if (Commands::Connect() != NOERROR) // cn->Connect()
       {
          EMessageBox::error(this, "Ошибка", "Не удалось установить связь");
          QApplication::restoreOverrideCursor();
          ERMSG("Не удалось установить связь");
          return;
       }
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    S2ConfigForTune.clear();
    S2Config.clear();
    SaveSettings();
    QApplication::restoreOverrideCursor();
    Stage2();
}

void MainWindow::Stage2()
{
        //if(interface == "Ethernet и RS485")

    if (MainInterface == I_USB)
    {
        int res = ModuleBSI::SetupBSI();
        if (res == GENERALERROR)
        {
            if (EMessageBox::question(this, "Ошибка", \
                                      "Не удалось установить связь.\nПовторить подключение?") == 1) // Yes
            {
                cn->Disconnect();
                emit Retry();
            }
            ERMSG("Ошибка чтения BSI");
            return;
        }
#if PROGSIZE >= PROGSIZE_LARGE
        else if (res == NOERROR)
        {
          if(ModuleBSI::ModuleTypeString != "")
          EMessageBox::information(this, "Успешно", "Связь с "+ModuleBSI::ModuleTypeString+" установлена");
        }
        /*else if (res == Error::ER_RESEMPTY)
        {
            if (OpenBhbDialog() != NOERROR)
            {
                EMessageBox::error(this, "Ошибка", "Ошибка при работе с Hidden block");
                return;
            }
        }*/

#endif
    }
    //else if(interface == "Ethernet и RS485" && ch104 != nullptr)
    //emit ch104->stopall();

    Stage3();
}

void MainWindow::UpdateHthWidget(ModuleBSI::Bsi* bsi)
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
#endif

void MainWindow::SetMainDefConf()
{
    if (MainConfDialog != nullptr)
        MainConfDialog->SetDefConf();
}

void MainWindow::SetBDefConf()
{
    if (ConfB != nullptr)
        ConfB->SetDefConf();
}

void MainWindow::SetMDefConf()
{
    if (ConfM != nullptr)
        ConfM->SetDefConf();
}

void MainWindow::Fill()
{
        if (MainConfDialog != nullptr)
        MainConfDialog->Fill();
    if (ConfB != nullptr)
        ConfB->Fill();
    if (ConfM != nullptr)
        ConfM->Fill();
}

#if PROGSIZE >= PROGSIZE_LARGE
void MainWindow::UpdateMainTE(QByteArray ba)
{
    if (!TEEnabled)
    {
        ERMSG("Ошибка TE");
        return;
    }
    QTextEdit *MainTE = this->findChild<QTextEdit *>("mainte");
    if (MainTE != nullptr)
        MainTE->append(ba.toHex());
}

void MainWindow::SetTEEnabled(bool enabled)
{
    TEEnabled = enabled;
}
#endif

void MainWindow::PasswordCheck(QString psw)
{
    if (psw == "se/7520a")
        Ok = true;
    else
        Ok = false;
    emit PasswordChecked();
}

#if PROGSIZE >= PROGSIZE_LARGE
/*int MainWindow::OpenBhbDialog()
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
#endif

#if PROGSIZE >= PROGSIZE_LARGE || PROGSIZE == PROGSIZE_EMUL
void MainWindow::StartEmul()
{
    bool ok;
    quint16 MType = sender()->objectName().toUShort(&ok, 16);
    if (!ok)
    {
        ERMSG("Wrong object name in StartEmul()");
        return;
    }
    ModuleBSI::Bsi bsi;
    bsi.MTypeB = (MType & 0xFF00) >> 8;
    bsi.MTypeM = MType & 0x00FF;
#if PROGSIZE != PROGSIZE_EMUL
   // if (cn->Connected)                             !!!!
   //     DisconnectAndClear();
#endif
    bsi.SerialNum = 0x12345678;
    bsi.Hth = 0x00;
    ModuleBSI::SetupEmulatedBsi(bsi);
    StdFunc::SetEmulated(true);
    Stage3();
}
#endif

void MainWindow::StartSettingsDialog()
{
    SettingsDialog *dlg = new SettingsDialog;
    dlg->show();
    SaveSettings();
}

void MainWindow::ShowErrorDialog()
{
    ErrorDialog *dlg = new ErrorDialog;
    dlg->show();
}
#if PROGSIZE != PROGSIZE_EMUL
void MainWindow::SetProgressBar1Size(int size)
{
    fileSize = size;
    SetProgressBarSize("1", size);
}

void MainWindow::SetProgressBar1(int cursize)
{
    curfileSize = cursize;
    ReceiveTimer->stop();
    ReceiveTimer->setInterval(5000);
    SetProgressBar("1", cursize);
    ReceiveTimer->start();
}

void MainWindow::FileTimeOut()
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

void MainWindow::SetProgressBar2Size(int size)
{
    SetProgressBarSize("2", size);
}

void MainWindow::SetProgressBar2(int cursize)
{
    SetProgressBar("2", cursize);
}

void MainWindow::ShowInterfaceDialog()
{
    QByteArray ba;
    //int res, i;
    QDialog *dlg = new QDialog(this);
    QString Str;
    cancel = false;
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

void MainWindow::SetCancelled()
{
   cancel = true;
}

void MainWindow::ShowConnectDialog()
{
    QByteArray ba;
    int res;
    QDialog *dlg = new QDialog(this);
    QString Str;
    //QStringList device = QStringList() << "KDV" << "2" << "1" << "2";
    //QStringList inter = QStringList() << "ETH" << "MODBUS";
    if(cancel)
    {
        ERMSG("Отмена ConnectDialog");
        return;
    }

    dlg->setMinimumWidth(150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("connectdlg");
    QVBoxLayout *lyout = new QVBoxLayout;   
    QStringListModel *tmpmodel = new QStringListModel;

    if(MainInterface == I_USB)
    {
        //#ifdef USBENABLE
        connect(cn,SIGNAL(Retry()),this,SLOT(ShowConnectDialog()));
        //#else
        //#ifdef COMPORTENABLE
        //    cn = new EUsbCom;
        //    connect(cn,SIGNAL(Retry()),this,SLOT(ShowConnectDialog()));
        //#endif
        //#endif
        connect(cn,SIGNAL(SetDataSize(int)),this,SLOT(SetProgressBar1Size(int)));
        connect(cn,SIGNAL(SetDataCount(int)),this,SLOT(SetProgressBar1(int)));
        connect(cn,SIGNAL(readbytessignal(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
        connect(cn,SIGNAL(writebytessignal(QByteArray)),this,SLOT(UpdateMainTE(QByteArray)));
        connect(cn, SIGNAL(ShowError(QString)), this, SLOT(ShowErrorMessageBox(QString)));
        connect(this,SIGNAL(Retry()),this,SLOT(Stage1_5()));
        connect(cn,SIGNAL(ReconnectSignal()),this,SLOT(ReConnect()));

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
//         else if (MainInterface == "Ethernet" || MainInterface == "RS485")
    else
    {

       /*tmpmodel->setStringList(device);
       QComboBox *portscb = new QComboBox;
       connect(portscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SDevice(QString)));
       portscb->setModel(tmpmodel);
       lyout->addWidget(portscb);
       QHBoxLayout *hlyout = new QHBoxLayout;
       QPushButton *pb = new QPushButton("Далее");
       connect(pb, SIGNAL(clicked(bool)),dlg,SLOT(close()));
       hlyout->addWidget(pb);
       pb = new QPushButton("Отмена");
       //connect(pb, SIGNAL(clicked(bool)),cn,SLOT(SetCancelled()));         !!!
       connect(pb, SIGNAL(clicked(bool)),dlg, SLOT(close()));
       hlyout->addWidget(pb);
       lyout->addLayout(hlyout);
       dlg->setLayout(lyout);
       dlg->exec();

       dlg = new QDialog(this);

       tmpmodel = new QStringListModel;
       tmpmodel->setStringList(inter);
       portscb = new QComboBox;
       lyout = new QVBoxLayout;
       connect(portscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SaveInterface(QString)));
       portscb->setModel(tmpmodel);
       lyout->addWidget(portscb);
       hlyout = new QHBoxLayout;
       pb = new QPushButton("Далее");
       connect(pb, SIGNAL(clicked(bool)),dlg,SLOT(close()));
       hlyout->addWidget(pb);
       pb = new QPushButton("Отмена");
       //connect(pb, SIGNAL(clicked(bool)),cn,SLOT(SetCancelled()));         !!!
       connect(pb, SIGNAL(clicked(bool)),dlg, SLOT(close()));
       hlyout->addWidget(pb);
       lyout->addLayout(hlyout);
       dlg->setLayout(lyout);
       dlg->exec();*/


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
    if(MainInterface == I_USB)
        connect(pb, SIGNAL(clicked(bool)),cn,SLOT(SetCancelled()));
    connect(pb, SIGNAL(clicked(bool)),dlg, SLOT(close()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    dlg->exec();
}

void MainWindow::SetProgressBarSize(QString prbnum, int size)
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

void MainWindow::SetProgressBar(QString prbnum, int cursize)
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
#endif

void MainWindow::GetAbout()
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
#if PROGSIZE != PROGSIZE_EMUL
void MainWindow::Disconnect()
{
    //emit stoptime();

    if (!StdFunc::IsInEmulateMode())
    {
        if(MainInterface == I_USB)
        {
            if(BdaTimer != nullptr)
         {
            BdaTimer->stop();
            TimeFunc::Wait(100);
         }
            if(cn->Connected)
            cn->Disconnect();
        }
        else
        {
/*            emit stopit();
            emit stopModBus(); */
            emit StopCommunications();
        }

    }
}

void MainWindow::GetDeviceFromTable(QModelIndex idx)
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
#endif
void MainWindow::DisconnectAndClear()
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
    #if PROGSIZE != PROGSIZE_EMUL
        Disconnect();
    #endif

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

        //if(thr != nullptr)
        //emit FinishAll();

        //thr = nullptr;
        //Time = nullptr;

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

#if PROGSIZE >= PROGSIZE_LARGE
void MainWindow::MouseMove()
{
    QPoint curPos = mapFromGlobal(QCursor::pos());
    QWidget *sww = this->findChild<QWidget *>("slidew");
    if (sww == nullptr)
    {
      ERMSG("Пустой sww");
      return;
    }
    if ((abs(curPos.x() - width()) < 10) && (curPos.y() > 0) && (curPos.y() < height()))
    {
        if (SWHide)
            ShowOrHideSlideSW();
    }
    else if ((abs(curPos.x() - width()) > sww->width()) && (curPos.y() > 0) && (curPos.y() < height()))
    {
        if (!SWHide)
            ShowOrHideSlideSW();
    }
}
#endif

void MainWindow::ShowErrorMessageBox(QString message)
{
    EMessageBox::error(this, "Ошибка", message);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    if (!SWHide)
    {
        QWidget *sww = this->findChild<QWidget *>("slidew");
        if (sww == nullptr)
        {
          ERMSG("Пустой sww");
          return;
        }
        sww->setGeometry(QRect(width()-sww->width(), 0, sww->width(), height()));
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
        emit Finished();
    if (e->key() == Qt::Key_Escape)
        StdFunc::Cancel();
    QMainWindow::keyPressEvent(e);
}

void MainWindow::ParseString(QString Str)
{ 
    insl.clear();
    insl.append(Str.split(" "));

    if (insl.size() < 2)
    {
        EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
        ERMSG("Некорректная запись в файле");
        return;
    }
    if(insl.at(1) == "ETH")
    {
       Disconnect();
       FullName = insl.at(0);
       AdrBaseStation = insl.at(2).toUShort();
       IPtemp = Str.split(" ").last();
    }
    else if(insl.at(1) == "MODBUS")
    {
        if(insl.size() > 6)
        {
            bool ok;
            FullName = insl.at(0);
            Settings.Baud =  insl.at(2).toUInt(&ok);
            if (!ok)
            {
                EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
                ERMSG("Некорректная запись в файле");
                return;
            }
            Settings.Parity =  insl.at(3);
            Settings.Stop =  insl.at(4);
            Settings.Address =  insl.at(5).toUInt(&ok);
            if (!ok)
            {
                ERMSG("Некорректная запись в файле");
                EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
                return;
            }
            Settings.Port =  insl.at(6);
        }
        else
        {
            EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
        }
    }
    else
        EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
}

void MainWindow::ParseInter(QString str)
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

/*void MainWindow::SaveModBusString(QString ModBus)
{

}


void MainWindow::SaveInterface(QString Interface)
{
    interface = Interface;
}

void MainWindow::SDevice(QString Device)
{
    SaveDevice = Device;
}*/
void MainWindow::SetPortSlot(QString port)
{
#if PROGSIZE != PROGSIZE_EMUL
    SavePort = port;
    cn->TranslateDeviceAndSave(port);
#endif
}

void MainWindow::SetDefConf()
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


void MainWindow::DisconnectMessage()
{
    EMessageBox::information(this, "Провал", "Нет подключения");
}

void MainWindow::FinishHim()
{
    //Time->closeThr = true;
    //thr->exit();
    //emit Time->finished();

    //thr->wait(100);
    //thr->deleteLater();
    ConfM->timeIndex = -1;
    ConfM->confIndex = -1;
    TimeFunc::Wait(1000);
    //ConfM->stopRead(ConfM->timeIndex);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //TheEnd = 1;
    DisconnectAndClear();
    //while(!TimeThrFinished || !ModBusThrFinished)
    //TimeFunc::Wait(100);
    event->accept();
}

void MainWindow::CheckTimeFinish()
{
   TimeThrFinished = true;
}

void MainWindow::CheckModBusFinish()
{
   TimeThrFinished = true;
}

void MainWindow::Stop_BdaTimer(int index)
{
    if(CheckB != nullptr)
    {
        if(index != CheckB->checkIndex)
        {
            BdaTimer->stop();
            //thr->msleep(100);
        }
    }

}

void MainWindow::Start_BdaTimer(int index)
{
    if(CheckB != nullptr)
    {
        if(index == CheckB->checkIndex)
        {
            BdaTimer->start();
            //thr->msleep(100);
        }
    }

}

void MainWindow::Stop_TimeTimer(int index)
{
    if(Time != nullptr)
    {
        if(index != Time->timeIndex)
        {
            TimeTimer->stop();
            //thr->msleep(100);
        }
    }

}

void MainWindow::Start_TimeTimer(int index)
{
    if(Time != nullptr)
    {
        if(index == Time->timeIndex)
        {
            TimeTimer->start();
            //thr->msleep(100);
        }
    }

}

void MainWindow::GetUSBAlarmTimerTimeout()
{
    int i = 0, PredArarmcount = 0, Ararmcount = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    QPixmap *pm[2] = {pmred, pmgrn};

    if (Commands::GetBd(11, &Bd_block11, sizeof(Bd11)) == NOERROR)
    {
        for(i=0; i<18; i++)
        {
           if(Bd_block11.predAlarm & (0x00000001 << i))
           PredAlarmEvents[i] = 1;
           else
           PredAlarmEvents[i] = 0;

           if(PredAlarmEvents[i])
           {
              WDFunc::SetLBLImage(this, QString::number(951), pm[0]);
              PredArarmcount++;
           }
        }

        for(i=0; i<7; i++)
        {
           if(Bd_block11.alarm & (0x00000001 << i))
           AlarmEvents[i] = 1;
           else
           AlarmEvents[i] = 0;

           if(AlarmEvents[i])
           {
              WDFunc::SetLBLImage(this, QString::number(952), pm[0]);
              Ararmcount++;
           }
        }

        if(PredArarmcount == 0)
        WDFunc::SetLBLImage(this,  QString::number(951), pm[1]);

        emit SetPredAlarmColor(PredAlarmEvents);

        if(Ararmcount == 0)
        WDFunc::SetLBLImage(this,  QString::number(952), pm[1]);

        emit SetAlarmColor(AlarmEvents);
    }
}

void MainWindow::GetUSBAlarmInDialog()
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


void MainWindow::ConnectMessage(QString* Name)
{
  EMessageBox::information(this, "Успешно", "Связь с "+*Name+" установлена");
}
