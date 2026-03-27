#pragma once

#include <avm-gen/memsettings.h>

#include <QObject>
#include <QString>
#include <cstdint>

/// \brief Базовая структура настроек соединения, предоставляющая характеристики, которыми обладают все соединения.
class BaseSettings : public MemSettings
{
    Q_OBJECT
public:
    explicit BaseSettings(QObject *parent = nullptr) noexcept;

    explicit BaseSettings(const std::uint32_t timeout, const std::uint32_t silentInt, const std::uint16_t reconnectInt,
        const std::uint16_t maxErrors, const std::uint16_t maxTimeouts, const bool loggingEnabled) noexcept;
};
