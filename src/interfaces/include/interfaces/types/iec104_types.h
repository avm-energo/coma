#pragma once

#include <interfaces/types/common_types.h>
#include <interfaces/types/iec104/apci.h>
#include <interfaces/types/iec104/asdu.h>

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

struct ConnectionSettings
{
    std::uint8_t t0 = 30; ///< Тайм-аут при установке соединения.
    std::uint8_t t1 = 15; ///< Тайм-аут при посылке или тестировании APDU.
    std::uint8_t t2 = 10; ///< Тайм-аут для подтверждения в случае отсутствия сообщения с данными.
    std::uint8_t t3 = 20; ///< Тайм-аут для посылки блоков тестирования в случае долгого простоя.
    std::uint16_t k = 12; ///<
    std::uint16_t w = 8;  ///<
};

}
