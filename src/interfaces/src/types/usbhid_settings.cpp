#include "interfaces/types/usbhid_settings.h"

#include <QRegularExpression>

UsbHidSettings::UsbHidSettings(QObject *parent) : BaseSettings(parent)
{
    set("vendor_id", 0x00);
    set("product_id", 0x00);
    set("serial", "00000000");
    set("path", "");
}

UsbHidSettings::UsbHidSettings(unsigned short dev, unsigned short pid, const QString &arr, const QString &str)
{
    set("vendor_id", dev);
    set("product_id", pid);
    set("serial", arr);
    set("path", str);
}

// bool UsbHidSettings::hasMatch(const QString &str) const
// {
//     if (!hasPartialMatch(str))
//         return false;
//     QRegularExpression regex("#((?>[a-zA-Z0-9_]+|(?R))*)#");
//     QRegularExpressionMatch match = regex.match(str);
//     if (!match.hasMatch())
//         return false;
//     QString buffer = match.captured(0);
//     buffer.chop(1);
//     buffer.remove(0, 1);
//     Q_ASSERT(!buffer.isEmpty());
//     buffer.replace("_", " ");
//     return buffer == serial;
// }

// bool UsbHidSettings::hasPartialMatch(const QString &str) const
// {
//     if (str.isEmpty())
//         return false;
//     QRegularExpression regex("(?<=[#&]).+(?=[#&])");
//     QRegularExpressionMatch match = regex.match(str);
//     if (!match.hasMatch())
//         return false;
//     regex.setPattern("[a-zA-Z]{3}.[a-fA-F0-9]{4}");
//     QString buffer = match.captured(0);
//     QRegularExpressionMatchIterator i = regex.globalMatch(buffer);
//     while (i.hasNext())
//     {
//         match = i.next();
//         buffer = match.captured(0);
//         const auto pair = buffer.split('_');
//         bool ok = false;
//         if (pair.first() == "PID")
//         {
//             quint16 pid = pair.at(1).toUShort(&ok, 16);
//             if (!ok)
//                 return false;
//             if (pid != product_id)
//                 return false;
//         }
//         if (pair.first() == "VID")
//         {
//             quint16 vid = pair.at(1).toUShort(&ok, 16);
//             if (!ok)
//                 return false;
//             if (vid != vendor_id)
//                 return false;
//         }
//     }
//     return true;
// }

QDebug operator<<(QDebug debug, UsbHidSettings *settings)
{
    debug.nospace() << QString::number(settings->get<u16>("vendor_id"), 16) << ":"
                    << QString::number(settings->get<u16>("product_id"), 16) << ":" << QString(settings->get("serial"));
#ifdef QT_DEBUG
    debug.nospace() << ":" << QString(settings->get("path"));
#endif
    return debug.maybeSpace();
}

bool operator==(UsbHidSettings &lhs, UsbHidSettings &rhs)
{
    // Не сравниваем path т.к. если переподключить устройство то path может сменится но остальные параметры - нет
    return ((lhs.get<u16>("product_id") == rhs.get<u16>("product_id"))
        && (static_cast<u16>(lhs.get("product_id")) == static_cast<u16>(rhs.get("product_id")))
        && (lhs.get<QString>("serial") == rhs.get<QString>("serial")));
}

bool operator!=(UsbHidSettings &lhs, UsbHidSettings &rhs)
{
    return !(lhs == rhs);
}
