#ifndef MODULESETTINGS_H
#define MODULESETTINGS_H

#include "../gen/datatypes.h"
#include "../s2/datarecv.h"
#include "modules.h"

#include <QHash>

namespace ModuleTypes
{
/// Config settings ///

using ConfigList = QList<DataTypes::RecordPair>;
using ConfigMap = QHash<quint32, ConfigList>;

/// Signal settings ///

/// \brief Перечисление для типа сигналов
using SignalType = DataTypes::SignalTypes;

/// \brief Структура для хранения информации узла <signal> из XML
struct Signal
{
    quint32 startAddr;
    quint16 count;
    SignalType sigType;
};

using SignalMap = QHash<quint32, Signal>;

/// Section settings ///

// key (quint32) - id вкладки
// value (QString) - имя вкладки
using TabsMap = QHash<quint32, QString>; ///< Хранит узлы <tab> секции <section-tabs>

/// \brief Структура для хранения информации узла <mwidget> из XML
struct MWidget
{
    QString desc;                 ///< атрибут "desc"
    quint32 startAddr;            ///< узел <start-addr>
    quint32 count = 1;            ///< узел <count>
    QString tooltip = "";         ///< узел <toolTip>
    QStringList subItemList = {}; ///< узел <string-array>
};

/// \brief Структура для хранения информации узла <sgroup> из XML
struct SGroup
{
    QString name;           ///< атрибут "header"
    QList<MWidget> widgets; ///< узлы <mwidget>
};

// key (quint32) - id вкладки
// value (список SGroup) - список SGroup
using SGMap = QMultiHash<quint32, SGroup>;

/// \brief Структура для хранения информации узла <section> из XML
struct Section
{
    QString name; ///< атрибут "header"
    SGMap sgMap;  ///< узлы <sgroup>
};

using SectionList = QList<Section>; ///< Хранит узлы <section> секции <sections>

/// Alarms settings ///

using HighlightMap = QMultiMap<quint32, quint32>;
// Info: DataTypes::Alarm - deprecated [see template.xml]
using AlarmKey = std::pair<bool, Modules::AlarmType>; ///< bool - isBase, Modules::AlarmType - тип сигнализации
using AlarmValue = QHash<quint32, QString>; ///< quint32 - адрес сигнализации, QString - нода <desc> (описание)
using AlarmMap = QHash<AlarmKey, AlarmValue>; ///< Хранит узлы <item> секции <alarms>

/// Journals settings ///

/// \brief Структура для хранения информации узла <item> в <journals>
struct Journal
{
    quint32 addr; ///< узел <addr>
    QString desc; ///< узел <desc>
};

// Info: DataTypes::JournalDesc - deprecated [see template.xml]
using JourMap = QHash<Modules::JournalType, QList<Journal>>; ///< Хранит узлы <item> секции <journals>

/// Interface settings ///

/// \brief Структура для хранения информации о протоколах protocom, modbus и iec104
struct InterfaceSettings
{
    QVariant settings;
};

}

class ModuleSettings
{
public:
    ModuleSettings();
    void startNewConfig();
    void appendToCurrentConfig(DataTypes::RecordPair pair);

    const ModuleTypes::ConfigMap &getConfigMap() const;
    const ModuleTypes::ConfigList getConfigs() const;
    const ModuleTypes::SignalMap &getSignals() const;
    const ModuleTypes::TabsMap &getTabs() const;
    const ModuleTypes::SectionList &getSections() const;
    const ModuleTypes::AlarmMap &getAlarms() const;
    const ModuleTypes::JourMap &getJours() const;
    const ModuleTypes::InterfaceSettings &getInterfaceSettings() const;

    ModuleTypes::ConfigList &getConfigs();
    ModuleTypes::SignalMap &getSignals();
    ModuleTypes::TabsMap &getTabs();
    ModuleTypes::SectionList &getSections();
    ModuleTypes::AlarmMap &getAlarms();
    ModuleTypes::JourMap &getJours();
    void setInterfaceSettings(const ModuleTypes::InterfaceSettings &settings);

private:
    ModuleTypes::HighlightMap critHighlight, warnHighlight;
    quint32 curConfigIndex;

    ModuleTypes::ConfigMap mConfigs;
    ModuleTypes::SignalMap mSignals;
    ModuleTypes::TabsMap mTabs;
    ModuleTypes::SectionList mSections;
    ModuleTypes::AlarmMap mAlarms;
    ModuleTypes::JourMap mJournals;
    ModuleTypes::InterfaceSettings mIfaceSettings;
};

#endif // MODULESETTINGS_H
