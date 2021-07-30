#pragma once

#include <QObject>
#include <time.h>
class TimeSyncronizer : public QObject
{
    Q_OBJECT
public:
    explicit TimeSyncronizer(QObject *parent = nullptr);

    timespec currentTime() const;
    void setCurrentTime(const timespec &currentTime);
    bool ntpStatus() const;

    Q_PROPERTY(bool ntpStatus READ ntpStatus NOTIFY ntpStatusChanged)
public slots:
    void handleTime(const timespec &time);

signals:
    void ntpStatusChanged(bool);
};
