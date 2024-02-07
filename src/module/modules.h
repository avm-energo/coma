#pragma once
#include <QMap>
#include <gen/pch.h>

/**
 *  Перечисление для хранения списка приборов
 */

namespace Modules
{

Q_NAMESPACE

enum Model
{
    KTF = 0xA282,
    KIV = 0xA284,
    KDV = 0xA387,
    MPE = 0xC001
};
Q_ENUM_NS(Model)

enum BaseBoard : quint16
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

enum MezzanineBoard : quint16
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

const QMap<quint16, QString> BaseBoards {
    { MTB_00, "АВ_ТУК-00" },       // нет базовой платы
    { MTB_14, "АВ_ТУК-14" },       // alise (ЦКУ sonica)
    { MTB_12, "АВ-ТУК-12 (ЦКУ)" }, // процессорный, не комбинируется
    { MTB_21, "АВ-ТУК-21" },       // аналоговый ввод DC, комбинируется с 2x,3x
    { MTB_22, "АВ-ТУК-22" },       // аналоговый ввод DC, комбинируется с 2x,3x
    { MTB_31, "АВ-ТУК-31" },       // дискретный ввод, комбинируется с 2x,3x
    { MTB_33, "АВ-ТУК-33" },       // дискретный ввод, комбинируется с 2x,3x
    { MTB_34, "АВ-ТУК-34" },       // дискретный ввод, комбинируется с 2x,3x
    { MTB_35, "АВ-ТУК-35" },       // дискретный вывод, комбинируется с 2x,3x
    { MTB_80, "АВ-ТУК-80" },       // аналоговый ввод AC, комбинируется с 81,82,83
    { MTB_85, "АВ-ТУК-85" },       // УСК, комбинируется с 85
    { MTB_86, "АВ-ТУК-86" },       // комбинируется с 86 и ?
    { MTB_87, "АВ-ТУК-87 (ЧР)" },  // ЧР, не комбинируется
    { MTB_A1, "ПКС-1" },           // ПКДН, прибор для Туркота, не комбинируется
    { MTB_A2, "АВМ" },             // МНК3, не комбинируется
};

const QMap<quint16, QString> MezzanineBoards {
    { MTM_00, "" },           // нет мезонина
    { MTM_21, "21" },         // аналоговый ввод, комбинируется с 2x,3x
    { MTM_22, "22" },         // аналоговый ввод, комбинируется с 2x,3x
    { MTM_31, "31" },         // дискретный ввод, комбинируется с 2x,3x
    { MTM_33, "33" },         // дискретный ввод, комбинируется с 2x,3x
    { MTM_35, "35" },         // дискретный вывод, комбинируется с 2x,3x
    { MTM_81, "81 (Э2Т0Н)" }, // 0U6I, комбинируется с 80,84
    { MTM_82, "82 (Э1Т1Н)" }, // 3U3I, комбинируется с 80,84
    { MTM_83, "83 (Э0Т2Н)" }, // 6U0I, комбинируется с 80,84
    { MTM_84, "84" },         // 3U3I, комбинируется с 80,81
    { MTM_85, "85 (УСК)" }    // 3U3I УСК (перегрузка по току 20х), комбинируется с 85
};

enum AlarmType
{
    Info,
    Warning,
    Critical
};

struct StartupInfoBlock
{
    quint32 MTypeB;
    quint32 MTypeM;
    quint32 HwverB;
    quint32 Fwver;
    quint32 Rst;
    quint32 RstCount;
    quint32 UIDLow;
    quint32 UIDMid;
    quint32 UIDHigh;
    quint32 SerialNumB;
    quint32 SerialNumM;
    quint32 HwverM;
    quint32 SerialNum;
    quint32 Cfcrc;
    quint32 Hth;

    bool isOutdated(const quint32 &configVersion) const
    {
        return Fwver < configVersion;
    }

    quint16 type() const
    {
        return quint16((MTypeB << 8) + MTypeM);
    }
};

struct StartupInfoBlockExt0
{
    quint32 NamePO;
    quint32 VerLoader;
    quint32 POVDT;
    quint32 POVCRCB;
    quint32 POVCRCM;
};

}
constexpr int MAXERRORFLAGNUM = 32;

constexpr int HTH_FNC = 0x00000200;     // частота не в допуске (Э)
constexpr int HTH_LS = 0x00000100;      // сигналы малы (Э)
constexpr int HTH_CONFIG = 0x00000080;  // в модуле нет конфигурации
constexpr int HTH_REGPARS = 0x00000040; // в модуле нет настроечных параметров
constexpr int HTH_ADC = 0x00000020;     // проблема с АЦП (Э)
constexpr int HTH_1PPS = 0x00000010;    // нет сигнала 1PPS
constexpr int HTH_REL = 0x00000008;     // неисправность выходных реле (Д)
constexpr int HTH_TUPP = 0x00000004;    // перегрев модуля

namespace AVM
{

const QStringList Health {
    "ADCI", //
    "FLS",  //
    "TUP",  //
    "ADCB", //
    "1PPS", //
    "ADCM", //
    "REGB", //
    "RCN",  //
    "HWIB", //
    "HWIM", //
    "REGM", //
    "BAT",  //
    "NTP",  //
    "FLS2", //
    "FRM"   //
};

const QStringList HthToolTip {
    "Проблемы со встроенным АЦП ",                                                      //
    "Не работает внешняя flash-память",                                                 //
    "Перегрев",                                                                         //
    "Проблемы с АЦП (нет связи) (базовая)",                                             //
    "Нет сигнала 1PPS с антенны",                                                       //
    "Проблемы с АЦП (нет связи) (мезонин)",                                             //
    "Ошибка регулировочных коэффициентов (базовая)",                                    //
    "Ошибка загрузки конфигурации из flash-памяти. Работает конфигурация по умолчанию", //
    "Некорректная Hardware информация (базовая)",                                       //
    "Некорректная Hardware информация (мезонин)",                                       //
    "Ошибка регулировочных коэффициентов (мезонин)",                                    //
    "Напряжение батареи низко (< 2,5 В)",                                               //
    "Нет связи с NTP-сервером",                                                         //
    "Не работает внешняя flash-память (мезонин)",                                       //
    "Не работает внешняя fram",                                                         //
    "Проблемы с RTC "                                                                   //
};

}

// Oscillogram / SwitchJournal Part

/// ID заголовка осциллограммы
constexpr int MT_HEAD_ID = 9000;
constexpr int MT_SPEC_ID = 9100;

/*constexpr int ID_OSC_CH0 = 10040; // ID осциллограммы канала 0
constexpr int ID_OSC_CH1 = 10041; // ID осциллограммы канала 1
constexpr int ID_OSC_CH2 = 10042; // ID осциллограммы канала 2
constexpr int ID_OSC_CH3 = 10043; // ID осциллограммы канала 3
constexpr int ID_OSC_CH4 = 10044; // ID осциллограммы канала 4
constexpr int ID_OSC_CH5 = 10045; // ID осциллограммы канала 5
constexpr int ID_OSC_CH6 = 10046; // ID осциллограммы канала 6
constexpr int ID_OSC_CH7 = 10047; // ID осциллограммы канала 7

constexpr int ID_SPECTR_CH0 = 10050; // ID спектрограммы канала 0
constexpr int ID_SPECTR_CH1 = 10051; // ID спектрограммы канала 1
constexpr int ID_SPECTR_CH2 = 10052; // ID спектрограммы канала 2
constexpr int ID_SPECTR_CH3 = 10053; // ID спектрограммы канала 3
constexpr int ID_SPECTR_CH4 = 10054; // ID спектрограммы канала 4
constexpr int ID_SPECTR_CH5 = 10055; // ID спектрограммы канала 5
constexpr int ID_SPECTR_CH6 = 10056; // ID спектрограммы канала 6
constexpr int ID_SPECTR_CH7 = 10057; // ID спектрограммы канала 7 */

namespace AVTUK_85
{
constexpr quint32 SWJ_ID = 10031; ///< ID журнала переключений с осциллограммой модуля 8085/8585
constexpr quint32 OSC_ID = 10030; ///< ID осциллограммы по модулю 8085/8585
}

namespace AVTUK_8X
{
constexpr quint32 OSC_ID = 10020; ///< ID осциллограммы по модулю 808x
}

namespace AVTUK_KDV
{
constexpr quint32 OSC_ID = 10040;   ///< ID осциллограммы модуля КДВ
constexpr quint32 OSCV_ID = 10041;  ///< ID осциллограммы модуля КДВ
constexpr quint32 SPEC_ID = 10050;  ///< ID спектрограммы модуля КДВ
constexpr quint32 SPECV_ID = 10051; ///< ID осциллограммы модуля КДВ
}

namespace AVTUK_21
{
constexpr quint32 OSC_ID_MIN = 10001; ///< Первый ID осциллограммы по модулю 21
constexpr quint32 OSC_ID_MAX = 10016; ///< Последний ID осциллограммы по модулю 21
}

constexpr quint32 MAXOSCBUFSIZE = 262144; ///< Максимальный размер буфера для осциллограмм
