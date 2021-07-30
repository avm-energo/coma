#include "timesyncronizer.h"

#include "helper.h"

#include <QDateTime>
#include <QTimer>
#include <iostream>
#include <sys/timex.h>
void printts(const timespec &st)
{
    auto datetime = QDateTime::fromMSecsSinceEpoch(((st.tv_sec * 1000) + (st.tv_nsec / 1.0e6)));
    std::cout << datetime.toString().toStdString().c_str() << std::endl;
    std::cout << st << std::endl;
}

TimeSyncronizer::TimeSyncronizer(QObject *parent) : QObject(parent)
{
    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, [this] { emit ntpStatusChanged(ntpStatus()); });
    timer->start();
}

void TimeSyncronizer::handleTime(const timespec &time)
{
    printts(time);
    setCurrentTime(time);
}

timespec TimeSyncronizer::currentTime() const
{
    timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    return time;
}

void TimeSyncronizer::setCurrentTime(const timespec &currentTime)
{
    clock_settime(CLOCK_REALTIME, &currentTime);
}

bool TimeSyncronizer::ntpStatus() const
{
    ntptimeval time;
    int status = ntp_gettime(&time);
    switch (status)
    {
    case TIME_OK:
        return true;
    case TIME_INS:
        return true;
    case TIME_DEL:
        return true;
    case TIME_OOP:
        return true;
    case TIME_WAIT:
        return true;
    case TIME_ERROR:
        return false;
    default:
        return false;
    }
}
