#pragma once

#include <QSettings>
#include <gen/singleton.h>
#include <gen/utils/convertable.h>
#include <settings/types.h>

namespace Settings
{

class ApplicationSettings : public Singleton<ApplicationSettings>
{
private:
    static const std::map<SettingType, SettingPair> s_params;
    QSettings m_settings;

public:
    explicit ApplicationSettings(token) noexcept;

    const QVariant &defaultValue(const SettingType type, const QVariant &defaultValue = QVariant()) const noexcept;

    /// \brief Возвращает имя настройки указанного типа.
    template <SettingType type> //
    inline const QString &nameof(const QString &defaultName = "") noexcept;

    /// \brief Возвращает значение настройки указанного типа.
    template <SettingType type> //
    inline utils::Convertable get(const QString &groupName = "settings") noexcept;

    /// \brief Записывает значение настройки указанного типа.
    template <SettingType type> //
    inline void set(const QVariant &value, const QString &groupName = "settings") noexcept;
};

template <SettingType type> //
inline const QString &ApplicationSettings::nameof(const QString &defaultName) noexcept
{
    const auto search = s_params.find(type);
    if (search != s_params.cend())
        return search->second.name;
    else
        return defaultName;
}

template <SettingType type> //
inline utils::Convertable ApplicationSettings::get(const QString &groupName) noexcept
{
    m_settings.beginGroup(groupName);
    utils::Convertable value { m_settings.value(nameof<type>(), defaultValue(type)) };
    m_settings.endGroup();
    return value;
}

template <SettingType type> //
inline void ApplicationSettings::set(const QVariant &value, const QString &groupName) noexcept
{
    m_settings.beginGroup(groupName);
    m_settings.setValue(nameof<type>(), value);
    m_settings.endGroup();
}

} // namespace Settings
