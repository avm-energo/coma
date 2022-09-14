#pragma once
#include "../gen/pch.h"

#include <QMap>

/**
 *  Перечисление для хранения списка приборов
 */

namespace Modules
{

Q_NAMESPACE

enum Model
{
    KTF = 0xA287,
    KIV = 0xA284,
    KDV = 0xA387
};
Q_ENUM_NS(Model);

enum BaseBoard : quint16
{
    MTB_00 = 0x00,
    MTB_12 = 0x12,
    MTB_14 = 0x14,
    MTB_21 = 0x21,
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
    { MTB_00, "АВ_ТУК-00" }, // нет базовой платы
    { MTB_14, "АВ_ТУК-14" }, // alise (ЦКУ sonica)
    //        {MTB_12,{"АВ-ТУК-12 (ЦКУ)", 0x1200}}, // процессорный, не комбинируется
    { MTB_21, "АВ-ТУК-21" }, // аналоговый ввод DC, комбинируется с 2x,3x
    // { MTB_22, "АВ-ТУК-22" },     // аналоговый ввод DC, комбинируется с 2x,3x
    { MTB_31, "АВ-ТУК-31" },      // дискретный ввод, комбинируется с 2x,3x
    { MTB_33, "АВ-ТУК-33" },      // дискретный ввод, комбинируется с 2x,3x
    { MTB_34, "АВ-ТУК-34" },      // дискретный ввод, комбинируется с 2x,3x
    { MTB_35, "АВ-ТУК-35" },      // дискретный вывод, комбинируется с 2x,3x
    { MTB_80, "АВ-ТУК-80" },      // аналоговый ввод AC, комбинируется с 81,82,83
    { MTB_85, "АВ-ТУК-85" },      // УСК, комбинируется с 85
    { MTB_86, "АВ-ТУК-86" },      // комбинируется с 86 и ?
    { MTB_87, "АВ-ТУК-87 (ЧР)" }, // ЧР, не комбинируется
    { MTB_A1, "ПКС-1" },          // ПКДН, прибор для Туркота, не комбинируется
    { MTB_A2, "АВМ" },            // МНК3, не комбинируется
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
    Warning,
    Critical,
    Info
};

enum JournalType
{
    Work,
    Meas,
    System
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
const QStringList sysJourDescription {
    "Рестарт модуля",                                                        //
    "Произошла запись и переход на новую конфигурацию",                      //
    "Произошла запись и переход на новую версию ВПО",                        //
    "Произошла запись в блок Bhb (hidden block)(базовая)",                   //
    "Произошла запись в блок Bhbm (hidden block)(мезонин)",                  //
    "Произошёл отказ внешней Flash-памяти 4 Мбайт на базовой плате",         //
    "Произошёл отказ внешней Flash-памяти 512 байт на мезонинной плате",     //
    "Ошибка работы внешней FRAM памяти",                                     //
    "Произошёл отказ при обновлении конфигурации",                           //
    "Ошибка загрузки конфигурации из flash памяти",                          //
    "Ошибка регулировочных коэффициентов (базовая)",                         //
    "Ошибка регулировочных коэффициентов (мезонин)",                         //
    "Ошибка перегрев модуля",                                                //
    "Напряжение батарейки низкое (< 2,5 В)",                                 //
    "Ошибка связи с NTP сервером",                                           //
    "Ошибка связи с 1PPS от антенны",                                        //
    "Ошибка АЦП (мезонин)",                                                  //
    "Ошибка АЦП (базовая)",                                                  //
    "Произошла запись регулировочных коэффициентов (базовая)",               //
    "Произошла запись регулировочных коэффициентов (мезонин)",               //
    "Произошло стирание системного журнала",                                 //
    "Произошло стирание рабочего журнала",                                   //
    "Произошло стирание осциллограмм",                                       //
    "Произошло стирание журнала измерений",                                  //
    "Ошибка ВПО",                                                            //
    "Ошибка встроенного АЦП 1",                                              //
    "Произошла запись в блок Bhb (hidden block)(базовая)",                   //
    "Произошла запись в блок Bhbm (hidden block)(мезонин)"                   //
    "Ошибка встроенного АЦП 2",                                              //
    "Ошибка RTC",                                                            //
    "Закрытие сокета 104"                                                    //
    "Произошла запись в блок Bhbd (hidden block)(доп)",                      //
    "Произошёл отказ внешней Flash-памяти 512 байт на дополнительной плате", //
    "Ошибка регулировочных коэффициентов (доп)"                              //
    "Ошибка АЦП (доп)",                                                      //
    "Произошла запись регулировочные коэффициенты (доп)",                    //
    "Произошла запись в блок Bhbd (hidden block)(доп)"                       //
};

const QStringList eventJourHeaders {
    " № ",              //
    "Дата/Время UTC",   //
    "Описание события", //
    "Тип события"       //
};

constexpr quint32 sysJourId = 1;
struct EventStruct
{
    EventStruct() = default;
    quint64 Time;
    quint8 EvType;
    quint8 EvNum[3];
    quint32 Reserv;
};

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
    "Проблемы со встроенным АЦП "                                                       //
};

}

namespace AVM_KTF
{

struct MeasureStruct
{
    quint32 NUM;
    quint32 Time;
    float Ueff[3];
    float Ieff[3];
    float Frequency;
    float U0;
    float U1;
    float U2;
    float I0;
    float I1;
    float I2;
    float Pf[4];
    float Qf[4];
    float Sf[4];
    float Cosphi;
    float Twin;
    float Tmk;
    float Tamb;
    quint32 res;
};

}

namespace AVM_KDV
{

struct MeasureStruct
{
    quint32 NUM;
    quint32 Time;
    float Ueff[3];
    float Ieff[3];
    float Frequency;
    float U1;
    float U2;
    float I1;
    float I2;
    float Pf_sum;
    float Qf_sum;
    float Sf_sum;
    float CosPhi;
    float Tmk;
    float Tamb;
    float Twin;
    float VibrSCZ_V[6];
    float VibrSCZ_D[6];
};

}

namespace AVM_KIV
{
struct MeasureStruct
{
    quint32 NUM;
    quint32 Time;
    float Ueff[3];
    float Ieff[3];
    float Frequency;
    float U0;
    float U1;
    float U2;
    float I0;
    float I1;
    float I2;
    float Cbush[3];
    float Tg_d[3];
    float dCbush[3];
    float dTg_d[3];
    float Iunb;
    float Phy_unb;
    float Tmk;
    float Tamb;
    quint32 res;
};

enum Registers : quint16
{
    MTypeB = 1,
    MTypeM = 2,
    Hwver = 3,
    Fwver = 4,
    Rst = 5,
    RstCount = 6,
    UID_Low = 7,
    UID_Mid = 8,
    UID_High = 9,
    SerialNumB = 10,
    SerialNumM = 11,
    HwverM = 12,
    SerialNum = 13,
    Cfcrc = 14,
    Hth = 15,
    Tmk = 101,
    Vbat = 102,
    Ccf = 801,
    Cfw = 802,
    Csmc = 803,
    Sbg = 900,
    Sbgtg = 905,
    WbgCA = 910,
    WbgCB = 911,
    WbgCC = 912,
    WbgTgA = 913,
    WbgTgB = 914,
    WbgTgC = 915,
    WcorTgA = 916,
    WcorTgB = 917,
    WcorTgC = 918,
    WInb = 919,
    WPhyunb = 920,
    UA = 1000,
    UB = 1001,
    UC = 1002,
    Ucp = 1003,
    UhA = 1004,
    UhB = 1005,
    UhC = 1006,
    Uhcp = 1007,
    KrF_U_A = 1008,
    KrF_U_B = 1009,
    KrF_U_C = 1010,
    U0 = 1011,
    U1 = 1012,
    U2 = 1013,
    KuU2 = 1014,
    KuU0 = 1015,
    IA = 1100,
    IB = 1101,
    IC = 1102,
    Icp = 1103,
    IhA = 1104,
    IhB = 1105,
    IhC = 1106,
    Ihcp = 1107,
    KrF_U_A1 = 1108,
    KrF_I_B = 1109,
    KrF_I_C = 1110,
    I0 = 1111,
    I1 = 1112,
    I2 = 1113,
    KuI2 = 1114,
    KuI0 = 1115,
    F = 2400,
    Phi_next0 = 2401,
    Phi_next1 = 2402,
    Phi_next2 = 2403,
    Phi_next3 = 2404,
    Phi_next4 = 2405,
    Phi_next5 = 2406,
    C_A = 2420,
    C_B = 2421,
    C_C = 2422,
    Tg_d_A = 2423,
    Tg_d_B = 2424,
    Tg_d_C = 2425,
    dC_A = 2426,
    dC_B = 2427,
    dC_C = 2428,
    dTg_d_A = 2429,
    dTg_d_B = 2430,
    dTg_d_C = 2431,
    Iunb = 2432,
    Phy_unb = 2433,
    BgCA = 4000,
    BgCB = 4001,
    BgCC = 4002,
    BgTgA = 4003,
    BgTgB = 4004,
    BgTgC = 4005,
    BgcorTgA = 4006,
    BgcorTgB = 4007,
    BgcorTgC = 4008,
    BgIunb = 4009,
    BgPhyunb = 4010,
    Tamb = 4501,
    Ramb = 4502,
    dC_pred = 7000,
    dC_alarm = 7001,
    dTg_pred = 7002,
    dTg_alarm = 7003,
    dIunb_pred = 7004,
    dIunb_alarm = 7005,
    AlarmDt = 201,
    SignVbat = 202,
    SignDpps = 203,
    NotUA = 3011,
    NotUB = 3012,
    NotUC = 3013,
    NotIA = 3014,
    NotIB = 3015,
    NotIC = 3016,
    BegDat = 3017,
    LowUA = 3018,
    LowUB = 3019,
    LowUC = 3020,
    SignTA = 3021,
    SignTB = 3022,
    SignTC = 3023,
    AlarmTA = 3024,
    AlarmTB = 3025,
    AlarmTC = 3026,
    SignCA = 3027,
    SignCB = 3028,
    SignCC = 3029,
    AlarmCA = 3030,
    AlarmCB = 3031,
    AlarmCC = 3032,
    PaspDat = 3033,
    SignInb = 3034,
    AlarmInb = 3035

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
