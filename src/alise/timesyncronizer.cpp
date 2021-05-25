#include "timesyncronizer.h"

#include "helper.h"

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
    auto newTime = time;
    newTime.tv_sec += 3600;
    emit sendTime(newTime);
}

timespec TimeSyncronizer::currentTime() const
{
    return m_currentTime;
}

void TimeSyncronizer::setCurrentTime(const timespec &currentTime)
{
    m_currentTime = currentTime;
}

bool TimeSyncronizer::isNtpSync() const
{
    ntptimeval time;
    int status = ntp_gettime(&time);
    return status != -1;
}
