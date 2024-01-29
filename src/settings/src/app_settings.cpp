#include <QTimeZone>
#include <settings/app_settings.h>

namespace Settings
{

// Системный часовой пояс
const auto systemTimezone = QTimeZone::systemTimeZone().displayName(QTimeZone::StandardTime, QTimeZone::OffsetName);

const std::map<SettingType, SettingPair> ApplicationSettings::s_params {
    { SettingType::Theme, { "Theme", "Light" } },                //
    { SettingType::Timezone, { "Timezone", systemTimezone } },   //
    { SettingType::LoggingEnabled, { "LoggingEnabled", true } }, //
    { SettingType::AlarmsEnabled, { "AlarmsEnabled", true } },   //
    { SettingType::TuneCount, { "TuneCount", 20 } },             //
    { SettingType::MipIp, { "MipIp", "172.16.31.178" } },        //
    { SettingType::MipPort, { "MipPort", "2404" } },             //
    { SettingType::MipBsAddress, { "MipBsAddress", "1" } },      //
};

ApplicationSettings::ApplicationSettings(token) noexcept
{
}

const QVariant &ApplicationSettings::defaultValue(const SettingType type, const QVariant &defaultValue) const noexcept
{
    const auto search = s_params.find(type);
    if (search != s_params.cend())
        return search->second.defaultValue;
    else
        return defaultValue;
}

} // namespace Settings
