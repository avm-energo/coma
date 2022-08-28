#include "modulesettings.h"

ModuleSettings::ModuleSettings(const Modules::StartupInfoBlock &startupInfo) : startupInfoBlock(startupInfo)
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

const ModuleTypes::ConfigMap ModuleSettings::getConfigs()
{
    return mConfigs;
}

const ModuleTypes::SignalMap ModuleSettings::getSignals()
{
    return mSignals;
}

const ModuleTypes::TabsMap ModuleSettings::getTabs()
{
    return mTabs;
}

const ModuleTypes::SectionList ModuleSettings::getSections()
{
    return mSections;
}

const ModuleTypes::AlarmMap ModuleSettings::getAlarms()
{
    return mAlarms;
}

const ModuleTypes::JourMap ModuleSettings::getJours()
{
    return mJournals;
}

const ModuleTypes::ModbusList ModuleSettings::getModbus()
{
    return mModbus;
}

const ModuleTypes::ProtocomList ModuleSettings::getProtocom()
{
    return mProtocom;
}

const ModuleTypes::IecList ModuleSettings::getIec()
{
    return mIec;
}
