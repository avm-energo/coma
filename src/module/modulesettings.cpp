#include "modulesettings.h"

ModuleSettings::ModuleSettings() : curConfigIndex(-1)
{
}

/// \brief Очистка настроек модуля.
void ModuleSettings::clear()
{
    mConfigs.clear();
    mCountMap.clear();
    mSignals.clear();
    mTabs.clear();
    mSections.clear();
    mAlarms.clear();
    mJournals.clear();
    mIfaceSettings.settings.clear();
    critHighlight.clear();
    warnHighlight.clear();
    curConfigIndex = 0;
}

/// \brief Создание нового списка конфигурации.
void ModuleSettings::startNewConfig()
{
    curConfigIndex++;
    mConfigs[curConfigIndex] = ModuleTypes::ConfigList();
}

/// \brief Добавление конфига в текущий список конфигурации.
void ModuleSettings::appendToCurrentConfig(const DataTypes::RecordPair &pair)
{
    if (mConfigs.contains(curConfigIndex))
        mConfigs[curConfigIndex].append(pair);
}

/// \brief Добавление уточнённого числа элементов для виджета в разделе конфигурирования.
void ModuleSettings::appendDetailCount(const quint16 &id, const quint16 &count)
{
    mCountMap.insert(id, count);
}

/// \brief Добавление нового сигнала в список сигналов.
void ModuleSettings::appendSignal(const quint32 &id, const ModuleTypes::Signal &sig)
{
    mSignals.insert(id, sig);
}

/// \brief Добавление новой вкладки в список вкладок.
void ModuleSettings::appendTab(const quint32 &id, const QString &tabName)
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

/// \brief Добавление жрунала в список журанлов по заданному типу журнала (key).
void ModuleSettings::appendJournal(const Modules::JournalType &key, const ModuleTypes::Journal &journal)
{
    mJournals[key].push_back(journal);
}

/// \brief Сохранение настроек интерфейса, по которому подключен модуль.
void ModuleSettings::setInterfaceSettings(const ModuleTypes::InterfaceSettings &settings)
{
    mIfaceSettings = settings;
}

/// \brief Constant getter for configs hashmap.
const ModuleTypes::ConfigMap &ModuleSettings::getConfigMap() const
{
    return mConfigs;
}

/// \brief Constant getter for current config list from configs hashmap.
const ModuleTypes::ConfigList ModuleSettings::getConfigs() const
{
    return mConfigs[curConfigIndex];
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

/// \brief Constant getter for journals hashmap.
const ModuleTypes::JourMap &ModuleSettings::getJours() const
{
    return mJournals;
}

/// \brief Constant getter for interface settings of current module connection.
const ModuleTypes::InterfaceSettings &ModuleSettings::getInterfaceSettings() const
{
    return mIfaceSettings;
}
