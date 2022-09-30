#pragma once
#include "datatypes.h"

#include <QDebug>
#include <bitset>

QDebug operator<<(QDebug debug, const DataTypes::BitStringStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::FloatWithTimeStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::FloatStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::SinglePointWithTimeStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::BlockStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::SignalsStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::Signal &st);
QDebug operator<<(QDebug debug, const DataTypes::GeneralResponseStruct &st);

template <std::size_t N> QDebug operator<<(QDebug debug, const std::bitset<N> &bitset)
{
    debug.nospace() << "bitset:";
    for (size_t i = 0; i < bitset.size(); i++)
        debug.nospace() << quint8(bitset.test(i));

    return debug.maybeSpace();
}
