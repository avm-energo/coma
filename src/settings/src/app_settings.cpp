#include <QTimeZone>
#include <QtDebug>
#include <settings/app_settings.h>

namespace Settings
{

// Системный часовой пояс
const auto systemTimezone = QTimeZone::systemTimeZone().displayName(QTimeZone::StandardTime, QTimeZone::OffsetName);

const std::map<SettingType, SettingPair> ApplicationSettings::s_params {
    { SettingType::LoggingEnabled, { "LoggingEnabled", true } }, //
    { SettingType::Theme, { "Theme", "Light" } },                //
    { SettingType::Timezone, { "Timezone", systemTimezone } },   //
    { SettingType::AlarmsEnabled, { "AlarmsEnabled", true } },   //
    { SettingType::TuneCount, { "TuneCount", 20 } },             //
    { SettingType::MipIp, { "MipIp", "172.16.31.178" } },        //
    { SettingType::MipPort, { "MipPort", "2404" } },             //
    { SettingType::MipBsAddress, { "MipBsAddress", "1" } },      //
};

ApplicationSettings::ApplicationSettings(token) noexcept
{
}

const QString &ApplicationSettings::nameof(const SettingType type, const QString &defaultName) noexcept
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

bool ApplicationSettings::getLoggingState() noexcept
{
    return unwrap_to<bool>(getValue<SettingType::LoggingEnabled>());
}

void ApplicationSettings::setLoggingState(const bool isLogging) noexcept
{
    setValue<SettingType::LoggingEnabled>(isLogging);
}

bool ApplicationSettings::getAlarmsState() noexcept
{
    return unwrap_to<bool>(getValue<SettingType::AlarmsEnabled>());
}

void ApplicationSettings::setAlarmsState(const bool isAlarms) noexcept
{
    setValue<SettingType::AlarmsEnabled>(isAlarms);
}

QString ApplicationSettings::getTuneCount() noexcept
{
    return unwrap_to<QString>(getValue<SettingType::TuneCount>());
}

void ApplicationSettings::setTuneCount(int count) noexcept
{
    if ((count < 0) || (count > 100))
    {
        count = defaultValue(SettingType::TuneCount).toInt();
        qWarning() << QString("Неверное число степени усреднения, "
                              "установлено значение по умолчанию %1")
                          .arg(count);
    }
    setValue<SettingType::TuneCount>(count);
}

QString ApplicationSettings::getMipIp() noexcept
{
    return unwrap_to<QString>(getValue<SettingType::MipIp>());
}

void ApplicationSettings::setMipIp(const QString &ip) noexcept
{
    setValue<SettingType::MipIp>(ip);
}

QString ApplicationSettings::getMipPort() noexcept
{
    return unwrap_to<QString>(getValue<SettingType::MipPort>());
}

void ApplicationSettings::setMipPort(const QString &port) noexcept
{
    setValue<SettingType::MipPort>(port);
}

QString ApplicationSettings::getMipBsAddr() noexcept
{
    return unwrap_to<QString>(getValue<SettingType::MipBsAddress>());
}

void ApplicationSettings::setMipBsAddr(const QString &bsAddress) noexcept
{
    setValue<SettingType::MipBsAddress>(bsAddress);
}

} // namespace Settings
