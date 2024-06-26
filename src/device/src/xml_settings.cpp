#include "device/xml_settings.h"

namespace Device::XmlDataTypes
{

void Settings::clear() noexcept
{
    m_countMap.clear();
    m_signals.clear();
    m_tabs.clear();
    m_sections.clear();
    m_alarms.clear();
    m_stateAllCfg.clear();
    m_workJournals.clear();
    m_measJournals.clear();
    m_critHighlight.clear();
    m_warnHighlight.clear();
    m_hiddenSettings.clear();
}

void Settings::appendSignal(const u32 id, const Signal sig)
{
    m_signals.insert({ id, sig });
}

void Settings::appendTab(const u32 id, const QString &tabName)
{
    m_tabs.insert(id, tabName);
}

void Settings::appendSection(const Section &section)
{
    m_sections.push_back(section);
}

void Settings::appendAlarmStateAll(const AlarmType type, const u32 index, const QString &desc)
{
    m_stateAllCfg.push_back(AlarmStateAllRecord { index, type, desc });
}

void Settings::appendAlarm(const AlarmType type, const u32 addr, const QString &desc)
{
    m_alarms[type].insert(addr, desc);
}

void Settings::appendHighlight(const AlarmType type, const u32 key, const QList<u32> &values)
{
    if (type == AlarmType::Critical)
    {
        for (auto &value : values)
            m_critHighlight.insert(key, value);
    }
    else if (type == AlarmType::Warning)
    {
        for (auto &value : values)
            m_warnHighlight.insert(key, value);
    }
}

void Settings::appendWorkJournal(const u32 id, const QString &desc)
{
    m_workJournals.insert(id, desc);
}

void Settings::appendMeasJournal(const u32 index, const QString &header, const BinaryType type, bool visib)
{
    m_measJournals.push_back(MeasJournal { index, type, visib, std::move(header) });
}

void Settings::appendHiddenTab(const HiddenTab &hiddenTab)
{
    m_hiddenSettings.push_back(hiddenTab);
}

void Settings::appendBsiExtItem(const u32 addr, const BinaryType type, bool visib, const QString &desc)
{
    m_bsiExtSettings.push_back(BsiExtItem { addr, type, visib, std::move(desc) });
}

const DetailCountMap &Settings::getDetailConfigCount() const
{
    return m_countMap;
}

const SignalMap &Settings::getSignals() const
{
    return m_signals;
}

const TabsMap &Settings::getTabs() const
{
    return m_tabs;
}

const SectionList &Settings::getSections() const
{
    return m_sections;
}

const AlarmStateAllConfig &Settings::getStateAllConfig() const
{
    return m_stateAllCfg;
}

const AlarmMap &Settings::getAlarms() const
{
    return m_alarms;
}

const HighlightMap &Settings::getHighlights(const AlarmType type) const
{
    if (type == AlarmType::Critical)
        return m_critHighlight;
    else
        return m_warnHighlight;
}

const WorkJourMap &Settings::getWorkJours() const
{
    return m_workJournals;
}

const MeasJourList &Settings::getMeasJours() const
{
    return m_measJournals;
}

const HiddenSettings &Settings::getHiddenSettings() const
{
    return m_hiddenSettings;
}

const BsiExtItemList &Settings::getBsiExtSettings() const
{
    return m_bsiExtSettings;
}

} // namespace Device::XmlDataTypes
