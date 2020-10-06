#include "alarmstateall.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../usb/commands.h"
#include "../widgets/wd_func.h"

#include <QBoxLayout>

#if _MSC_VER && !__INTEL_COMPILER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

AlarmStateAll::AlarmStateAll(QWidget *parent) : UDialog(parent)
{
    AlarmState();
}

void AlarmStateAll::AlarmState()
{
    QVBoxLayout *lyout = new QVBoxLayout;

    QVBoxLayout *vlayout = new QVBoxLayout;

    QWidget *w = new QWidget;
    w->setObjectName("window");
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};"); // color:
    float circleRadius = CIRCLE_RADIUS;                                     // rgba(220,220,220,255);

    for (int i = 0; i < HthToolTip().size(); ++i)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        hlyout->setObjectName("hlyout" + QString::number(i));

        if (ModuleBSI::ModuleBsi.Hth & (0x00000001 << i))
        {
            QPixmap circle = WDFunc::NewCircle(Qt::red, circleRadius);
            hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(i), &circle));
        }
        else
        {
            QPixmap circle = WDFunc::NewCircle(Qt::green, circleRadius);
            hlyout->addWidget(WDFunc::NewLBL(w, "", "", QString::number(i), &circle));
        }

        hlyout->addWidget(WDFunc::NewLBLT(w, HthToolTip().at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    w->setLayout(vlayout);

    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb, SIGNAL(clicked()), this, SLOT(hide()));
    lyout->addWidget(pb, 0);
    this->setLayout(lyout);
}

void AlarmStateAll::UpdateHealth(quint32 health)
{
    float circleRadius = CIRCLE_RADIUS;
    for (quint32 i = 0; i < health; ++i)
    {
        QPixmap circle = WDFunc::NewCircle(Qt::green, circleRadius);
        if (health & (0x00000001 << i))
            circle = (WarnPositions.indexOf(i) == -1) ? WDFunc::NewCircle(Qt::yellow, circleRadius)
                                                      : WDFunc::NewCircle(Qt::red, circleRadius);
        WDFunc::SetLBLImage(this, (QString::number(i)), &circle);
    }
}

void AlarmStateAll::CallUpdateHealth()
{
    this->UpdateHealth(ModuleBSI::ModuleBsi.Hth);
}

void AlarmStateAll::Update(QList<bool>)
{
}
