#include "alarm.h"

#include "../widgets/wd_func.h"

#include <QVBoxLayout>
#include <QtDebug>

Alarm::Alarm(QWidget *parent) : QWidget(parent)
{
}

int Alarm::realAlarmSize()
{
    return m_realAlarmSize;
}

/*void Alarm::Update(std::bitset<32> &states)
{
    for (int i = 0; i < m_realAlarmSize; ++i)
    {
        UpdatePixmaps(states[i], i);
    }
} */

void Alarm::showEvent(QShowEvent *e)
{
    if (isVisible())
        move(QCursor::pos());
    e->accept();
}

void Alarm::updatePixmap(bool isset, int position)
{
    auto pixmap = WDFunc::NewCircle((isset) ? Qt::red : Qt::green, CIRCLE_RADIUS);
    WDFunc::SetLBLImage(this, (QString::number(position)), &pixmap);
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

void Alarm::setupUI(const QStringList &events)
{
    m_realAlarmSize = events.size();
    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");

    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;

    for (int i = 0; i < m_realAlarmSize; ++i)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "transparent", QString::number(i)));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    w->setLayout(vlayout);

    lyout->addWidget(w);
    lyout->addWidget(WDFunc::NewPB(this, "", "Ok", this, SLOT(hide())), 0);
    setLayout(lyout);
}
