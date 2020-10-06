#include "alarm.h"

#include "../widgets/wd_func.h"

#include <QVBoxLayout>
#include <QtDebug>

Alarm::Alarm(QWidget *parent) : QWidget(parent)
{
}

void Alarm::Update(QList<bool> states)
{
    if (states.isEmpty())
        return;
    auto max_range = states.length();
    if (m_alarmFlags.size() != states.length())
    {
        qDebug() << m_alarmFlags.size() << ":" << states.length();
        max_range = std::min(m_alarmFlags.size(), states.length());
    }
    for (int i = 0; i < max_range; i++)
    {
        UpdatePixmaps(states.at(i), i);
    }
}

void Alarm::showEvent(QShowEvent *e)
{
    if (isVisible())
        move(QCursor::pos());
    e->accept();
}

void Alarm::UpdatePixmaps(bool isset, int position, bool warn)
{
    if (isset)
    {
        if (warn)
        {
            auto pixmap = WDFunc::NewCircle(Qt::yellow, CIRCLE_RADIUS);
            WDFunc::SetLBLImage(this, (QString::number(position)), &pixmap);
        }
        else
        {
            auto pixmap = WDFunc::NewCircle(Qt::red, CIRCLE_RADIUS);
            WDFunc::SetLBLImage(this, (QString::number(position)), &pixmap);
        }
    }
    else
    {
        auto pixmap = WDFunc::NewCircle(Qt::green, CIRCLE_RADIUS);
        WDFunc::SetLBLImage(this, (QString::number(position)), &pixmap);
    }
}

void Alarm::SetupUI(const QStringList &events, int counters)
{
    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");

    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;

    auto max_range = std::min(events.size(), counters);
    for (int i = 0; i < max_range; ++i)
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
