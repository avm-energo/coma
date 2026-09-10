#pragma once

// SettingsKeys::{USB,Serial,Iec104}, MemKeys and KeysMap describe the settings
// consumed by the transport layer itself and are defined there. This header
// extends those namespaces with the keys that are specific to the application.
#include <interfaces/types/settings_keys.h>

#include <QMap>
#include <QPair>

namespace Names
{
}

namespace SettingsKeys
{
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
constexpr auto popupLifetime = "popupLifetime";
}
