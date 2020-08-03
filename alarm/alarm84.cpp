
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
#include <QBoxLayout>
#include <QPushButton>


#include "abstractalarm.h"
#include "../gen/colors.h"
#include "../widgets/wd_func.h"
#include "alarm84.h"
#include "../gen/modulebsi.h"
#include "../gen/error.h"
#include "../gen/commands.h"


Alarm84::Alarm84(QWidget *parent):
      AbstractAlarm(parent)
{
   AlmReleWidget();
  // AlmNewTimers();

}


void Alarm84::AlmNewTimers()
{

   // BdaTimer = new QTimer;
  //  BdaTimer->setInterval(ANMEASINT);
  //  BdaTimer->start();

  //  connect(BdaTimer,SIGNAL(timeout()), this, SLOT(USBSetAlarms()));
  //  connect(BdaTimer,SIGNAL(timeout()),this,SLOT(UpdateUSB()));

}



void Alarm84::DeviceState()
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

void Alarm84::PredAlarmState()
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

void Alarm84::AlarmState()
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


void Alarm84::UpdatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    int i = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmylw = new QPixmap("images/yellowc.png");
    QPixmap *pm[2] = {pmgrn, pmylw};

    for(i=0; i<Signal->SigNumber; i++)
    {
        quint32 sigadr = Signal->Spon[i].SigAdr;
        quint8 sigval = Signal->Spon[i].SigVal;
        if(!(sigval & 0x80))
        {
            int pmidx = (sigval & 0x00000001) ? 1 : 0;
            if((((sigadr >= 3024) && (sigadr < 3027))) || ((sigadr >= 3030) && (sigadr < 3033)) || (sigadr == 3035))
                WDFunc::SetLBLImage(Walarm, (QString::number(sigadr)), pm[pmidx]);
            else
                WDFunc::SetLBLImage(Wpred, (QString::number(sigadr)), pm[pmidx]);
       }
    }
}

void Alarm84::UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    int i = 0, predalarmcount = 0, alarmcount = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");

    for(i=0; i<Signal->SigNumber; i++)
    {
        quint8 sigval = Signal->Spon[i].SigVal;
        if(!(sigval & 0x80))
        {
            quint32 sigadr = Signal->Spon[i].SigAdr;
            int pmidx = (sigval & 0x00000001) ? 1 : 0;

            if((sigadr >= 3011) && (sigadr < 3024))
            PredAlarmEvents[sigadr - 3011] = pmidx;
            else if((sigadr >= 3024) && (sigadr < 3027))
            AlarmEvents[sigadr - 3024] = pmidx;
            else if((sigadr >= 3027) && (sigadr < 3030))
            PredAlarmEvents[sigadr - 3014] = pmidx;
            else if((sigadr >= 3030) && (sigadr < 3033))
            AlarmEvents[sigadr - 3027] = pmidx;
            else if(sigadr == 3033)
            PredAlarmEvents[16] = pmidx;
            else if(sigadr == 3034)
            PredAlarmEvents[17] = pmidx;
            else if(sigadr == 3035)
            AlarmEvents[6] = pmidx;
        }
    }
    for(i=0; i<18; i++)
    {
        if(PredAlarmEvents[i])
        {
           WDFunc::SetLBLImage(this, "951", pmred);
           predalarmcount++;
        }
    }
    if(predalarmcount == 0)
        WDFunc::SetLBLImage(this,  "951", pmgrn);
    for(i=0; i<7; i++)
    {
        if(AlarmEvents[i])
        {
           WDFunc::SetLBLImage(this, "952", pmred);
           alarmcount++;
        }
    }
    if(alarmcount == 0)
        WDFunc::SetLBLImage(this, "952", pmgrn);
}

void Alarm84::ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)
{
    int i = 0, predalarmcount = 0, alarmcount = 0;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");

    for(i=0; i<Signal.countBytes; i++)
    {
        if(i == 3)
        {
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
           WDFunc::SetLBLImage(this, "951", pmred);
           predalarmcount++;
        }
    }
    if(predalarmcount == 0)
        WDFunc::SetLBLImage(this,  "951", pmgrn);

    emit SetPredAlarmColor(PredAlarmEvents);

    for(i=0; i<7; i++)
    {
        if(AlarmEvents[i])
        {
           WDFunc::SetLBLImage(this, "952", pmred);
           alarmcount++;
        }
    }
    if(alarmcount == 0)
        WDFunc::SetLBLImage(this,  "952", pmgrn);
    emit SetAlarmColor(AlarmEvents);
}

void Alarm84::ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)
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
                WDFunc::SetLBLImage(Wpred, "3033", pm[signal]);
                if(j==7)
                WDFunc::SetLBLImage(Wpred, "3034", pm[signal]);

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


void Alarm84::UpdateUSB()
{
    if (MainInterface == I_USB)
    {
        int i = 0, predalarmcount = 0, alarmcount = 0;
        QPixmap *pmgrn = new QPixmap("images/greenc.png");
        QPixmap *pmylw = new QPixmap("images/yellowc.png");
        QPixmap *pmred = new QPixmap("images/redc.png");
        Bd11 Bd_block11;

        INFOMSG("UpdateUSB()");
//        if (CheckB != nullptr)
//            CheckB->USBUpdate();

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
            WDFunc::SetLBLImage(this, "951", (predalarmcount == 0) ? pmgrn : pmylw);
            WDFunc::SetLBLImage(this, "952", (alarmcount == 0) ? pmgrn : pmred);
            emit SetPredAlarmColor(PredAlarmEvents);
            emit SetAlarmColor(AlarmEvents);
        }
        if (Commands::GetBsi(ModuleBSI::ModuleBsi) == NOERROR)
        {
            if (ModuleBSI::ModuleBsi.Hth & PredBSIMask)
                WDFunc::SetLBLImage(this, "950", pmylw);
            else if (ModuleBSI::ModuleBsi.Hth & AvarBSIMask)
                WDFunc::SetLBLImage(this, "950", pmred);
            else
                WDFunc::SetLBLImage(this, "950", pmgrn);
        }
    }
}

void Alarm84::USBSetAlarms()
{
    if (MainInterface == I_USB)
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
}






















