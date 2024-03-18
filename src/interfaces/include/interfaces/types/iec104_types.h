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

enum class Command : std::uint8_t
{
    RequestGroup = 0,
    RequestFile,
    RequestConfig,
    WriteFile,
    Command45,
    Command50,
    Command51,
    None = std::numeric_limits<std::uint8_t>::max()
};

}
