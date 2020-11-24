#pragma once
#include "../gen/datatypes.h"
#include "../interfaces/usbhidportinfo.h"

#include <QDebug>

QDebug operator<<(QDebug debug, const DataTypes::BitStringStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::FloatWithTimeStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::FloatStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::SinglePointWithTimeStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::FileStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::ConfParameterStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::BlockStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::ConfParametersListStruct &list);
QDebug operator<<(QDebug debug, const DataTypes::SignalsStruct &st);
QDebug operator<<(QDebug debug, const DataTypes::Signal &st);
QDebug operator<<(QDebug debug, const DataTypes::GeneralResponseStruct &st);
QDebug operator<<(QDebug debug, const UsbHidSettings &settings);
