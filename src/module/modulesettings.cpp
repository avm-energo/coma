#include "modulesettings.h"

/// \brief Очистка настроек модуля.
void ModuleSettings::clear()
{
    mCountMap.clear();
    mSignals.clear();
    mTabs.clear();
    mSections.clear();
    mAlarms.clear();
    workJournals.clear();
    measJournals.clear();
    critHighlight.clear();
    warnHighlight.clear();
}

/// \brief Добавление нового сигнала в список сигналов.
void ModuleSettings::appendSignal(const quint32 id, const ModuleTypes::Signal sig)
{
    mSignals.insert(id, sig);
}

/// \brief Добавление новой вкладки в список вкладок.
void ModuleSettings::appendTab(const quint32 id, const QString &tabName)
{
    mTabs.insert(id, tabName);
}

/// \brief Добавление раздела в список разделов.
void ModuleSettings::appendSection(const ModuleTypes::Section &section)
{
    mSections.push_back(section);
}

/// \brief Добавление сигнализации в список сигнализаций по заданному типу сигнализации (key).
void ModuleSettings::appendAlarm(const ModuleTypes::AlarmKey &key, const quint32 &addr, const QString &desc)
{
    mAlarms[key].insert(addr, desc);
}

/// \brief Добавление подсветки указанных полей для подсветки при появлении сигнализации.
void ModuleSettings::appendHighlight(const Modules::AlarmType &type, const quint32 &key, const QList<quint32> &values)
{
    if (type == Modules::AlarmType::Critical)
    {
        for (auto &value : values)
            critHighlight.insert(key, value);
    }
    else if (type == Modules::AlarmType::Warning)
    {
        for (auto &value : values)
            warnHighlight.insert(key, value);
    }
}

/// \brief Добавление записи в список описаний рабочего журналов.
void ModuleSettings::appendWorkJournal(const quint32 id, const QString &desc)
{
    workJournals.insert(id, desc);
}

/// \brief Добавление записи в список о формате журнала измерений.
void ModuleSettings::appendMeasJournal(const quint32 index, const QString &header, //
    const ModuleTypes::BinaryType type, bool visib)
{
    measJournals.append({ index, header, type, visib });
}

/// \brief Constant getter for detailed count hashmap for current config list.
const ModuleTypes::DetailCountMap &ModuleSettings::getDetailConfigCount() const
{
    return mCountMap;
}

/// \brief Constant getter for signals list.
const ModuleTypes::SignalMap &ModuleSettings::getSignals() const
{
    return mSignals;
}

/// \brief Constant getter for tabs list.
const ModuleTypes::TabsMap &ModuleSettings::getTabs() const
{
    return mTabs;
}

/// \brief Constant getter for sections list.
const ModuleTypes::SectionList &ModuleSettings::getSections() const
{
    return mSections;
}

/// \brief Constant getter for alarms hashmap.
const ModuleTypes::AlarmMap &ModuleSettings::getAlarms() const
{
    return mAlarms;
}

/// \brief Constant getter for alarm's highlights.
const ModuleTypes::HighlightMap &ModuleSettings::getHighlights(const Modules::AlarmType &type) const
{
    if (type == Modules::AlarmType::Critical)
        return critHighlight;
    else
        return warnHighlight;
}

/// \brief Constant getter for work journals.
const ModuleTypes::WorkJourMap &ModuleSettings::getWorkJours() const
{
    return workJournals;
}

/// \brief Constant getter for measurement journals.
const ModuleTypes::MeasJourList &ModuleSettings::getMeasJours() const
{
    return measJournals;
}
