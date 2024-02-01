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
    PasswordHash,

    LoggingEnabled,
    AlarmsInterval,
    AlarmsEnabled,
    SilentInterval,
    TimeoutCount,
    ErrorCount,

    ProtocomTimeout,
    ProtocomReconnect,
    ModbusTimeout,
    ModbusReconnect,
    Iec104Timeout,
    Iec104Reconnect,
    Iec104T0,
    Iec104T1,
    Iec104T2,
    Iec104T3,
    Iec104K,
    Iec104W,

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
