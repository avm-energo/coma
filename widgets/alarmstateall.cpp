#include "alarmstateall.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/error.h"
#include "../module/modules.h"
#include "../widgets/wd_func.h"

#include <QBoxLayout>
#include <QDebug>

constexpr quint32 BSIWARNMASK = 0x00005F55;
constexpr quint32 BSIALARMMASK = 0x000020AA;
AlarmStateAll::AlarmStateAll(QWidget *parent) : BaseAlarm(parent)
{
    m_alarmFlags = BSIALARMMASK;
}

void AlarmStateAll::update(quint32 health)
{
    // NOTE hardcoded value for test
    // health = 0xDEADBEEF;
    const std::bitset<32> healthSet = health;

    for (int i = 0; i < AVM::HthToolTip.size(); ++i)
    {
        QColor color(Qt::green);
        bool state = healthSet.test(i);
        if (state)
        {
            if (state != m_alarmFlags.test(i))
                color = Qt::yellow;
            else
                color = Qt::red;
        }

        QPixmap circle = WDFunc::NewCircle(color, circleRadius);

        WDFunc::SetLBLImage(this, (QString::number(i)), &circle);
    }
    QColor color;
    if (health & BSIALARMMASK)
        color = Qt::red;
    else if (health & BSIWARNMASK)
        color = Qt::yellow;
    else
        color = Qt::green;

    emit updateColor(color);
}

// void AlarmStateAll::CallUpdateHealth()
//{
//    UpdateHealth(ModuleBSI::ModuleBsi.Hth);
//}

void AlarmStateAll::reqUpdate()
{
    //    Q_ASSERT(iface() != nullptr);
    //    iface()->reqBSI();
    Q_ASSERT(BaseInterface::iface() != nullptr);
    BaseInterface::iface()->reqBSI();
    // update(Board::GetInstance().health());
}

void AlarmStateAll::setupUI(const QStringList &events)
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    setObjectName("window");
    setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");

    for (int i = 0; i < events.size(); ++i)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        hlyout->setObjectName("hlyout" + QString::number(i));
        QPixmap circle = WDFunc::NewCircle(Qt::green, circleRadius);
        hlyout->addWidget(WDFunc::NewLBL(this, "", "", QString::number(i), &circle));

        hlyout->addWidget(WDFunc::NewLBLT(this, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }
    lyout->addLayout(vlayout);
    lyout->addWidget(WDFunc::NewPB(this, "", "Ok", this, SLOT(hide())), 0);
    this->setLayout(lyout);
    const auto &board = Board::GetInstance();

    connect(&board, &Board::healthChanged, this, &AlarmStateAll::update);
}
