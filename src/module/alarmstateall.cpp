#include "alarmstateall.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../widgets/wd_func.h"
#include "board.h"
#include "modules.h"

#include <QBoxLayout>
#include <QDebug>

AlarmStateAll::AlarmStateAll(QWidget *parent) : BaseAlarm(parent)
{
    mAlarmFlags = BSIALARMMASK;
}

void AlarmStateAll::update(quint32 health)
{
    const std::bitset<32> healthSet = health;
    for (int i = 0; i < AVM::HthToolTip.size(); ++i)
    {
        QColor color(normalColor);
        bool state = healthSet.test(i);
        if (state)
        {
            if (state != mAlarmFlags.test(i))
                color = Qt::yellow;
            else
                color = Qt::red;
        }
        QPixmap circle = WDFunc::NewCircle(color, circleRadius);
        WDFunc::SetLBLImage(this, QString::number(i), &circle);
    }

    QColor color;
    if (health & BSIALARMMASK)
        color = Qt::red;
    else if (health & BSIWARNMASK)
        color = Qt::yellow;
    else
        color = normalColor;
    emit updateColor(color);
}

void AlarmStateAll::setupUI(const QStringList &events)
{
    auto vlayout = new QVBoxLayout(this);
    for (int i = 0; i < events.size(); ++i)
    {
        auto hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL2(this, "", QString::number(i)));
        hlyout->addWidget(WDFunc::NewLBL2(this, events.at(i)), 1);
        vlayout->addLayout(hlyout);
    }
    vlayout->addWidget(WDFunc::NewPB(this, "", "Ok", static_cast<QWidget *>(this), &QWidget::hide), 0);
    const auto &board = Board::GetInstance();
    connect(&board, &Board::healthChanged, this, &AlarmStateAll::update);
}
