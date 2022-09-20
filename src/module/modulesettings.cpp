#include "modulesettings.h"

ModuleSettings::ModuleSettings() : curConfigIndex(0)
{
}

void ModuleSettings::startNewConfig()
{
    curConfigIndex++;
    mConfigs[curConfigIndex] = ModuleTypes::ConfigList();
}

void ModuleSettings::appendToCurrentConfig(const DataTypes::RecordPair &pair)
{
    if (mConfigs.contains(curConfigIndex))
        mConfigs[curConfigIndex].append(pair);
}

void ModuleSettings::appendSignal(const quint32 &id, const ModuleTypes::Signal &sig)
{
    mSignals.insert(id, sig);
}

void ModuleSettings::appendTab(const quint32 &id, const QString &tabName)
{
    mTabs.insert(id, tabName);
}

void ModuleSettings::appendSection(const ModuleTypes::Section &section)
{
    mSections.push_back(section);
}

void ModuleSettings::appendAlarm(const ModuleTypes::AlarmKey &key, const quint32 &addr, const QString &desc)
{
    mAlarms[key].insert(addr, desc);
}

void ModuleSettings::appendJournal(const Modules::JournalType &key, const ModuleTypes::Journal &journal)
{
    mJournals[key].push_back(journal);
}

void ModuleSettings::setInterfaceSettings(const ModuleTypes::InterfaceSettings &settings)
{
    mIfaceSettings = settings;
}

const ModuleTypes::ConfigMap &ModuleSettings::getConfigMap() const
{
    return mConfigs;
}

const ModuleTypes::ConfigList ModuleSettings::getConfigs() const
{
    return mConfigs[curConfigIndex];
}

const ModuleTypes::SignalMap &ModuleSettings::getSignals() const
{
    return mSignals;
}

const ModuleTypes::TabsMap &ModuleSettings::getTabs() const
{
    return mTabs;
}

const ModuleTypes::SectionList &ModuleSettings::getSections() const
{
    return mSections;
}

const ModuleTypes::AlarmMap &ModuleSettings::getAlarms() const
{
    return mAlarms;
}

const ModuleTypes::JourMap &ModuleSettings::getJours() const
{
    return mJournals;
}

const ModuleTypes::InterfaceSettings &ModuleSettings::getInterfaceSettings() const
{
    return mIfaceSettings;
}
