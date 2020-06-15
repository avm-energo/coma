#include <QDateTime>
#include "timefunc.h"

TimeFunc::TimeFunc()
{

}

QString TimeFunc::NsTimeToString(quint64 nstime)
{
    quint32 unixtime = nstime / 1000000000L; // unix время
    QString tmps = QDateTime::fromTime_t(unixtime).toString("dd-MM-yyyy hh:mm:ss");
    quint32 nst = nstime % 1000000000L; // оставшееся время в нс до секунды
    tmps += "." + QString("%1").arg((nst/1000000L),3,10,QChar('0'));
    nst = nst % 1000000L;
    tmps += "." + QString("%1").arg((nst/1000L),3,10,QChar('0'));
    nst = nst % 1000L;
    tmps += "." + QString("%1").arg(nst,3,10,QChar('0'));
    return tmps;
}

QString TimeFunc::UnixTime64ToString(quint64 utime)
{
    quint32 tmpi = utime >> 32;
    QDateTime tn = QDateTime::fromTime_t(tmpi, Qt::UTC);
    quint32 timens = utime & 0xFFFFFFFF;
    QString ms = QString::number((timens/1000000));
    QString mcs = QString::number(((timens-ms.toInt()*1000000)/1000));
    QString ns = QString::number(timens-ms.toInt()*1000000-mcs.toInt()*1000);
    QString time = tn.toString("dd/MM/yyyy hh:mm:ss.")+ms+"."+mcs+"."+ns;
    return time;
}

QString TimeFunc::UnixTime64ToInvStringFractional(quint64 utime)
{
    quint32 tmpi = utime >> 32;
    QDateTime tn = QDateTime::fromTime_t(tmpi, Qt::UTC); // in seconds
    utime &= 0x00000000FFFFFFFF;
    QString outs = tn.toString("yyyy/MM/dd hh:mm:ss");
    QString frac = QString::number(utime);
    frac.truncate(3);
    outs += "." + frac;
    return outs;
}

QString TimeFunc::UnixTime32ToInvString(quint32 utime)
{
    QDateTime tn = QDateTime::fromTime_t(utime, Qt::UTC); // in seconds
    return tn.toString("yyyy/MM/dd hh:mm:ss");
}

