#pragma once

#include <QSettings>
#include <gen/singleton.h>
#include <settings/types.h>

namespace Settings
{

class ApplicationSettings : public Singleton<ApplicationSettings>
{
private:
    static const std::map<SettingType, SettingPair> s_params;
    QSettings m_settings;

    /// \brief Возвращает значение настройки указанного типа.
    template <SettingType type> //
    inline QVariant getValue() noexcept;

    /// \brief Записывает значение настройки указанного типа.
    template <SettingType type> //
    inline void setValue(const QVariant &value) noexcept;

    /// \brief Шаблонная функция для получения данных, хранимых в типе QVariant.
    template <typename T> //
    inline T unwrap_to(const QVariant &variant) noexcept;

public:
    explicit ApplicationSettings(token) noexcept;

    const QString &nameof(const SettingType type, const QString &defaultName = "") noexcept;
    const QVariant &defaultValue(const SettingType type, const QVariant &defaultValue = QVariant()) noexcept;
    const SettingPair &operator[](const SettingType type);

    QString getTimezone() noexcept;
    void setTimezone(const QString &timezone) noexcept;

    QString getTheme() noexcept;
    void setTheme(const QString &theme) noexcept;

    bool getLoggingState() noexcept;
    void setLoggingState(const bool isLogging) noexcept;

    bool getAlarmsState() noexcept;
    void setAlarmsState(const bool isAlarms) noexcept;

    QString getTuneCount() noexcept;
    void setTuneCount(int count) noexcept;

    QString getMipIp() noexcept;
    void setMipIp(const QString &ip) noexcept;

    QString getMipPort() noexcept;
    void setMipPort(const QString &port) noexcept;

    QString getMipBsAddr() noexcept;
    void setMipBsAddr(const QString &bsAddress) noexcept;
};

template <SettingType type> //
inline QVariant ApplicationSettings::getValue() noexcept
{
    m_settings.beginGroup("settings");
    auto value = m_settings.value(nameof(type), defaultValue(type));
    m_settings.endGroup();
    return value;
}

template <SettingType type> //
inline void ApplicationSettings::setValue(const QVariant &value) noexcept
{
    m_settings.beginGroup("settings");
    m_settings.setValue(nameof(type), value);
    m_settings.endGroup();
}

/// \brief Преобразует данные variant в строку.
template <> inline QString ApplicationSettings::unwrap_to(const QVariant &variant) noexcept
{
    return variant.toString();
}

/// \brief Преобразует данные variant в переменную типа bool.
template <> inline bool ApplicationSettings::unwrap_to(const QVariant &variant) noexcept
{
    return variant.toBool();
}

/// \brief Преобразует данные variant в переменную типа int.
template <> inline int ApplicationSettings::unwrap_to(const QVariant &variant) noexcept
{
    return variant.toInt();
}

} // namespace Settings
