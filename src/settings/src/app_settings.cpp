#include <QTimeZone>
#include <settings/app_settings.h>

namespace Settings
{

// Системный часовой пояс
const auto systemTimezone = QTimeZone::systemTimeZone().displayName(QTimeZone::StandardTime, QTimeZone::OffsetName);

const std::map<SettingType, SettingPair> ApplicationSettings::s_params {
    { SettingType::LoggingEnabled, { "LoggingEnabled", true } }, //
    { SettingType::Theme, { "Theme", "Light" } },                //
    { SettingType::Timezone, { "Timezone", systemTimezone } },   //
    { SettingType::AlarmsEnabled, { "AlarmsEnabled", true } }    //
};

ApplicationSettings::ApplicationSettings(token) noexcept
{
}

const QString &ApplicationSettings::settingName(const SettingType type, const QString &defaultName) noexcept
{
    const auto search = s_params.find(type);
    if (search != s_params.cend())
        return search->second.name;
    else
        return defaultName;
}

const QVariant &ApplicationSettings::defaultValue(const SettingType type, const QVariant &defaultValue) noexcept
{
    const auto search = s_params.find(type);
    if (search != s_params.cend())
        return search->second.defaultValue;
    else
        return defaultValue;
}

const SettingPair &ApplicationSettings::operator[](const SettingType type)
{
    return s_params.at(type);
}

QString ApplicationSettings::getTimezone() noexcept
{
    return unwrap_to<QString>(getValue<SettingType::Timezone>());
}

void ApplicationSettings::setTimezone(const QString &timezone) noexcept
{
    setValue<SettingType::Timezone>(timezone);
}

QString ApplicationSettings::getTheme() noexcept
{
    return unwrap_to<QString>(getValue<SettingType::Theme>());
}

void ApplicationSettings::setTheme(const QString &theme) noexcept
{
    setValue<SettingType::Theme>(theme);
}

bool ApplicationSettings::getLoggingEnabled() noexcept
{
    return unwrap_to<bool>(getValue<SettingType::LoggingEnabled>());
}

void ApplicationSettings::setLoggingEnabled(const bool isLogging) noexcept
{
    setValue<SettingType::LoggingEnabled>(isLogging);
}

bool ApplicationSettings::getAlarmsEnabled() noexcept
{
    return unwrap_to<bool>(getValue<SettingType::AlarmsEnabled>());
}

void ApplicationSettings::setAlarmsEnabled(const bool isAlarms) noexcept
{
    setValue<SettingType::LoggingEnabled>(isAlarms);
}

} // namespace Settings
