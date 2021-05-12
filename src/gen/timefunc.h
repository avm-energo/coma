#ifndef TIMEFUNC_H
#define TIMEFUNC_H

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QTimeZone>
#define SLEEPINT 20
#define TIMEOUT_MAIN 2000
#define TIMEOUT_BIG 30000

class TimeFunc
{
public:
    TimeFunc();

    static inline void Wait(int ms = SLEEPINT)
    {
        QElapsedTimer tmr;
        tmr.start();
        while (tmr.elapsed() < ms)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
    }

    static QString NsTimeToString(quint64 nstime);
    static QString UnixTime64ToString(quint64 utime, QTimeZone tz = QTimeZone::utc());
    static QString UnixTime64ToInvStringFractional(quint64 utime, QTimeZone tz = QTimeZone::utc());
    static QString UnixTime32ToInvString(quint32 utime, QTimeZone tz = QTimeZone::utc());
    static quint64 InvStringToUnixTime32(QString utime, QTimeZone tz = QTimeZone::utc());
    static QTimeZone userTimeZone();
    static QString userTimeZoneName()
    {
        return userTimeZone().displayName(QTimeZone::StandardTime, QTimeZone::OffsetName);
    }
    static bool WaitFor(bool &flag, int timeoutms = TIMEOUT_MAIN);
};

#endif // TIMEFUNC_H
