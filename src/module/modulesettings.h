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

/// \brief Структура для хранения информации узла <signal> из XML
struct Signal
{
    quint64 startAddr;
    quint16 count;
};

using SignalMap = QHash<quint32, Signal>;

/// Section settings ///

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
    quint32 tabId;          ///< атрибут "tab"
    QString name;           ///< атрибут "header"
    QList<MWidget> widgets; ///< узлы <mwidget>
};

// key (int) - id вкладки
// value (список SGroup) - список SGroup
using SGMap = QHash<int, QList<SGroup>>;

/// \brief Структура для хранения информации узла <section> из XML
struct Section
{
    QString name; ///< атрибут "header"
    SGMap sgMap;  ///< узлы <sgroup>
};

// key (int) - id вкладки
// value (QString) - имя вкладки
using TabsMap = QHash<int, QString>; ///< Хранит узлы <tab> секции <section-tabs>
using SectionList = QList<Section>;  ///< Хранит узлы <section> секции <sections>

/// Alarms settings ///

/// \brief Структура для хранения информации узла <item> в <alarms>
struct Alarm
{
    quint32 addr; ///< узел <addr>
    QString desc; ///< узел <desc>
};

using HighlightMap = QMultiMap<quint32, quint32>;
// Info: DataTypes::Alarm - deprecated [see template.xml]
using AlarmMap = QMap<Modules::AlarmType, QList<Alarm>>; ///< Хранит узлы <item> секции <alarms>

/// Journals settings ///

/// \brief Структура для хранения информации узла <item> в <journals>
struct Journal
{
    quint32 addr; ///< узел <addr>
    QString desc; ///< узел <desc>
};

// Info: DataTypes::JournalDesc - deprecated [see template.xml]
using JourMap = QMap<Modules::JournalType, QList<Journal>>; ///< Хранит узлы <item> секции <journals>

/// Modbus settings ///

/// \brief Структура для хранения информации узла <group> из <modbus>
struct ModbusItem
{
    quint32 signalId;             ///< узел <signal-id>
    quint16 regType;              ///< узел <reg-type>
    ctti::unnamed_type_id_t type; ///< узел <type>
};

using ModbusList = QList<ModbusItem>; ///< Хранит узлы <group> секции <modbus>

/// Protocom settings ///

/// \brief Структура для хранения информации узла <group> из <protocom>
struct ProtocomItem
{
    quint32 signalId; ///< узел <signal-id>
    quint32 block;    ///< узел <block>
};

using ProtocomList = QList<ProtocomItem>; ///< Хранит узлы <group> секции <protocom>

/// IEC 60870-5-104 settings ///

/// \brief Структура для хранения информации узла <group> из <iec60870>
struct IecItem
{
    quint32 signalId;  ///< узел <signal-id>
    quint16 sigType;   ///< узел <sig-type>
    quint16 transType; ///< узел <trans-type>
    quint16 sigGrioup; ///< узел <sig-group>
};

using IecList = QList<IecItem>; ///< Хранит узлы <group> секции <iec60870>
}

class ModuleSettings
{
public:
    ModuleSettings(const Modules::StartupInfoBlock &startupInfo);

    void startNewConfig();
    void appendToCurrentConfig(DataTypes::RecordPair pair);

    ModuleTypes::ConfigMap &getConfigs();
    ModuleTypes::SignalMap &getSignals();
    ModuleTypes::TabsMap &getTabs();
    ModuleTypes::SectionList &getSections();
    ModuleTypes::AlarmMap &getAlarms();
    ModuleTypes::JourMap &getJours();
    ModuleTypes::ModbusList &getModbus();
    ModuleTypes::ProtocomList &getProtocom();
    ModuleTypes::IecList &getIec();

    const auto &getStartupInfoBlock()
    {
        return startupInfoBlock;
    }

private:
    const Modules::StartupInfoBlock &startupInfoBlock;
    ModuleTypes::HighlightMap critHighlight, warnHighlight;

    ModuleTypes::ConfigMap mConfigs;
    ModuleTypes::SignalMap mSignals;
    ModuleTypes::TabsMap mTabs;
    ModuleTypes::SectionList mSections;
    ModuleTypes::AlarmMap mAlarms;
    ModuleTypes::JourMap mJournals;
    ModuleTypes::ModbusList mModbus;
    ModuleTypes::ProtocomList mProtocom;
    ModuleTypes::IecList mIec;

    int curConfigIndex;
};

#endif // MODULESETTINGS_H
