#include "alarm.h"

#include "../gen/datamanager.h"
//#include "../gen/helper.h"
#include "../widgets/wd_func.h"

#include <QVBoxLayout>
#include <QtDebug>

BaseAlarm::BaseAlarm(QWidget *parent) : QWidget(parent), m_actualAlarmFlags(0x0000)
{
    connect(&DataManager::GetInstance(), &DataManager::singlePointReceived, this, &BaseAlarm::updateSpData);
}

// int Alarm::realAlarmSize()
//{
//    return m_realAlarmSize;
//}

/*void Alarm::Update(std::bitset<32> &states)
{
    for (int i = 0; i < m_realAlarmSize; ++i)
    {
        UpdatePixmaps(states[i], i);
    }
} */

void BaseAlarm::showEvent(QShowEvent *e)
{
    if (isVisible())
        move(QCursor::pos());
    e->accept();
}

void BaseAlarm::updatePixmap(bool isset, int position)
{
    auto pixmap = WDFunc::NewCircle((isset) ? Qt::red : Qt::green, CIRCLE_RADIUS);
    auto status = WDFunc::SetLBLImage(this, QString::number(position), &pixmap);
    if (!status)
        qCritical() << Error::DescError;
    //    else
    //        qInfo() << Error::NoError;
    m_actualAlarmFlags.set(position, isset);
    // m_actualAlarmFlags[position] = isset;
    emit updateAlarm(m_actualAlarmFlags != 0x0000);
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

void BaseAlarm::updateSpData(const DataTypes::SinglePointWithTimeStruct &sp)
{
    quint32 minAddress = m_startAlarmAddress;
    quint32 maxAddress = m_startAlarmAddress + m_alarmFlags.size();
    if (!((sp.sigAdr >= minAddress) && (sp.sigAdr <= maxAddress)))
        return;
    // bool alarmFlag = false; // warning flag

    const int index = (sp.sigAdr - minAddress);
    // int index = sp.sigAdr;
    const quint8 sigval = sp.sigVal;
    if (sigval & 0x80)
        return;

    if (m_alarmFlags.test(index))
    {
        //    qDebug() << m_alarmFlags;
        //    qDebug() << index << bool(sigval);
        // alarmFlag = true;
        updatePixmap(sigval & 0x00000001, index);
    }
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
