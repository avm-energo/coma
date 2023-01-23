#include "alarmstateall.h"

#include "../widgets/wd_func.h"
#include "board.h"
#include "modules.h"

#include <QBoxLayout>

AlarmStateAll::AlarmStateAll(QWidget *parent) : BaseAlarm(parent)
{
    mAlarmFlags = BSIALARMMASK;
}

/// \brief Request for updating a BSI data.
void AlarmStateAll::reqUpdate()
{
    auto iface = BaseInterface::iface();
    if (iface != nullptr)
        iface->reqBSI();
}

/// \brief The slot called when a device health changed for updating alarms.
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
        auto circle = WDFunc::NewCircle(color, circleRadius);
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

/// \brief Setup UI: creating text labels and indicators (pixmaps) for alarms displaying.
void AlarmStateAll::setupUI(const QStringList &events)
{
    auto mainLayout = new QVBoxLayout(this);
    for (int i = 0; i < events.size(); ++i)
    {
        auto hLayout = new QHBoxLayout;
        auto label = WDFunc::NewLBL2(this, "", QString::number(i));
        auto pixmap = WDFunc::NewCircle(normalColor, circleRadius);
        label->setPixmap(pixmap);
        hLayout->addWidget(label);
        hLayout->addWidget(WDFunc::NewLBL2(this, events.at(i)), 1);
        mainLayout->addLayout(hLayout);
    }
    mainLayout->addWidget(WDFunc::NewPB(this, "", "Ok", static_cast<QWidget *>(this), &QWidget::hide), 0);
    setLayout(mainLayout);
    const auto &board = Board::GetInstance();
    connect(&board, &Board::healthChanged, this, &AlarmStateAll::update);
}
