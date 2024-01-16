#include "interfaces/types/usbhidsettings.h"

#include <QRegularExpression>

#ifdef QT_DEBUG
UsbHidSettings::UsbHidSettings(unsigned short dev, unsigned short pid, const QString &arr, const QString &str)
    : vendor_id(dev), product_id(pid), serial(arr), path(str)
#else
UsbHidSettings::UsbHidSettings(unsigned short dev, unsigned short pid, const QString &arr)
    : vendor_id(dev), product_id(pid), serial(arr)
#endif
{
}

bool UsbHidSettings::hasMatch(const QString &str) const
{
    if (!hasPartialMatch(str))
        return false;
    QRegularExpression regex("#((?>[a-zA-Z0-9_]+|(?R))*)#");
    QRegularExpressionMatch match = regex.match(str);
    if (!match.hasMatch())
        return false;
    QString buffer = match.captured(0);
    buffer.chop(1);
    buffer.remove(0, 1);
    Q_ASSERT(!buffer.isEmpty());
    buffer.replace("_", " ");
    return buffer == serial;
}

bool UsbHidSettings::hasPartialMatch(const QString &str) const
{
    if (str.isEmpty())
        return false;
    QRegularExpression regex("(?<=[#&]).+(?=[#&])");
    QRegularExpressionMatch match = regex.match(str);
    if (!match.hasMatch())
        return false;
    regex.setPattern("[a-zA-Z]{3}.[a-fA-F0-9]{4}");
    QString buffer = match.captured(0);
    QRegularExpressionMatchIterator i = regex.globalMatch(buffer);
    while (i.hasNext())
    {
        match = i.next();
        buffer = match.captured(0);
        const auto pair = buffer.split('_');
        bool ok = false;
        if (pair.first() == "PID")
        {
            quint16 pid = pair.at(1).toUShort(&ok, 16);
            if (!ok)
                return false;
            if (pid != product_id)
                return false;
        }
        if (pair.first() == "VID")
        {
            quint16 vid = pair.at(1).toUShort(&ok, 16);
            if (!ok)
                return false;
            if (vid != vendor_id)
                return false;
        }
    }
    return true;
}

QDebug operator<<(QDebug debug, const UsbHidSettings &settings)
{
    debug.nospace() << QString::number(settings.vendor_id, 16) << ":" << QString::number(settings.product_id, 16) << ":"
                    << settings.serial;
#ifdef QT_DEBUG
    debug.nospace() << ":" << settings.path;
#endif
    return debug.maybeSpace();
}

bool operator==(const UsbHidSettings &lhs, const UsbHidSettings &rhs)
{
    // Не сравниваем path т.к. если переподключить устройство то path может сменится но остальные параметры - нет
    return ((lhs.product_id == rhs.product_id) && (lhs.vendor_id == rhs.vendor_id) && (lhs.serial == rhs.serial));
}

bool operator!=(const UsbHidSettings &lhs, const UsbHidSettings &rhs)
{
    return !(lhs == rhs);
}
