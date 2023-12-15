#pragma once

#include <interfaces/types/common_types.h>
#include <interfaces/types/iec104/apci.h>
#include <interfaces/types/iec104/asdu_unpacker.h>

constexpr quint32 BSIGROUP = 1;
constexpr quint32 STARTUPGROUP = 2;
constexpr quint32 ALARMGROUP = 3;
constexpr quint32 MAINFLOATGROUP = 4;
constexpr quint32 MAINBITSTRINGGROUP = 4;
constexpr quint32 TIMEGROUP = 15;

namespace Iec104
{

enum Commands
{
    // New
    RequestGroup,
    RequestFile,
    RequestConfigFile,
    WriteFile,
    COM45,
    COM50,
    COM51,
    // Old
    CM104_REQGROUP,
    CM104_COM51,
    CM104_COM45,
    CM104_COM50,
    CM104_REQFILE,
    CM104_REQCONFIGFILE,
    CM104_WRITEFILE
};

}
