#ifndef TIMEFUNC_H
#define TIMEFUNC_H

#include <QTime>
#include <QCoreApplication>

#define SLEEPINT    20


class TimeFunc
{
public:
    TimeFunc();

    static inline void Wait(int ms = SLEEPINT)
    {
        QTime tme;
        tme.start();
        while (tme.elapsed() < ms)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
    }

    static QString NsTimeToString (quint64 nstime);
    static QString UnixTime64ToString(quint64 utime);

};

#endif // TIMEFUNC_H
