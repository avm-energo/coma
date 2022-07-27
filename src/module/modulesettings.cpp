#include "modulesettings.h"

ModuleSettings::ModuleSettings(const Modules::StartupInfoBlock &startupInfo) : startupInfoBlock(startupInfo)
{
}

void ModuleSettings::startNewConfig()
{
    m_curConfigIndex++;
    m_config[m_curConfigIndex] = ConfigList();
}

void ModuleSettings::appendToCurrentConfig(DataTypes::RecordPair pair)
{
    assert(m_config.contains(m_curConfigIndex));
    m_config[m_curConfigIndex].append(pair);
}

const ModuleSettings::ConfigMap ModuleSettings::getConfig()
{
    return m_config;
}

const ModuleSettings::TabsMap ModuleSettings::getTabs()
{
    return m_tabs;
}

const ModuleSettings::CheckMap ModuleSettings::getCheck()
{
    return m_checkMap;
}
