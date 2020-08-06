#include "alarmwidget.h"

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



AlarmWidget::AlarmWidget(QWidget *parent):
         QDialog(parent)
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
        connect(pb,SIGNAL(clicked()),this,SIGNAL(AlarmButtonPressed()));;
        QGroupBox *gb = new QGroupBox("");
        hlyout->addWidget(pb,Qt::AlignRight);
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", "950", pmgrn), 1);
        gb->setLayout(hlyout);
        hlyout2->addWidget(gb);


        gb = new QGroupBox("");
        hlyout = new QHBoxLayout;
        pb = new QPushButton("Предупредительная сигнализация");
        pb->setMinimumSize(QSize(230,30));
        connect(pb,SIGNAL(clicked()),this,SIGNAL(ModuleWarnButtonPressed()));
        hlyout->addWidget(pb,Qt::AlignRight);
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", "951", pmgrn), 1);
        gb->setLayout(hlyout);
        hlyout2->addWidget(gb);

        menu = new QMenu;
        gb = new QGroupBox("");
        hlyout = new QHBoxLayout;
        pb = new QPushButton("Аварийная сигнализация");
        pb->setMinimumSize(QSize(230,30));
        connect(pb,SIGNAL(clicked()),this,SIGNAL(ModuleAlarmButtonPressed()));
        hlyout->addWidget(pb,Qt::AlignRight);
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", "952", pmgrn), 1);
        gb->setLayout(hlyout);
        hlyout2->addWidget(gb);

        if (hlyout2->count())
        vlyout->addLayout(hlyout2);
        w->setLayout(vlyout);


        warnCounts[0xA284]=18;
        warnCounts[0xA287]=15;

        avarCounts[0xA284]=7;
        avarCounts[0xA287]=2;

        BdNumbers[0xA284]=11;
        BdNumbers[0xA287]=16;

}

void AlarmWidget::UpdateUSB()
{
    if (MainInterface == I_USB)
    {
        int i = 0, predalarmcount = 0, alarmcount = 0;
        QPixmap *pmgrn = new QPixmap("images/greenc.png");
        QPixmap *pmylw = new QPixmap("images/yellowc.png");
        QPixmap *pmred = new QPixmap("images/redc.png");
        BdAlarm signaling;
        quint32 MType=(MTypeB<<8)+MTypeM;

        WDFunc::SetLBLImage(this, "951", (predalarmcount == 0) ? pmgrn : pmylw);
        WDFunc::SetLBLImage(this, "952", (alarmcount == 0) ? pmgrn : pmred);


        if (Commands::GetBd(BdNumbers[MType], &signaling, sizeof(BdAlarm)) == NOERROR)
        {
            for(i=0; i<warnCounts[MType]; ++i)
            {
               if(signaling.Warn & (0x00000001 << i))
               {
                   WarnEvents[i] = 1;
                   ++predalarmcount;
                   break;
               }
               else
                   WarnEvents[i] = 0;
            }



            for(i=0; i<warnCounts[MType]; ++i)
            {
               if(signaling.Alarm & (0x00000001 << i))
               {
                   AvarAlarmEvents[i] = 1;
                   ++alarmcount;
                   break;
               }
               else
                   AvarAlarmEvents[i] = 0;
            }
            WDFunc::SetLBLImage(this, "951", (predalarmcount == 0) ? pmgrn : pmylw);
            WDFunc::SetLBLImage(this, "952", (alarmcount == 0) ? pmgrn : pmred);
            emit SetWarnAlarmColor(WarnEvents);
            emit SetAlarmColor(AvarAlarmEvents);
        }
        if (Commands::GetBsi(ModuleBSI::ModuleBsi) == NOERROR)
        {
            if (ModuleBSI::ModuleBsi.Hth & WarnBSIMask)
                WDFunc::SetLBLImage(this, "950", pmylw);
            else if (ModuleBSI::ModuleBsi.Hth & AvarBSIMask)
                WDFunc::SetLBLImage(this, "950", pmred);
            else
                WDFunc::SetLBLImage(this, "950", pmgrn);
        }


    }
}

void AlarmWidget::USBSetAlarms()
{
}

void AlarmWidget::UpdateWarningEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);

}
void AlarmWidget::UpdateStateWarningEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void AlarmWidget:: ModbusUpdateStateWarningEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
void AlarmWidget::ModBusUpdateWarningEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}

