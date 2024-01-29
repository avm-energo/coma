#pragma once

#include <QString>
#include <QVariant>
#include <cstdint>

namespace Settings
{

/// \brief Структура, хранящая имя настройки и её значение по умолчанию.
struct SettingPair
{
    QString name;          ///< Имя настройки.
    QVariant defaultValue; ///< Значение настройки по умолчанию.
};

enum SettingType
{
    Theme,
    Timezone,

    LoggingEnabled,
    AlarmsEnabled,
    ReconnectInterval,
    ProtocomTimeout,
    ModbusTimeout,
    Iec104Timeout,

    TuneCount,
    MipIp,
    MipPort,
    MipBsAddress,

    IpAddress,
    IpPort,
    Iec104BsAddress,

    SerialPort,
    SerialSpeed,
    SerialParity,
    SerialStop,
    ModbusAddress,
};

} // namespace Settings
