#pragma once

#include <QSettings>
#include <gen/singleton.h>
#include <gen/utils/convertable.h>
#include <settings/types.h>

namespace Settings
{

/// \brief Класс, предоставляющий возможность обрабатывать и хранить пользовательские настройки.
class UserSettings final : public Singleton<UserSettings>
{
private:
    static const std::map<SettingType, SettingPair> s_params;
    QSettings m_settings;

public:
    explicit UserSettings(token) noexcept;

    /// \brief Возвращает ссылку на хранимый эксземпляр QSettings для прямого доступа к данным.
    QSettings &native() noexcept;
    /// \brief Проверяет, существует ли параметр с указанным именем.
    bool isExist(const QString &name) noexcept;
    /// \brief Удаляет параметр с указанным именем.
    void remove(const QString &name) noexcept;
    /// \brief Функция для переключения с текущей группы настроек на другую.
    void switchTo(const QString &name) noexcept;

    /// \brief Возвращает значение по умолчанию настройки указанного типа.
    const QVariant &defaultValue(const SettingType type, const QVariant &defaultValue = QVariant()) const noexcept;

    /// \brief Возвращает имя настройки указанного типа.
    template <SettingType type> //
    inline const QString &nameof(const QString &defaultName = "") const noexcept;

    /// \brief Возвращает значение настройки указанного типа.
    template <SettingType type> //
    inline utils::Convertable get() const noexcept
    {
        return utils::Convertable { m_settings.value(nameof<type>(), defaultValue(type)) };
    }

    /// \brief Записывает значение настройки указанного типа.
    template <SettingType type> //
    inline void set(const QVariant &value) noexcept
    {
        m_settings.setValue(nameof<type>(), value);
    }
};

template <SettingType type> //
inline const QString &UserSettings::nameof(const QString &defaultName) const noexcept
{
    const auto search = s_params.find(type);
    if (search != s_params.cend())
        return search->second.name;
    else
        return defaultName;
}

/// \brief Структура для использования определённой группы настроек в рамках скоупа кода.
struct ScopedSettingsGroup final
{
private:
    QSettings &m_settings;

public:
    explicit ScopedSettingsGroup(UserSettings &settings, const QString &groupName) noexcept
        : m_settings(settings.native())
    {
        m_settings.beginGroup(groupName);
    }

    ~ScopedSettingsGroup() noexcept
    {
        m_settings.endGroup();
    }
};

} // namespace Settings
