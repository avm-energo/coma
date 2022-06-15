#pragma once
#include "../check/delegate_check.h"
#include "../gen/datatypes.h"
#include "datarecv.h"

struct InterfaceSettings
{
    QVariant settings;
};

struct ModuleType
{
    quint16 typeB;
    quint16 typeM;
};

struct Configs
{
    QList<DataTypes::RecordPair> general;
    QList<DataTypes::RecordPair> base;
    QList<DataTypes::RecordPair> mezz;
};

struct ConfigSettings
{
    DataTypes::valueMap *s2filesMap;
    config::widgetMap *s2widgetMap;
    categoryMap *s2categories;
};

using AlarmMap = QMap<Modules::AlarmType, DataTypes::Alarm>;
using JourMap = QMap<Modules::JournalType, DataTypes::JournalDesc>;

struct ModuleSettings
{
    ModuleSettings(const Modules::StartupInfoBlock &startupInfoBlock_) : startupInfoBlock(startupInfoBlock_)
    {
    }

    int alarmCount() const
    {
        int counter = 0;
        for (const auto &alarm : alarms)
            counter += alarm.desc.size();
        return counter;
    }

    AlarmMap alarms;
    JourMap journals;
    InterfaceSettings ifaceSettings;
    Configs configSettings;
    QMultiMap<quint32, quint32> highlightWarn, highlightCrit;
    const Modules::StartupInfoBlock &startupInfoBlock;
    int interfaceType;
};

struct CheckItem
{
    QString header;
    check::itemVector itemsVector;
    check::signalsVector signlsVec;
};

struct CheckSettings
{
    std::vector<CheckItem> items;
    categoryMap categories;
};

struct GlobalSettings
{
    ConfigSettings config;
    CheckSettings check;
};
