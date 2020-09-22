#ifndef TIMEFUNC_H
#define TIMEFUNC_H

#include "../gen/error.h"

#include <QCoreApplication>
#include <QElapsedTimer>

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
    static QString UnixTime64ToString(quint64 utime);
    static QString UnixTime64ToInvStringFractional(quint64 utime);
    static QString UnixTime32ToInvString(quint32 utime);
    static Error::Msg WaitFor(bool &flag, int timeoutms = TIMEOUT_MAIN);
};

#endif // TIMEFUNC_H
