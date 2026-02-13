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
    // std::uint32_t m_timeout;           ///< Requirement for QueryExecutor.
    // std::uint32_t m_silentInterval;    ///< Requirement for ConnectionManager.
    // std::uint16_t m_reconnectInterval; ///< Requirement for BaseInterface.
    // std::uint16_t m_maxErrors;         ///< Requirement for ConnectionManager.
    // std::uint16_t m_maxTimeouts;       ///< Requirement for ConnectionManager.
    // bool m_isLoggingEnabled;           ///< Requirement for BaseInterface and QueryExecutor.
    // QString m_logLevel;                ///< For interface & executor

    explicit BaseSettings(QObject *parent = nullptr) noexcept;

    explicit BaseSettings(const std::uint32_t timeout, const std::uint32_t silentInt, const std::uint16_t reconnectInt,
        const std::uint16_t maxErrors, const std::uint16_t maxTimeouts, const bool loggingEnabled) noexcept;
};
