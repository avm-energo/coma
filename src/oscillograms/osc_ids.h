#pragma once

#include <gen/integers.h>

// Oscillogram / SwitchJournal Part

/// ID заголовка осциллограммы
constexpr int MT_HEAD_ID = 9000;
constexpr int MT_SPEC_ID = 9100;

/*
constexpr u32 ID_OSC_CH0 = 10040; // ID осциллограммы канала 0
constexpr u32 ID_OSC_CH1 = 10041; // ID осциллограммы канала 1
constexpr u32 ID_OSC_CH2 = 10042; // ID осциллограммы канала 2
constexpr u32 ID_OSC_CH3 = 10043; // ID осциллограммы канала 3
constexpr u32 ID_OSC_CH4 = 10044; // ID осциллограммы канала 4
constexpr u32 ID_OSC_CH5 = 10045; // ID осциллограммы канала 5
constexpr u32 ID_OSC_CH6 = 10046; // ID осциллограммы канала 6
constexpr u32 ID_OSC_CH7 = 10047; // ID осциллограммы канала 7

constexpr u32 ID_SPECTR_CH0 = 10050; // ID спектрограммы канала 0
constexpr u32 ID_SPECTR_CH1 = 10051; // ID спектрограммы канала 1
constexpr u32 ID_SPECTR_CH2 = 10052; // ID спектрограммы канала 2
constexpr u32 ID_SPECTR_CH3 = 10053; // ID спектрограммы канала 3
constexpr u32 ID_SPECTR_CH4 = 10054; // ID спектрограммы канала 4
constexpr u32 ID_SPECTR_CH5 = 10055; // ID спектрограммы канала 5
constexpr u32 ID_SPECTR_CH6 = 10056; // ID спектрограммы канала 6
constexpr u32 ID_SPECTR_CH7 = 10057; // ID спектрограммы канала 7
*/

namespace AVTUK_85
{
constexpr u32 SWJ_ID = 10031; ///< ID журнала переключений с осциллограммой модуля 8085/8585
constexpr u32 OSC_ID = 10030; ///< ID осциллограммы по модулю 8085/8585
} // namespace AVTUK_85

namespace AVTUK_8X
{
constexpr u32 OSC_ID = 10020; ///< ID осциллограммы по модулю 808x
} // namespace AVTUK_8X

namespace AVTUK_KIV
{
constexpr u32 OSC_ALL_ID = 10022;
constexpr u32 OSC_A_ID = 10023;
constexpr u32 OSC_B_ID = 10024;
constexpr u32 OSC_C_ID = 10025;
} // namespace AVTUK_KIV

namespace AVTUK_KDV
{
constexpr u32 OSC_ID = 10040;   ///< ID осциллограммы модуля КДВ
constexpr u32 OSCV_ID = 10041;  ///< ID осциллограммы модуля КДВ
constexpr u32 SPEC_ID = 10050;  ///< ID спектрограммы модуля КДВ
constexpr u32 SPECV_ID = 10051; ///< ID осциллограммы модуля КДВ
} // namespace AVTUK_KDV

namespace AVTUK_21
{
constexpr u32 OSC_ID_MIN = 10001; ///< Первый ID осциллограммы по модулю 21
constexpr u32 OSC_ID_MAX = 10016; ///< Последний ID осциллограммы по модулю 21
} // namespace AVTUK_21

constexpr u32 MAXOSCBUFSIZE = 262144; ///< Максимальный размер буфера для осциллограмм
