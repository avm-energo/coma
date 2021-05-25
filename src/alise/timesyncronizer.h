#ifndef TIMESYNCRONIZER_H
#define TIMESYNCRONIZER_H

#include <QObject>
#include <time.h>
class TimeSyncronizer : public QObject
{
    Q_OBJECT
public:
    explicit TimeSyncronizer(QObject *parent = nullptr);

    timespec currentTime() const;
    void setCurrentTime(const timespec &currentTime);
    bool isNtpSync() const;
signals:
    void sendTime(const timespec &);
public slots:
    void handleTime(const timespec &time);

private:
    timespec m_currentTime;
};

#endif // TIMESYNCRONIZER_H
