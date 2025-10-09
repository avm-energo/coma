#pragma once

#include <QMap>
#include <QPair>

namespace Names
{
}

namespace SettingsKeys
{
namespace USB
{
    constexpr auto prefix = "protocom";
    constexpr auto protocomTimeout = "protocomTimeout";
    constexpr auto protocomReconnect = "protocomReconnect";
}
namespace Serial
{
    constexpr auto prefix = "modbus";
    constexpr auto modbusTimeout = "modbusTimeout";
    constexpr auto modbusReconnect = "modbusReconnect";
}
namespace Iec104
{
    constexpr auto prefix = "iec104";
    constexpr auto iec104Timeout = "iec104Timeout";
    constexpr auto iec104Reconnect = "iec104Reconnect";
    constexpr auto iec104T0 = "iec104T0";
    constexpr auto iec104T1 = "iec104T1";
    constexpr auto iec104T2 = "iec104T2";
    constexpr auto iec104T3 = "iec104T3";
    constexpr auto iec104K = "iec104K";
    constexpr auto iec104W = "iec104W";
}
namespace Mip
{
    constexpr auto mipIp = "mipIp";
    constexpr auto mipPort = "mipPort";
    constexpr auto mipBsAddress = "mipBsAddress";
}
constexpr auto psw = "password";
constexpr auto passwordHash = "passwordHash";
constexpr auto theme = "theme";
constexpr auto timezone = "timezone";
constexpr auto loggingEnabled = "loggingEnabled";
constexpr auto logLevel = "logLevel";
constexpr auto alarmsInterval = "alarmsInterval";
constexpr auto alarmsEnabled = "alarmsEnabled";
constexpr auto timeoutCount = "timeoutCount";
constexpr auto silentInterval = "silentInterval";
constexpr auto errorCount = "errorCount";
constexpr auto tuneCount = "tuneCount";
}

namespace MemKeys
{
namespace USB
{
    constexpr auto vendor_id = "vendor_id";
    constexpr auto product_id = "product_id";
    constexpr auto serial = "serial";
    constexpr auto path = "path";
}
namespace Serial
{
    constexpr auto address = "address";
}
namespace Iec104
{
    constexpr auto bsAddress = "bsAddress";
}
constexpr auto timeout = "timeout";
constexpr auto reconnectInterval = "reconnectInterval";
constexpr auto silentInterval = "silentInterval";
constexpr auto maxErrors = "maxErrors";
}

// QMap<SettingsKey, MemKey>
const QMap<QString, QString> KeysMap { { SettingsKeys::USB::protocomTimeout, MemKeys::timeout } };
