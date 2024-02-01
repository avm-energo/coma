#include <QTimeZone>
#include <settings/user_settings.h>

namespace Settings
{

// Системный часовой пояс
const auto systemTimezone = QTimeZone::systemTimeZone().displayName(QTimeZone::StandardTime, QTimeZone::OffsetName);

const std::map<SettingType, SettingPair> UserSettings::s_params {
    { SettingType::Theme, { "Theme", "Light" } },                     //
    { SettingType::Timezone, { "Timezone", systemTimezone } },        //
    { SettingType::LoggingEnabled, { "LoggingEnabled", true } },      //
    { SettingType::AlarmsInterval, { "AlarmsInterval", 10000 } },     //
    { SettingType::AlarmsEnabled, { "AlarmsEnabled", true } },        //
    { SettingType::SilentInterval, { "SilentInterval", 10000 } },     //
    { SettingType::TimeoutCount, { "TimeoutCount", 5 } },             //
    { SettingType::ErrorCount, { "ErrorCount", 5 } },                 //
    { SettingType::ProtocomTimeout, { "ProtocomTimeout", 1000 } },    //
    { SettingType::ProtocomReconnect, { "ProtocomReconnect", 100 } }, //
    { SettingType::ModbusTimeout, { "ModbusTimeout", 3000 } },        //
    { SettingType::ModbusReconnect, { "ModbusReconnect", 1000 } },    //
    { SettingType::Iec104Timeout, { "Iec104Timeout", 1000 } },        //
    { SettingType::Iec104Reconnect, { "Iec104Reconnect", 1000 } },    //
    { SettingType::Iec104T0, { "Iec104T0", 30 } },                    //
    { SettingType::Iec104T1, { "Iec104T1", 15 } },                    //
    { SettingType::Iec104T2, { "Iec104T2", 10 } },                    //
    { SettingType::Iec104T3, { "Iec104T3", 20 } },                    //
    { SettingType::Iec104K, { "Iec104K", 12 } },                      //
    { SettingType::Iec104W, { "Iec104W", 8 } },                       //
    { SettingType::TuneCount, { "TuneCount", 20 } },                  //
    { SettingType::MipIp, { "MipIp", "172.16.31.178" } },             //
    { SettingType::MipPort, { "MipPort", 2404 } },                    //
    { SettingType::MipBsAddress, { "MipBsAddress", 1 } },             //
    { SettingType::IpAddress, { "ipAddress", "127.0.0.1" } },         //
    { SettingType::IpPort, { "port", 2404 } },                        //
    { SettingType::Iec104BsAddress, { "bsAddress", 205 } },           //
    { SettingType::SerialPort, { "port", "COM-1" } },                 //
    { SettingType::SerialSpeed, { "speed", 115200 } },                //
    { SettingType::SerialParity, { "parity", "Нет" } },               //
    { SettingType::SerialStop, { "stop", 1 } },                       //
    { SettingType::ModbusAddress, { "address", 1 } },                 //
};

UserSettings::UserSettings(token) noexcept
{
}

QSettings &UserSettings::native() noexcept
{
    return m_settings;
}

bool UserSettings::isExist(const QString &name) noexcept
{
    return m_settings.childGroups().contains(name);
}

void UserSettings::remove(const QString &name) noexcept
{
    if (isExist(name))
        m_settings.remove(name);
}

void UserSettings::switchTo(const QString &name) noexcept
{
    m_settings.endGroup();
    m_settings.beginGroup(name);
}

const QVariant &UserSettings::defaultValue(const SettingType type, const QVariant &defaultValue) const noexcept
{
    const auto search = s_params.find(type);
    if (search != s_params.cend())
        return search->second.defaultValue;
    else
        return defaultValue;
}

} // namespace Settings
