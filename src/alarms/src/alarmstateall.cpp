#include "alarms/alarmstateall.h"

#include "../../module/modules.h"
#include "../../widgets/wd_func.h"

#include <QBoxLayout>

AlarmStateAll::AlarmStateAll(QWidget *parent) : BaseAlarm(parent)
{
    m_alarmFlags = BSIALARMMASK;
}

void AlarmStateAll::reqUpdate()
{
    engine()->currentConnection()->reqBSI();
}

void AlarmStateAll::update(quint32 health)
{
    const std::bitset<32> healthSet = health;
    for (int i = 0; i < AVM::HthToolTip.size(); ++i)
    {
        QColor color(m_normalColor);
        bool state = healthSet.test(i);
        if (state)
        {
            if (m_alarmFlags.test(i))
                color = Qt::red;
            else
                color = Qt::yellow;
        }
        auto circle = WDFunc::NewCircle(color, circleRadius);
        WDFunc::SetLBLImage(this, QString::number(i), &circle);
    }

    QColor color;
    if (health & BSIALARMMASK)
        color = Qt::red;
    else if (health & BSIWARNMASK)
        color = Qt::yellow;
    else
        color = m_normalColor;
    emit updateColor(color);
}

void AlarmStateAll::setupUI(const QStringList &events)
{
    auto mainLayout = new QVBoxLayout(this);
    for (int i = 0; i < events.size(); ++i)
    {
        auto hLayout = new QHBoxLayout;
        auto label = WDFunc::NewLBL2(this, "", QString::number(i));
        auto pixmap = WDFunc::NewCircle(m_normalColor, circleRadius);
        label->setPixmap(pixmap);
        hLayout->addWidget(label);
        hLayout->addWidget(WDFunc::NewLBL2(this, events.at(i)), 1);
        mainLayout->addLayout(hLayout);
    }
    mainLayout->addWidget(WDFunc::NewPB(this, "", "Ok", static_cast<QWidget *>(this), &QWidget::hide), 0);
    setLayout(mainLayout);
}
