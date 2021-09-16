#pragma once
#include "../gen/datatypes.h"
///#include "../interfaces/interfacesettings.h"

#include <QDebug>
#include <bitset>

class ErrorProtocolItem;
class ETableItemData;
struct UsbHidSettings;

QDebug operator<<(QDebug debug, const DataTypes::BitStringStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::FloatWithTimeStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::FloatStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::SinglePointWithTimeStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::FileStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::BlockStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::SignalsStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::Signal &st);
QDebug operator<<(QDebug debug, const DataTypes::GeneralResponseStruct &st);
QDebug operator<<(QDebug debug, const S2DataTypes::OscInfo &st);
QDebug operator<<(QDebug debug, const UsbHidSettings &settings);

template <std::size_t N> QDebug operator<<(QDebug debug, const std::bitset<N> &bitset)
{
    debug.nospace() << "bitset:";
    for (size_t i = 0; i < bitset.size(); i++)
        debug.nospace() << quint8(bitset.test(i));

    return debug.maybeSpace();
}

QDebug operator<<(QDebug debug, const ErrorProtocolItem &item);
QDebug operator<<(QDebug debug, const ETableItemData &item);
