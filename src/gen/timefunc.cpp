#include "timefunc.h"

#include "settings.h"

#include <QDateTime>
#include <QSettings>
#include <memory>

TimeFunc::TimeFunc()
{
}

bool TimeFunc::WaitFor(bool &flag, int timeoutms)
{
    QElapsedTimer tmr;
    tmr.start();
    while ((!flag) && (tmr.elapsed() < timeoutms))
        QCoreApplication::processEvents();
    if (!flag)
        return false;
    return true;
}

QString TimeFunc::NsTimeToString(quint64 nstime)
{
    quint32 unixtime = nstime / 1000000000L; // unix время
    QString tmps = QDateTime::fromSecsSinceEpoch(unixtime).toString("dd-MM-yyyy hh:mm:ss");
    quint32 nst = nstime % 1000000000L; // оставшееся время в нс до секунды
    tmps += "." + QString("%1").arg((nst / 1000000L), 3, 10, QChar('0'));
    nst = nst % 1000000L;
    tmps += "." + QString("%1").arg((nst / 1000L), 3, 10, QChar('0'));
    nst = nst % 1000L;
    tmps += "." + QString("%1").arg(nst, 3, 10, QChar('0'));
    return tmps;
}

QString TimeFunc::UnixTime64ToString(quint64 utime, QTimeZone tz)
{
    quint32 tmpi = utime >> 32;
    QDateTime tn = QDateTime::fromSecsSinceEpoch(tmpi, tz);
    quint32 timens = utime & 0xFFFFFFFF;
    QString ms = QString::number((timens / 1000000));
    QString mcs = QString::number(timens / 1000 - ms.toULongLong() * 1000);
    QString ns = QString::number(timens - ms.toULongLong() * 1000000 - mcs.toULongLong() * 1000);
    QString time = tn.toString("dd/MM/yyyy hh:mm:ss.") + ms + "." + mcs + "." + ns;
    return time;
}

QString TimeFunc::UnixTime64ToInvStringFractional(quint64 utime, QTimeZone tz)
{
    quint32 tmpi = utime >> 32;
    QDateTime tn = QDateTime::fromSecsSinceEpoch(tmpi, tz); // in seconds
    utime &= 0x00000000FFFFFFFF;
    QString outs = tn.toString("yyyy/MM/dd hh:mm:ss");
    QString frac = QString::number(utime);
    frac.truncate(3);
    outs += "." + frac;
    return outs;
}

QString TimeFunc::UnixTime32ToInvString(quint32 utime, QTimeZone tz)
{
    QDateTime tn = QDateTime::fromSecsSinceEpoch(utime, tz); // in seconds
    return tn.toString("yyyy/MM/dd hh:mm:ss");
}

quint64 TimeFunc::InvStringToUnixTime32(QString utime, QTimeZone tz)
{
    Q_UNUSED(tz)
    return QDateTime::fromString(utime, "yyyy/MM/dd hh:mm:ss").toSecsSinceEpoch();
}
QTimeZone TimeFunc::userTimeZone()
{
    using namespace settings;
    auto sets = std::make_unique<QSettings>();
    QString timezone = QTimeZone::systemTimeZone().displayName(QTimeZone::StandardTime, QTimeZone::OffsetName);
    timezone = sets->value(regMap[timezoneKey].name, timezone).toString();
    if (!timezone.isEmpty())
        return QTimeZone(timezone.toUtf8());
    return QTimeZone::systemTimeZone();
}
