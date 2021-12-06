#include "basealarm.h"

#include "../gen/error.h"
#include "../widgets/wd_func.h"

#include <QDebug>
#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>

BaseAlarm::BaseAlarm(QWidget *parent) : QWidget(parent), m_actualAlarmFlags(0x0000)
{

    m_normalColor = Qt::transparent;
    m_alarmColor = Qt::red;
}

void BaseAlarm::disable()
{
    disconnect(this);
}

void BaseAlarm::setupUI(const QStringList &events)
{
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

    auto mainWindow = WDFunc::getMainWindow();
    if (mainWindow)
    {
        auto mainHeight = mainWindow->height();
        const auto maxElements = mainHeight / (2 * circleRadius);
        if (eventsCount <= maxElements)
        {
            lyout->addLayout(vlayout);
        }
        else
        {
            QWidget *widget = new QWidget(this);
            widget->setLayout(vlayout);
            QScrollArea *scrollArea = new QScrollArea(this);
            scrollArea->setWidget(widget);
            lyout->addWidget(scrollArea);
        }
    }
    else
    {
        QWidget *widget = new QWidget(this);
        widget->setLayout(vlayout);
        QScrollArea *scrollArea = new QScrollArea(this);
        scrollArea->setWidget(widget);
        lyout->addWidget(scrollArea);
    }
    lyout->addWidget(WDFunc::NewPB(this, "", "Ok", static_cast<QWidget *>(this), &QWidget::hide));
    setLayout(lyout);
}

void BaseAlarm::updatePixmap(bool isset, int position)
{
    auto pixmap = WDFunc::NewCircle((isset) ? m_alarmColor : m_normalColor, circleRadius);
    auto status = WDFunc::SetLBLImage(this, QString::number(position), &pixmap);
    if (!status)
        qCritical() << Error::DescError;

    m_actualAlarmFlags.set(position, isset);

    emit updateColor((m_actualAlarmFlags != 0x0000) ? m_alarmColor : m_normalColor);
}

void BaseAlarm::showEvent(QShowEvent *e)
{
    if (isVisible())
        move(QCursor::pos());
    e->accept();
}
