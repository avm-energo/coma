#include "timesyncronizer.h"

#include "helper.h"
//#include
#include <QDateTime>
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

bool TimeSyncronizer::isNtpSync() const
{
    ntptimeval time;
    int status = ntp_gettime(&time);
    return status != -1;
}
