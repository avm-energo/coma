#include "helper.h"

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
    debug.nospace() << QString::number(st.filenum) << ":"
                    << "File size: " << QString::number(st.filedata.size());
    return debug.maybeSpace();
}

QDebug operator<<(QDebug debug, const DataTypes::ConfParameterStruct &st)
{
    debug.nospace() << QString::number(st.ID) << ":" << st.data.toHex();
    return debug.maybeSpace();
}

QDebug operator<<(QDebug debug, const DataTypes::BlockStruct &st)
{
    debug.nospace() << QString::number(st.ID, 16) << ":" << st.data.toHex();
    return debug.maybeSpace();
}

QDebug operator<<(QDebug debug, const DataTypes::ConfParametersListStruct &list)
{
    for (const auto &st : list)
        debug.nospace() << st;
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

QDebug operator<<(QDebug debug, const UsbHidSettings &settings)
{
    debug.nospace() << QString::number(settings.vendor_id, 16) << ":" << QString::number(settings.product_id, 16) << ":"
                    << settings.serial << ":" << settings.path;
    return debug.maybeSpace();
}