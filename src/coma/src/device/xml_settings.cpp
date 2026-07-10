#include "device/xml_settings.h"

namespace Device::XmlDataTypes
{

void XmlSettings::clear() noexcept
{
    m_featuresMap.clear();
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
    m_bsiExtSettings.clear();
    m_bsi.clear();
    m_bsiExt.clear();
    m_haveBsiExt = false;
    m_haveMeasJournal = false;
    m_haveWorkJournal = false;
}

void XmlSettings::appendFeature(const QString &key, const QString &value)
{
    m_featuresMap[key] = value;
}

void XmlSettings::appendSignal(const u32 id, const Signal sig)
{
    m_signals.insert({ id, sig });
}

void XmlSettings::appendTab(const u32 id, const SectionTabStruct &sectionTab)
{
    m_tabs.insert(id, sectionTab);
}

void XmlSettings::appendSection(const Section &section)
{
    m_sections.push_back(section);
}

void XmlSettings::appendAlarmStateAll(const AlarmType type, const u32 index, const QString &desc)
{
    m_stateAllCfg.push_back(AlarmStateAllRecord { index, type, desc });
}

void XmlSettings::appendAlarm(const AlarmType type, const u32 addr, const QString &desc, const u32 order)
{
    AlarmOne oneAlarm { addr, desc };
    m_alarms[type].insert(order, oneAlarm);
}

void XmlSettings::appendHighlight(const AlarmType type, const u32 key, const QList<u32> &values)
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

void XmlSettings::appendWorkJournal(const u32 id, const QString &desc)
{
    m_haveWorkJournal = true;
    m_workJournals.insert(id, desc);
}

void XmlSettings::appendMeasJournal(const u32 index, const QString &header, const BinaryType type, bool visib)
{
    m_haveMeasJournal = true;
    m_measJournals.push_back(MeasJournal { index, type, visib, std::move(header) });
}

void XmlSettings::appendHiddenTab(const HiddenTab &hiddenTab)
{
    m_hiddenSettings.push_back(hiddenTab);
}

void XmlSettings::appendBsiExtItem(const u32 addr, const BinaryType type, bool visib, const QString &desc)
{
    m_haveBsiExt = true;
    m_bsiExtSettings.push_back(BsiExtItem { addr, type, visib, std::move(desc) });
}

void XmlSettings::appendBsi(const QString &name, const QString &desc, ViewType::ViewTypes type, u32 offset)
{
    m_bsi.push_back(BsiRecord { name, desc, type, offset });
}

void XmlSettings::appendBsiExt(const QString &name, const QString &desc, ViewType::ViewTypes type, u32 offset)
{
    m_haveBsiExt = true;
    m_bsiExt.push_back(BsiExtRecord { name, desc, type, offset });
}

const FeaturesMap &XmlSettings::getFeatures() const
{
    return m_featuresMap;
}

const DetailCountMap &XmlSettings::getDetailConfigCount() const
{
    return m_countMap;
}

const SignalMap &XmlSettings::getSignals() const
{
    return m_signals;
}

const TabsMap &XmlSettings::getTabs() const
{
    return m_tabs;
}

const SectionList &XmlSettings::getSections() const
{
    return m_sections;
}

const AlarmStateAllConfig &XmlSettings::getStateAllConfig() const
{
    return m_stateAllCfg;
}

const AlarmMap &XmlSettings::getAlarms() const
{
    return m_alarms;
}

const HighlightMap &XmlSettings::getHighlights(const AlarmType type) const
{
    if (type == AlarmType::Critical)
        return m_critHighlight;
    else
        return m_warnHighlight;
}

const WorkJourMap &XmlSettings::getWorkJours() const
{
    return m_workJournals;
}

const MeasJourList &XmlSettings::getMeasJours() const
{
    return m_measJournals;
}

const HiddenSettings &XmlSettings::getHiddenSettings() const
{
    return m_hiddenSettings;
}

const BsiExtItemList &XmlSettings::getBsiExtSettings() const
{
    return m_bsiExtSettings;
}

const BsiSettings &XmlSettings::getBsi() const
{
    return m_bsi;
}

const BsiExtSettings &XmlSettings::getBsiExt() const
{
    return m_bsiExt;
}

bool XmlSettings::HaveBSIExt() const
{
    return m_haveBsiExt;
}

bool XmlSettings::HaveMeasJournal() const
{
    return m_haveMeasJournal;
}

bool XmlSettings::HaveWorkJournal() const
{
    return m_haveWorkJournal;
}

bool XmlSettings::HaveTrue() const
{
    return true;
}

} // namespace Device::XmlDataTypes
