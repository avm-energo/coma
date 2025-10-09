#pragma once

#include <QString>

/// \brief Enumeration for application configuration.
class AppConfiguration
{
public:
    enum Config
    {
        Debug = false,
        Service = true
    };

    static Config m_appConfig;

    explicit AppConfiguration() {};
    static Config app();
    static void setApp(Config config);
    static bool notDenied(const QString &attr);

private:
};
