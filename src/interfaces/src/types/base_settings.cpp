#include <interfaces/types/base_settings.h>

BaseSettings::BaseSettings(QObject *parent) noexcept : MemSettings(parent)
{
    set("timeout", 0);
    set("silentInterval", 0);
    set("reconnectInterval", 0);
    set("maxErrors", 0);
    set("maxTimeouts", 0);
    set("isLoggingEnabled", true);
    set("logLevel", "Warn");
}

BaseSettings::BaseSettings(const uint32_t timeout, const uint32_t silentInt, const uint16_t reconnectInt,
    const uint16_t maxErrors, const uint16_t maxTimeouts, const bool loggingEnabled) noexcept
{
    set("timeout", timeout);
    set("silentInterval", silentInt);
    set("reconnectInterval", reconnectInt);
    set("maxErrors", maxErrors);
    set("maxTimeouts", maxTimeouts);
    set("isLoggingEnabled", loggingEnabled);
    set("logLevel", "Warn");
}
