#include "modulesettings.h"

ModuleSettings::ModuleSettings(const Modules::StartupInfoBlock &startupInfo) : startupInfoBlock(startupInfo)
{
}

void ModuleSettings::startNewConfig()
{
    curConfigIndex++;
    configs[curConfigIndex] = ModuleTypes::ConfigList();
}

void ModuleSettings::appendToCurrentConfig(DataTypes::RecordPair pair)
{
    assert(configs.contains(curConfigIndex));
    configs[curConfigIndex].append(pair);
}

const ModuleTypes::ConfigMap ModuleSettings::getConfig()
{
    return configs;
}

const ModuleTypes::TabsMap ModuleSettings::getTabs()
{
    return tabs;
}

const ModuleTypes::SectionList ModuleSettings::getSectionList()
{
    return sections;
}
