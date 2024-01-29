#pragma once

#include <QString>
#include <QVariant>
#include <cstdint>

namespace Settings
{

/// \brief Структура, хранящая имя настройки и её значение по умолчанию.
struct SettingPair
{
    QString name;          ///< Имя настройки.
    QVariant defaultValue; ///< Значение настройки по умолчанию.
};

enum SettingType
{
    Theme,
    Timezone,
    LoggingEnabled,
    AlarmsEnabled,
    TuneCount,
    MipIp,
    MipPort,
    MipBsAddress,
};

} // namespace Settings
