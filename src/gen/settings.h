#pragma once
#include "datatypes.h"

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
