#pragma once

#include <cstdint>

/// \brief Базовая структура настроек соединения, предоставляющая характеристики, которыми обладают все соединения.
struct BaseSettings
{
public:
    std::uint32_t m_timeout;           ///< Requirement for QueryExecutor.
    std::uint32_t m_silentInterval;    ///< Requirement for ConnectionManager.
    std::uint16_t m_reconnectInterval; ///< Requirement for BaseInterface.
    std::uint16_t m_maxErrors;         ///< Requirement for ConnectionManager.
    std::uint16_t m_maxTimeouts;       ///< Requirement for ConnectionManager.
    bool m_isLoggingEnabled;           ///< Requirement for BaseInterface and QueryExecutor.

    explicit BaseSettings() noexcept
        : m_timeout(0)
        , m_silentInterval(0)
        , m_reconnectInterval(0)
        , m_maxErrors(0)
        , m_maxTimeouts(0)
        , m_isLoggingEnabled(true)
    {
    }

    explicit BaseSettings(const std::uint32_t timeout, const std::uint32_t silentInt, const std::uint16_t reconnectInt,
        const std::uint16_t maxErrors, const std::uint16_t maxTimeouts, const bool loggingEnabled) noexcept
        : m_timeout(timeout)
        , m_silentInterval(silentInt)
        , m_reconnectInterval(reconnectInt)
        , m_maxErrors(maxErrors)
        , m_maxTimeouts(maxTimeouts)
        , m_isLoggingEnabled(loggingEnabled)
    {
    }
};
