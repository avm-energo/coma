#include "usbhidsettings.h"

#include <QRegularExpression>
#ifdef Q_OS_WINDOWS
bool UsbHidSettings::hasMatch(const QString &str)
{
    //(?<=[a-zA-Z]{3}.)[a-fA-F0-9]+
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

bool UsbHidSettings::hasPartialMatch(const QString &str)
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
#endif

QDebug operator<<(QDebug debug, const UsbHidSettings &settings)
{
    debug.nospace() << QString::number(settings.vendor_id, 16) << ":" << QString::number(settings.product_id, 16) << ":"
                    << settings.serial;
#ifdef QT_DEBUG
    debug.nospace() << ":" << settings.path;
#endif
    return debug.maybeSpace();
}
