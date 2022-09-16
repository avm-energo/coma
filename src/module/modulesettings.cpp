#include "modulesettings.h"

ModuleSettings::ModuleSettings() : curConfigIndex(0)
{
}

void ModuleSettings::startNewConfig()
{
    curConfigIndex++;
    mConfigs[curConfigIndex] = ModuleTypes::ConfigList();
}

void ModuleSettings::appendToCurrentConfig(DataTypes::RecordPair pair)
{
    assert(mConfigs.contains(curConfigIndex));
    mConfigs[curConfigIndex].append(pair);
}

const ModuleTypes::ConfigMap &ModuleSettings::getConfigMap() const
{
    return mConfigs;
}

ModuleTypes::ConfigList &ModuleSettings::getConfigs()
{
    return mConfigs[curConfigIndex];
}

const ModuleTypes::ConfigList ModuleSettings::getConfigs() const
{
    return mConfigs[curConfigIndex];
}

ModuleTypes::SignalMap &ModuleSettings::getSignals()
{
    return mSignals;
}

const ModuleTypes::SignalMap &ModuleSettings::getSignals() const
{
    return mSignals;
}

ModuleTypes::TabsMap &ModuleSettings::getTabs()
{
    return mTabs;
}

const ModuleTypes::TabsMap &ModuleSettings::getTabs() const
{
    return mTabs;
}

ModuleTypes::SectionList &ModuleSettings::getSections()
{
    return mSections;
}

const ModuleTypes::SectionList &ModuleSettings::getSections() const
{
    return mSections;
}

ModuleTypes::AlarmMap &ModuleSettings::getAlarms()
{
    return mAlarms;
}

const ModuleTypes::AlarmMap &ModuleSettings::getAlarms() const
{
    return mAlarms;
}

ModuleTypes::JourMap &ModuleSettings::getJours()
{
    return mJournals;
}

const ModuleTypes::JourMap &ModuleSettings::getJours() const
{
    return mJournals;
}

const ModuleTypes::InterfaceSettings &ModuleSettings::getInterfaceSettings() const
{
    return mIfaceSettings;
}

void ModuleSettings::setInterfaceSettings(const ModuleTypes::InterfaceSettings &settings)
{
    mIfaceSettings = settings;
}
