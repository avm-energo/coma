#include <gen/integers.h>
#include <string_view>
#include <unordered_map>

namespace Device
{

enum class Model : u16
{
    KTF = 0xA282,
    KIV = 0xA284,
    KDV = 0xA387,
    MPE = 0xC001
};

enum class BaseBoard : u8
{
    MTB_00 = 0x00,
    MTB_12 = 0x12,
    MTB_14 = 0x14,
    MTB_21 = 0x21,
    MTB_22 = 0x22,
    MTB_24 = 0x24,
    MTB_31 = 0x31,
    MTB_33 = 0x33,
    MTB_34 = 0x34,
    MTB_35 = 0x35,
    MTB_52 = 0x52,
    MTB_80 = 0x80,
    MTB_85 = 0x85,
    MTB_86 = 0x86,
    MTB_87 = 0x87,
    MTB_A1 = 0xA1,
    MTB_A2 = 0xA2
};

enum class MezzanineBoard : u8
{
    MTM_00 = 0x00,
    MTM_21 = 0x21,
    MTM_22 = 0x22,
    MTM_31 = 0x31,
    MTM_33 = 0x33,
    MTM_34 = 0x34,
    MTM_35 = 0x35,
    MTM_81 = 0x81,
    MTM_82 = 0x82,
    MTM_83 = 0x83,
    MTM_84 = 0x84,
    MTM_85 = 0x85,
    MTM_86 = 0x86,
    MTM_87 = 0x87
};

static const std::unordered_map<Model, std::string_view> BoxModules {
    { Model::KTF, "AVM-KTF" }, //
    { Model::KIV, "AVM-KIV" }, //
    { Model::KDV, "AVM-KDV" }, //
    { Model::MPE, "AVM-MPE" }, //
};

static const std::unordered_map<BaseBoard, std::string_view> BaseBoards {
    { BaseBoard::MTB_00, "АВ_ТУК-00" },       // нет базовой платы
    { BaseBoard::MTB_14, "АВ_ТУК-14" },       // alise (ЦКУ sonica)
    { BaseBoard::MTB_12, "АВ-ТУК-12 (ЦКУ)" }, // процессорный, не комбинируется
    { BaseBoard::MTB_21, "АВ-ТУК-21" },       // аналоговый ввод DC, комбинируется с 2x,3x
    { BaseBoard::MTB_22, "АВ-ТУК-22" },       // аналоговый ввод DC, комбинируется с 2x,3x
    { BaseBoard::MTB_31, "АВ-ТУК-31" },       // дискретный ввод, комбинируется с 2x,3x
    { BaseBoard::MTB_33, "АВ-ТУК-33" },       // дискретный ввод, комбинируется с 2x,3x
    { BaseBoard::MTB_34, "АВ-ТУК-34" },       // дискретный ввод, комбинируется с 2x,3x
    { BaseBoard::MTB_35, "АВ-ТУК-35" },       // дискретный вывод, комбинируется с 2x,3x
    { BaseBoard::MTB_80, "АВ-ТУК-80" },      // аналоговый ввод AC, комбинируется с 81,82,83
    { BaseBoard::MTB_85, "АВ-ТУК-85" },      // УСК, комбинируется с 85
    { BaseBoard::MTB_86, "АВ-ТУК-86" },      // комбинируется с 86 и ?
    { BaseBoard::MTB_87, "АВ-ТУК-87 (ЧР)" }, // ЧР, не комбинируется
    { BaseBoard::MTB_A1, "ПКС-1" }, // ПКДН, прибор для Туркота, не комбинируется
    { BaseBoard::MTB_A2, "АВМ" },   // МНК3, не комбинируется
};

static const std::unordered_map<MezzanineBoard, std::string_view> MezzanineBoards {
    { MezzanineBoard::MTM_00, "" },           // нет мезонина
    { MezzanineBoard::MTM_21, "21" },         // аналоговый ввод, комбинируется с 2x,3x
    { MezzanineBoard::MTM_22, "22" },         // аналоговый ввод, комбинируется с 2x,3x
    { MezzanineBoard::MTM_31, "31" },         // дискретный ввод, комбинируется с 2x,3x
    { MezzanineBoard::MTM_33, "33" },         // дискретный ввод, комбинируется с 2x,3x
    { MezzanineBoard::MTM_35, "35" },         // дискретный вывод, комбинируется с 2x,3x
    { MezzanineBoard::MTM_81, "81 (Э2Т0Н)" }, // 0U6I, комбинируется с 80,84
    { MezzanineBoard::MTM_82, "82 (Э1Т1Н)" }, // 3U3I, комбинируется с 80,84
    { MezzanineBoard::MTM_83, "83 (Э0Т2Н)" }, // 6U0I, комбинируется с 80,84
    { MezzanineBoard::MTM_84, "84" },         // 3U3I, комбинируется с 80,81
    { MezzanineBoard::MTM_85, "85 (УСК)" } // 3U3I УСК (перегрузка по току 20х), комбинируется с 85
};

} // namespace Device
