#include "basealarm.h"

#include "../gen/error.h"
#include "../widgets/wd_func.h"

#include <QDebug>
#include <QVBoxLayout>
BaseAlarm::BaseAlarm(BaseInterface *interface, QWidget *parent)
    : QWidget(parent), m_actualAlarmFlags(0x0000), m_iface(interface)
{
}

BaseAlarm::BaseAlarm(QWidget *parent) : QWidget(parent), m_actualAlarmFlags(0x0000)
{
    m_normalColor = Qt::green;
    m_alarmColor = Qt::red;
}

void BaseAlarm::setInterface(BaseInterface *interface) noexcept
{
    m_iface = interface;
}

BaseInterface *BaseAlarm::iface() const noexcept
{
    return m_iface;
}

void BaseAlarm::setupUI(const QStringList &events)
{
    // m_realAlarmSize = events.size();
    quint16 flagsCount = m_alarmFlags.count();
    quint16 eventsCount = events.size();
    Q_ASSERT(flagsCount == eventsCount);
    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");

    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;

    int i, j;
    for (i = 0, j = 0; i < m_alarmFlags.size(); ++i)
    {
        const bool flag = m_alarmFlags.test(i);
        if (!flag)
            continue;
        QHBoxLayout *hlyout = new QHBoxLayout;
        int number = i;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "transparent", QString::number(number)));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(j++), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }
    w->setLayout(vlayout);

    lyout->addWidget(w);
    lyout->addWidget(WDFunc::NewPB(this, "", "Ok", this, SLOT(hide())), 0);
    // lyout->addWidget(WDFunc::NewPB2(this, "", "Ok", this, &QWidget::hide), 0);
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
