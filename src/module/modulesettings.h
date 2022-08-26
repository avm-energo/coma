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
using ConfigMap = QHash<int, ConfigList>;

/// Section settings ///

/// \brief Контейнер для хранения информации узла <mwidget> из XML
struct MWidget
{
    QString desc;                 ///< атрибут "desc"
    quint32 startAddr;            ///< узел <start-addr>
    quint32 count = 1;            ///< узел <count>
    QString tooltip = "";         ///< узел <toolTip>
    QStringList subItemList = {}; ///< узел <string-array>
};

/// \brief Контейнер для хранения информации узла <sgroup> из XML
struct SGroup
{
    quint32 tabId;          ///< атрибут "tab"
    QString name;           ///< атрибут "header"
    QList<MWidget> widgets; ///< узлы <mwidget>
};

// key (int) - id вкладки
// value (список SGroup) - список SGroup
using SGMap = QHash<int, QList<SGroup>>;

/// \brief Контейнер для хранения информации узла <section> из XML
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

/// \brief Контейнер для хранения информации узла <item> в <alarms>
struct Alarm
{
    quint32 addr; ///< узел <addr>
    QString desc; ///< узел <desc>
};

using HighlightMap = QMultiMap<quint32, quint32>;
// Info: DataTypes::Alarm - deprecated [see template.xml]
using AlarmMap = QMap<Modules::AlarmType, QList<Alarm>>; ///< Хранит узлы <item> секции <alarms>

/// Journals settings ///

/// \brief Контейнер для хранения информации узла <item> в <journals>
struct Journal
{
    quint32 addr; ///< узел <addr>
    QString desc; ///< узел <desc>
};

// Info: DataTypes::JournalDesc - deprecated [see template.xml]
using JourMap = QMap<Modules::JournalType, QList<Journal>>; ///< Хранит узлы <item> секции <journals>
}

class ModuleSettings
{
public:
    void startNewConfig();
    void appendToCurrentConfig(DataTypes::RecordPair pair);
    const ModuleTypes::ConfigMap getConfig();

    const ModuleTypes::TabsMap getTabs();
    const ModuleTypes::SectionList getSectionList();

    ModuleSettings(const Modules::StartupInfoBlock &startupInfo);

    // TODO: А где Modbus, Protocom, IEC60870 ?

private:
    const Modules::StartupInfoBlock &startupInfoBlock;
    ModuleTypes::ConfigMap configs;
    ModuleTypes::HighlightMap critHighlight, warnHighlight;
    ModuleTypes::TabsMap tabs;
    ModuleTypes::SectionList sections;
    ModuleTypes::AlarmMap alarms;
    ModuleTypes::JourMap journals;
    int curConfigIndex;
};

#endif // MODULESETTINGS_H
