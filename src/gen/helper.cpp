#include "helper.h"

#include "../interfaces/usbhidportinfo.h"
#ifdef QT_GUI_LIB
#include "../models/errorprotocolitem.h"
#endif
#include "timefunc.h"
QDebug operator<<(QDebug debug, const DataTypes::BitStringStruct &st)
{
    debug.nospace() << QString::number(st.sigAdr) << ":" << QString::number(st.sigVal, 16) << ":"
                    << QString::number(st.sigQuality, 16);
    return debug.maybeSpace();
}

QDebug operator<<(QDebug debug, const DataTypes::FloatWithTimeStruct &st)
{
    debug.nospace() << QString::number(st.sigAdr) << ":" << QString::number(st.sigVal) << ":"
                    << QString::number(st.sigQuality, 16) << ":" << QString::number(st.CP56Time, 16);
    return debug.maybeSpace();
}

QDebug operator<<(QDebug debug, const DataTypes::FloatStruct &st)
{
    debug.nospace() << QString::number(st.sigAdr) << ":" << QString::number(st.sigVal);
    return debug.maybeSpace();
}

QDebug operator<<(QDebug debug, const DataTypes::SinglePointWithTimeStruct &st)
{
    debug.nospace() << QString::number(st.sigAdr) << ":" << QString::number(st.sigVal, 16) << ":"
                    << QString::number(st.CP56Time, 16);
    return debug.maybeSpace();
}

QDebug operator<<(QDebug debug, const DataTypes::FileStruct &st)
{
    debug.nospace() << QString::number(st.ID) << ":"
                    << "File size: " << QString::number(st.data.size());
    return debug.maybeSpace();
}

QDebug operator<<(QDebug debug, const DataTypes::BlockStruct &st)
{
    debug.nospace() << QString::number(st.ID) << ":" << st.data.toHex();
    return debug.maybeSpace();
}

QDebug operator<<(QDebug debug, const DataTypes::SignalsStruct &st)
{
    debug.nospace() << st.type << ":" << st.data;
    return debug.maybeSpace();
}

QDebug operator<<(QDebug debug, const DataTypes::Signal &st)
{
    debug.nospace() << QString::number(st.addr, 16) << ":" << QString::number(st.value, 16);
    return debug.maybeSpace();
}

QDebug operator<<(QDebug debug, const DataTypes::GeneralResponseStruct &st)
{
    debug.nospace() << st.type << ":" << QString::number(st.data, 16);
    return debug.maybeSpace();
}

QDebug operator<<(QDebug debug, const S2DataTypes::OscInfo &st)
{

    debug.nospace() << st.typeHeader.id << ":" << st.typeHeader.numByte << ":" << st.id << ":"
                    << TimeFunc::UnixTime64ToString(st.unixtime) << ":" << st.idOsc0;
    return debug.maybeSpace();
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
#ifdef QT_GUI_LIB
QDebug operator<<(QDebug debug, const ErrorProtocolItem &item)
{
    const auto &values = item.constData();
    for (const auto i : *values)
        debug.nospace() << *i << ":";
    return debug.maybeSpace();
}

QDebug operator<<(QDebug debug, const ETableItemData &item)
{
    debug.nospace() << item.value << ":" << item.role;
    return debug.maybeSpace();
}
#endif
