#include <common/appconfig.h>

AppConfiguration::Config AppConfiguration::m_appConfig = AppConfiguration::Service;

AppConfiguration::Config AppConfiguration::app()
{
    return m_appConfig;
}

void AppConfiguration::setApp(Config config)
{
    m_appConfig = config;
}

bool AppConfiguration::notDenied(const QString &attr)
{
    return ((AppConfiguration::app() == AppConfiguration::Debug) || //
        ((AppConfiguration::app() == AppConfiguration::Service) &&  //
            (attr.compare("Yes", Qt::CaseInsensitive) != 0)));
}
