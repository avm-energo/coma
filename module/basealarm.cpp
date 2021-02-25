#include "basealarm.h"

#include "../gen/error.h"
#include "../widgets/wd_func.h"

#include <QDebug>
#include <QVBoxLayout>
// BaseAlarm::BaseAlarm(BaseInterface *interface, QWidget *parent)
//    : QWidget(parent), m_actualAlarmFlags(0x0000), m_iface(interface)
// BaseAlarm::BaseAlarm(QWidget *parent)
//    : QWidget(parent), m_actualAlarmFlags(0x0000)
//{
//}

BaseAlarm::BaseAlarm(QWidget *parent) : QWidget(parent), m_actualAlarmFlags(0x0000)
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_normalColor = Qt::green;
    m_alarmColor = Qt::red;
}

void BaseAlarm::disable()
{
    disconnect(this);
}

// void BaseAlarm::setInterface(BaseInterface *interface) noexcept
//{
//    m_iface = interface;
//}

// BaseInterface *BaseAlarm::iface() const noexcept
//{
//    return m_iface;
//}

void BaseAlarm::setupUI(const QStringList &events)
{
    // m_realAlarmSize = events.size();
    qint64 flagsCount = m_alarmFlags.count();
    qint64 eventsCount = events.size();
    Q_ASSERT(flagsCount == eventsCount);

    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;

    unsigned int i, j;
    for (i = 0, j = 0; (i < m_alarmFlags.size()); ++i)
    {

        const bool flag = m_alarmFlags.test(i);
        if (!flag)
            continue;
        QHBoxLayout *hlyout = new QHBoxLayout;
        int number = i;
        auto *label = WDFunc::NewLBL2(this, "", QString::number(number));
        auto pixmap = WDFunc::NewCircle(m_normalColor, circleRadius);
        label->setPixmap(pixmap);
        hlyout->addWidget(label);

        hlyout->addWidget(WDFunc::NewLBL2(this, events.at(j++)), 1);
        vlayout->addLayout(hlyout);
    }
    lyout->addLayout(vlayout);

    lyout->addWidget(WDFunc::NewPB(this, "", "Ok", static_cast<QWidget *>(this), &QWidget::hide), 0);
    setLayout(lyout);
}

void BaseAlarm::updatePixmap(bool isset, int position)
{
    auto pixmap = WDFunc::NewCircle((isset) ? m_alarmColor : m_normalColor, circleRadius);
    auto status = WDFunc::SetLBLImage(this, QString::number(position), &pixmap);
    if (!status)
        qCritical() << Error::DescError;
    //    else
    //        qInfo() << Error::NoError;
    m_actualAlarmFlags.set(position, isset);
    // m_actualAlarmFlags[position] = isset;

    emit updateColor((m_actualAlarmFlags != 0x0000) ? m_alarmColor : m_normalColor);
    //    emit updateAlarm(position, isset);
    //    if (isset)
    //    {
    //        auto pixmap = WDFunc::NewCircle(Qt::red, CIRCLE_RADIUS);
    //        WDFunc::SetLBLImage(this, (QString::number(position)), &pixmap);
    //    }
    //    else
    //    {
    //        auto pixmap = WDFunc::NewCircle(Qt::green, CIRCLE_RADIUS);
    //        WDFunc::SetLBLImage(this, (QString::number(position)), &pixmap);
    //    }
}

// void BaseAlarm::updatePreferColor(QColor &newColor)
//{
//    if (m_preferColor.red() >= newColor.red())
//        return;

//}

void BaseAlarm::showEvent(QShowEvent *e)
{
    if (isVisible())
        move(QCursor::pos());
    e->accept();
}
