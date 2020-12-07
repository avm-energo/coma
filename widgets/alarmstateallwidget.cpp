#include "alarmstateallwidget.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../module/modules.h"
#include "../widgets/wd_func.h"

#include <QBoxLayout>

AlarmStateAll::AlarmStateAll(QWidget *parent) : QWidget(parent)
{
    const auto &board = Board::GetInstance();
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    //    QWidget *w = new QWidget;
    setObjectName("window");
    setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};"); // color:
    float circleRadius = CIRCLE_RADIUS;                                  // rgba(220,220,220,255);

    for (int i = 0; i < AVM::HthToolTip.size(); ++i)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        hlyout->setObjectName("hlyout" + QString::number(i));

        if (board.health() & (0x00000001 << i))
        {
            QPixmap circle = WDFunc::NewCircle(Qt::red, circleRadius);
            hlyout->addWidget(WDFunc::NewLBL(this, "", "", QString::number(i), &circle));
        }
        else
        {
            QPixmap circle = WDFunc::NewCircle(Qt::green, circleRadius);
            hlyout->addWidget(WDFunc::NewLBL(this, "", "", QString::number(i), &circle));
        }

        hlyout->addWidget(WDFunc::NewLBLT(this, AVM::HthToolTip.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    //    w->setLayout(vlayout);
    lyout->addLayout(vlayout);
    lyout->addWidget(WDFunc::NewPB(this, "", "Ok", this, SLOT(hide())), 0);
    //   lyout->addWidget(pb, 0);
    this->setLayout(lyout);
    connect(&board, &Board::healthChanged, this, &AlarmStateAll::UpdateHealth);
}

// void AlarmStateAll::AlarmState()
//{
//}

void AlarmStateAll::UpdateHealth(quint32 health)
{
    float circleRadius = CIRCLE_RADIUS;
    for (int i = 0; i < AVM::HthToolTip.size(); ++i)
    {
        QPixmap circle = WDFunc::NewCircle(Qt::green, circleRadius);
        if (health & (0x00000001 << i))
            circle = (WarnPositions.indexOf(i) == -1) ? WDFunc::NewCircle(Qt::yellow, circleRadius)
                                                      : WDFunc::NewCircle(Qt::red, circleRadius);
        WDFunc::SetLBLImage(this, (QString::number(i)), &circle);
    }
    emit BSIUpdated((health & BSI_HEALTH_MASK) == 0x0000);
}

// void AlarmStateAll::CallUpdateHealth()
//{
//    UpdateHealth(ModuleBSI::ModuleBsi.Hth);
//}

void AlarmStateAll::update()
{
    UpdateHealth(Board::GetInstance().health());
}
