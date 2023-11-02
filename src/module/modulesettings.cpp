#include "modulesettings.h"

void ModuleSettings::clear()
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
}

void ModuleSettings::appendSignal(const quint32 id, const ModuleTypes::Signal sig)
{
    m_signals.insert(id, sig);
}

void ModuleSettings::appendTab(const quint32 id, const QString &tabName)
{
    m_tabs.insert(id, tabName);
}

void ModuleSettings::appendSection(const ModuleTypes::Section &section)
{
    m_sections.push_back(section);
}

void ModuleSettings::appendAlarm(const ModuleTypes::AlarmKey &key, const quint32 &addr, const QString &desc)
{
    m_alarms[key].insert(addr, desc);
}

void ModuleSettings::appendHighlight(const Modules::AlarmType &type, const quint32 &key, const QList<quint32> &values)
{
    if (type == Modules::AlarmType::Critical)
    {
        for (auto &value : values)
            m_critHighlight.insert(key, value);
    }
    else if (type == Modules::AlarmType::Warning)
    {
        for (auto &value : values)
            m_warnHighlight.insert(key, value);
    }
}

void ModuleSettings::appendWorkJournal(const quint32 id, const QString &desc)
{
    m_workJournals.insert(id, desc);
}

void ModuleSettings::appendMeasJournal(const quint32 index, const QString &header, //
    const ModuleTypes::BinaryType type, bool visib)
{
    m_measJournals.push_back({ index, header, type, visib });
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

const ModuleTypes::HighlightMap &ModuleSettings::getHighlights(const Modules::AlarmType &type) const
{
    if (type == Modules::AlarmType::Critical)
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
