#pragma once
#include "../check/delegate_check.h"
#include "../gen/modules.h"
#include "datatypes.h"
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
    S2DataTypes::valueTypeMap *s2filesMap;
    config::widgetMap *s2widgetMap;
    categoryMap *s2categories;
};

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
    QMap<Modules::AlarmType, DataTypes::Alarm> alarms;
    QMap<Modules::JournalType, DataTypes::JournalDesc> journals;
    InterfaceSettings ifaceSettings;
    Configs configSettings;
    QMultiMap<quint32, quint32> highlightWarn, highlightCrit;
    const Modules::StartupInfoBlock &startupInfoBlock;
    int interfaceType;
};

struct CheckSettings
{
    check::itemVector items;
    check::signalsVector signlsVec;
    categoryMap categories;
};

struct GlobalSettings
{
    ConfigSettings config;
    CheckSettings check;
};

namespace settings
{
constexpr char logKey[] = "WriteLog";
constexpr char logWidget[] = "writelogchb";
constexpr char tuneCountKey[] = "TuneRequestCount";
constexpr char tuneCountWidget[] = "reqcount";
constexpr char timezoneKey[] = "Timezone";
constexpr char timezoneWidget[] = "timezoneCB";
}
