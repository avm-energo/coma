#include <settings/user_settings.h>

#include <QTimeZone>

// Системный часовой пояс
const auto systemTimezone = QTimeZone::systemTimeZone().displayName(QTimeZone::StandardTime, QTimeZone::OffsetName);
constexpr auto defaultHash = "d93fdd6d1fb5afcca939fa650b62541d09dbcb766f41c39352dc75f348fb35dc";

QMap<const UserSettings::SettingName, UserSettings::SettingStruct> UserSettings::s_settingsMap = {
    { SettingName::Theme, { "Theme", "Light" } },                     //
    { SettingName::Timezone, { "Timezone", systemTimezone } },        //
    { SettingName::PasswordHash, { "PasswordHash", defaultHash } },   //
    { SettingName::LoggingEnabled, { "LoggingEnabled", true } },      //
    { SettingName::LogLevel, { "LogLevel", "Warn" } },                //
    { SettingName::AlarmsInterval, { "AlarmsInterval", 10000 } },     //
    { SettingName::AlarmsEnabled, { "AlarmsEnabled", true } },        //
    { SettingName::SilentInterval, { "SilentInterval", 10000 } },     //
    { SettingName::TimeoutCount, { "TimeoutCount", 5 } },             //
    { SettingName::ErrorCount, { "ErrorCount", 5 } },                 //
    { SettingName::ProtocomTimeout, { "ProtocomTimeout", 1000 } },    //
    { SettingName::ProtocomReconnect, { "ProtocomReconnect", 100 } }, //
    { SettingName::ModbusTimeout, { "ModbusTimeout", 3000 } },        //
    { SettingName::ModbusReconnect, { "ModbusReconnect", 1000 } },    //
    { SettingName::Iec104Timeout, { "Iec104Timeout", 1000 } },        //
    { SettingName::Iec104Reconnect, { "Iec104Reconnect", 1000 } },    //
    { SettingName::Iec104T0, { "Iec104T0", 30 } },                    //
    { SettingName::Iec104T1, { "Iec104T1", 15 } },                    //
    { SettingName::Iec104T2, { "Iec104T2", 10 } },                    //
    { SettingName::Iec104T3, { "Iec104T3", 20 } },                    //
    { SettingName::Iec104K, { "Iec104K", 12 } },                      //
    { SettingName::Iec104W, { "Iec104W", 8 } },                       //
    { SettingName::TuneCount, { "TuneCount", 20 } },                  //
    { SettingName::MipIp, { "MipIp", "172.16.31.178" } },             //
    { SettingName::MipPort, { "MipPort", 2404 } },                    //
    { SettingName::MipBsAddress, { "MipBsAddress", 1 } },             //
    { SettingName::IpAddress, { "ipAddress", "127.0.0.1" } },         //
    { SettingName::IpPort, { "port", 2404 } },                        //
    { SettingName::Iec104BsAddress, { "bsAddress", 205 } },           //
    { SettingName::SerialPort, { "port", "COM-1" } },                 //
    { SettingName::SerialSpeed, { "speed", 115200 } },                //
    { SettingName::SerialParity, { "parity", "Нет" } },               //
    { SettingName::SerialStop, { "stop", 1 } },                       //
    { SettingName::ModbusAddress, { "address", 1 } }                  //
};

QMap<const UserSettings::SettingGroupTypes, QString> UserSettings::s_groupTypesMap
    = { { UserSettings::Ethernet, "Ethernet" }, { UserSettings::Serial, "Serial" } };

UserSettings::UserSettings() { }

UserSettings::~UserSettings() { }
