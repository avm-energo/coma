#include "avaralarmktf.h"

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


AvarAlarmKTF::AvarAlarmKTF(QWidget *parent):
    AbstractAlarm(parent)
{
    AvarState();
}

void AvarAlarmKTF::AvarState()
{


    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    QString tmps = QString(PROGCAPTION);

    INFOMSG("AlarmState()");
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");
    //QPixmap *pm[2] = {pmred, pmgrn};
    QStringList events = QStringList() << "Аварийное сообщение по недопустимому превышению температуры обмотки"
                                       << "Аварийное сообщение по недопустимому уровню пускового тока         ";

    QWidget *w = new QWidget;
    Walarm = w;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");


    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", "5023", (AvarAlarmEventsKTF[12]) ? pmred : pmgrn));
    hlyout->addWidget(WDFunc::NewLBLT(w, events.at(0), "", "", ""), 1);
    vlayout->addLayout(hlyout);

    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(w, "", "", "5025", (AvarAlarmEventsKTF[14]) ? pmred : pmgrn));
    hlyout->addWidget(WDFunc::NewLBLT(w, events.at(1), "", "", ""), 1);
    vlayout->addLayout(hlyout);

    w->setLayout(vlayout);

    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb,SIGNAL(clicked()),this,SLOT(close()));
    lyout->addWidget(pb,0);
    setLayout(lyout);


}

void AvarAlarmKTF::AlarmState()
{
}
void AvarAlarmKTF::PredAlarmState()
{
}

void AvarAlarmKTF::UpdateUSB()
{
}

void AvarAlarmKTF::USBSetAlarms()
{
}

void AvarAlarmKTF::UpdatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);

}
void AvarAlarmKTF::UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *Signal)
{
    Q_UNUSED(Signal);
}
void AvarAlarmKTF::ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
void AvarAlarmKTF::ModBusUpdatePredAlarmEvents(ModBus::Coils Signal)
{
    Q_UNUSED(Signal);
}
