
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
#include "../gen/modulebsi.h"
#include "../gen/error.h"
#include "../gen/commands.h"
#include "avaralarm84.h"

AvarAlarm84::AvarAlarm84(QWidget *parent):
    AbstractAlarm(parent)
{
   AvarState();
}

void AvarAlarm84::AvarState()
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
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3024+i), (AvarAlarmEvents[i]) ? pmred : pmgrn));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    for (int i = 0; i < 3; ++i)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(3030+i), (AvarAlarmEvents[3+i]) ? pmred : pmgrn));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(3+i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", "3035", (AvarAlarmEvents[6]) ? pmred : pmgrn));
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

void AvarAlarm84::AlarmState()
{
}
void AvarAlarm84::PredAlarmState()
{
}

void AvarAlarm84::UpdateUSB()
{
}

void AvarAlarm84::USBSetAlarms()
{
}

void AvarAlarm84::UpdatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);

}
void AvarAlarm84::UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void AvarAlarm84::ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
void AvarAlarm84::ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}









