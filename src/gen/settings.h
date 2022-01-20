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

namespace settings
{
struct RegValues
{
    QString name;
    QString defValue;
};

enum SettingsKeys
{
    logKey,
    logWidget,
    tuneCountKey,
    tuneCountWidget,
    hidTimeout,
    timezoneKey,
    timezoneWidget,
    MIPIP,
    MIPAddress
};

const QMap<int, settings::RegValues> regMap { { logKey, { "WriteLog", "0" } }, { logWidget, { "writelogchb", "" } },
    { tuneCountKey, { "TuneRequestCount", "20" } }, { tuneCountWidget, { "reqcount", "" } },
    { hidTimeout, { "hidTimeout", "50" } }, { timezoneKey, { "Timezone", "0" } },
    { timezoneWidget, { "timezoneCB", "" } }, { MIPIP, { "mipIP", "172.16.11.12" } },
    { MIPAddress, { "mipAddress", "205" } } };
}
