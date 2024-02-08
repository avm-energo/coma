#include "device/modulesettings.h"

void ModuleSettings::clear() noexcept
{
    m_countMap.clear();
    m_signals.clear();
    m_tabs.clear();
    m_sections.clear();
    m_alarms.clear();
    m_workJournals.clear();
    m_measJournals.clear();
    m_critHighlight.clear();
    m_warnHighlight.clear();
    m_hiddenSettings.clear();
}

void ModuleSettings::appendSignal(const u32 id, const ModuleTypes::Signal sig)
{
    m_signals.insert({ id, sig });
}

void ModuleSettings::appendTab(const u32 id, const QString &tabName)
{
    m_tabs.insert(id, tabName);
}

void ModuleSettings::appendSection(const ModuleTypes::Section &section)
{
    m_sections.push_back(section);
}

void ModuleSettings::appendAlarm(const ModuleTypes::AlarmType key, const u32 addr, const QString &desc)
{
    m_alarms[key].insert(addr, desc);
}

void ModuleSettings::appendHighlight(const ModuleTypes::AlarmType type, const u32 key, const QList<u32> &values)
{
    if (type == ModuleTypes::AlarmType::Critical)
    {
        for (auto &value : values)
            m_critHighlight.insert(key, value);
    }
    else if (type == ModuleTypes::AlarmType::Warning)
    {
        for (auto &value : values)
            m_warnHighlight.insert(key, value);
    }
}

void ModuleSettings::appendWorkJournal(const u32 id, const QString &desc)
{
    m_workJournals.insert(id, desc);
}

void ModuleSettings::appendMeasJournal(const u32 index, const QString &header, //
    const ModuleTypes::BinaryType type, bool visib)
{
    m_measJournals.push_back(ModuleTypes::MeasJournal { index, type, visib, header });
}

void ModuleSettings::appendHiddenTab(const ModuleTypes::HiddenTab &hiddenTab)
{
    m_hiddenSettings.push_back(hiddenTab);
}

const ModuleTypes::DetailCountMap &ModuleSettings::getDetailConfigCount() const
{
    return m_countMap;
}

const ModuleTypes::SignalMap &ModuleSettings::getSignals() const
{
    return m_signals;
}

const ModuleTypes::TabsMap &ModuleSettings::getTabs() const
{
    return m_tabs;
}

const ModuleTypes::SectionList &ModuleSettings::getSections() const
{
    return m_sections;
}

const ModuleTypes::AlarmMap &ModuleSettings::getAlarms() const
{
    return m_alarms;
}

const ModuleTypes::HighlightMap &ModuleSettings::getHighlights(const ModuleTypes::AlarmType type) const
{
    if (type == ModuleTypes::AlarmType::Critical)
        return m_critHighlight;
    else
        return m_warnHighlight;
}

const ModuleTypes::WorkJourMap &ModuleSettings::getWorkJours() const
{
    return m_workJournals;
}

const ModuleTypes::MeasJourList &ModuleSettings::getMeasJours() const
{
    return m_measJournals;
}

const ModuleTypes::HiddenSettings &ModuleSettings::getHiddenSettings() const
{
    return m_hiddenSettings;
}
