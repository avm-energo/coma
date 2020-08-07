#include "alarmstateall.h"

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



#include "../gen/colors.h"
#include "../widgets/wd_func.h"
#include "../gen/modulebsi.h"
#include "../gen/error.h"
#include "../gen/commands.h"

AlarmStateAll::AlarmStateAll(QWidget *parent):
    AbstractAlarm(parent)
{
    AlarmState();
}

void AlarmStateAll::AlarmState()
{
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
    connect(pb,SIGNAL(clicked()),this,SLOT(close()));
    lyout->addWidget(pb,0);
    setLayout(lyout);

}
void AlarmStateAll::PredAlarmState()
{
}
void AlarmStateAll::AvarState()
{
}

void AlarmStateAll::UpdateUSB()
{
}

void AlarmStateAll::USBSetAlarms()
{
}

void AlarmStateAll::UpdatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);

}
void AlarmStateAll::UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void AlarmStateAll::ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
void AlarmStateAll::ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
