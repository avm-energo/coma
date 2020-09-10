
#include "avaralarmkiv.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../usb/commands.h"
#include "../widgets/wd_func.h"
#include "abstractalarm.h"

#include <QBoxLayout>

AvarAlarmKIV::AvarAlarmKIV(AlarmClass *alarm, QDialog *parent) : AbstractAlarm(parent)
{
    Alarm = alarm;
    AvarState();
}

void AvarAlarmKIV::AvarState()
{

    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;

    QStringList events = QStringList() << "Авария по приращению тангенса дельта ввода фазы А"
                                       << "Авария по приращению тангенса дельта ввода фазы B"
                                       << "Авария по приращению тангенса дельта ввода фазы C"
                                       << "Авария по приращению C ввода фазы А              "
                                       << "Авария по приращению C ввода фазы B              "
                                       << "Авария по приращению C ввода фазы C              "
                                       << "Авария по недопустимому небалансу токов          ";
    QWidget *w = new QWidget;

    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");

    for (int i = 0; i < Alarm->MapAlarm[MTYPE_KIV].avarCounts; ++i)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "transparent", QString::number(i)));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    w->setLayout(vlayout);

    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb, SIGNAL(clicked()), this, SLOT(hide()));
    lyout->addWidget(pb, 0);
    setLayout(lyout);
}

void AvarAlarmKIV::AlarmState()
{
}
void AvarAlarmKIV::WarnAlarmState()
{
}

void AvarAlarmKIV::Update(QList<bool> states)
{
    int i;
    float circleRadius = 25.0;
    if (states.isEmpty())
        return;
    for (i = 0; i < Alarm->MapAlarm[MTYPE_KIV].avarCounts; i++)
    {
        quint32 alarm = states.at(i);
        alarm ? WDFunc::SetLBLImage(this, (QString::number(i)), &WDFunc::NewCircle(Qt::red, circleRadius))
              : WDFunc::SetLBLImage(this, (QString::number(i)), &WDFunc::NewCircle(Qt::green, circleRadius));
    }
}
