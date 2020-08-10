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


AvarAlarmKTF::AvarAlarmKTF(AlarmClass *alarm,QWidget *parent):
    AbstractAlarm(parent)
{
    Alarm = alarm;
    AvarState();
}

void AvarAlarmKTF::AvarState()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    QString tmps = QString(PROGCAPTION);

    QStringList events = QStringList() << "Аварийное сообщение по недопустимому превышению температуры обмотки"
                                       << "Аварийное сообщение по недопустимому уровню пускового тока         ";

    QWidget *w = new QWidget;

    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");

    for (int i = 0; i < Alarm->avarCounts[MTYPE_KTF]; ++i)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(i)));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

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


void AvarAlarmKTF::Update(QList<bool> states)
{
    int i;
    QPixmap *pmgrn = new QPixmap("images/greenc.png");
    QPixmap *pmred = new QPixmap("images/redc.png");

    for(i=0; i<Alarm->avarCounts[MTYPE_KTF]; i++)
    {
        quint32 alarm =states[i];
        WDFunc::SetLBLImage(this, (QString::number(i)), (alarm) ? pmred : pmgrn);
    }

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
