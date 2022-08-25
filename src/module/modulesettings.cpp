#include "modulesettings.h"

ModuleSettings::ModuleSettings(const Modules::StartupInfoBlock &startupInfo) : startupInfoBlock(startupInfo)
{
}

void ModuleSettings::startNewConfig()
{
    curConfigIndex++;
    configs[curConfigIndex] = ConfigList();
}

void ModuleSettings::appendToCurrentConfig(DataTypes::RecordPair pair)
{
    assert(configs.contains(curConfigIndex));
    configs[curConfigIndex].append(pair);
}

const ModuleSettings::ConfigMap ModuleSettings::getConfig()
{
    return configs;
}

const ModuleSettings::TabsMap ModuleSettings::getTabs()
{
    return tabs;
}

const ModuleSettings::SectionList ModuleSettings::getSectionList()
{
    return sections;
}
