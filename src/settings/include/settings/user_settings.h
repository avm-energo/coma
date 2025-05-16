#pragma once

#include <gen/settings.h>
#include <gen/utils/convertable.h>

class UserSettings
{
public:
    enum SettingName
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

    enum SettingGroupTypes
    {
        Ethernet,
        Serial
    };

private:
    struct SettingStruct
    {
        QString name;
        QVariant defValue;
    };
    static QMap<const SettingName, SettingStruct> s_settingsMap;
    static QMap<const SettingGroupTypes, QString> s_groupTypesMap;

public:
    UserSettings();
    ~UserSettings();
    /// \brief Get user setting value by enum
    inline static utils::Convertable get(SettingName name)
    {
        return Settings::get(s_settingsMap[name].name, s_settingsMap[name].defValue);
    }

    /// \brief Set user setting value by enum
    inline static void set(SettingName name, const QVariant &value)
    {
        Settings::set(s_settingsMap[name].name, value);
    }

    inline static void setDefValue(SettingName name)
    {
        Settings::set(s_settingsMap[name].name, s_settingsMap[name].defValue);
    }

    inline static utils::Convertable getDefValue(SettingName name)
    {
        return utils::Convertable { s_settingsMap[name].defValue };
    }
};
