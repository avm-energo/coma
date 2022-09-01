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

ModuleTypes::ConfigMap &ModuleSettings::getConfigs()
{
    return mConfigs;
}

ModuleTypes::SignalMap &ModuleSettings::getSignals()
{
    return mSignals;
}

ModuleTypes::TabsMap &ModuleSettings::getTabs()
{
    return mTabs;
}

ModuleTypes::SectionList &ModuleSettings::getSections()
{
    return mSections;
}

ModuleTypes::AlarmMap &ModuleSettings::getAlarms()
{
    return mAlarms;
}

ModuleTypes::JourMap &ModuleSettings::getJours()
{
    return mJournals;
}

ModuleTypes::ModbusList &ModuleSettings::getModbus()
{
    return mModbus;
}

ModuleTypes::ProtocomList &ModuleSettings::getProtocom()
{
    return mProtocom;
}

ModuleTypes::IecList &ModuleSettings::getIec()
{
    return mIec;
}
