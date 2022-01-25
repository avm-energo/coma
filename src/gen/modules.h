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
    MTB_22 = 0x22,
    MTB_31 = 0x31,
    MTB_33 = 0x33,
    MTB_34 = 0x34,
    MTB_35 = 0x35,
    MTB_52 = 0x52,
    MTB_80 = 0x80,
    MTB_81 = 0x81,
    MTB_84 = 0x84,
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
    MTM_85 = 0x85
};

const QMap<quint16, QString> BaseBoards {
    { MTB_00, "АВ_ТУК-00" }, // нет базовой платы
    { MTB_14, "АВ_ТУК-14" }, // alise (ЦКУ sonica)
    //        {MTB_12,{"АВ-ТУК-12 (ЦКУ)", 0x1200}}, // процессорный, не комбинируется
    { MTB_21, "АВ-ТУК-21" }, // аналоговый ввод DC, комбинируется с 2x,3x
    // { MTB_22, "АВ-ТУК-22" },     // аналоговый ввод DC, комбинируется с 2x,3x
    { MTB_31, "АВ-ТУК-31" },     // дискретный ввод, комбинируется с 2x,3x
    { MTB_33, "АВ-ТУК-33" },     // дискретный ввод, комбинируется с 2x,3x
    { MTB_34, "АВ-ТУК-34" },     // дискретный ввод, комбинируется с 2x,3x
    { MTB_35, "АВ-ТУК-35" },     // дискретный вывод, комбинируется с 2x,3x
    { MTB_80, "АВ-ТУК-80" },     // аналоговый ввод AC, комбинируется с 81,82,83
    { MTB_81, "АВ-ТУК-81" },     // МНК3, комбинируется с 84
    { MTB_84, "АВ-ТУК-84" },     // НКВВ, комбинируется с 81,82,83
    { MTB_85, "АВ-ТУК-85" },     // УСК, комбинируется с 85
    { MTB_86, "АВ-ТУК-86" },     // комбинируется с 86 и ?
    { MTB_87, "АВ-ТУК-87 (ЧР)" } // ЧР, не комбинируется
    //        {MTB_A1,{"ПКС-1", 0xA100}}, // ПКДН, прибор для Туркота, не комбинируется
    //        {MTB_A2,{"АВМ", 0xA200}}, // МНК3, не комбинируется
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
    All,
    Base,
    Mezz
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
    bool isOutdated(quint32 configVersion) const
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

enum BciNumber : unsigned
{
    dummyElement = 0,
    MTypeB_ID = 1,
    MTypeE_ID = 2,
    CType = 3,
    Abs_104 = 4,
    Cycle_104 = 5,
    T1_104 = 6,
    T2_104 = 7,
    T3_104 = 8,
    k_104 = 9,
    w_104 = 10,
    mTimezone = 11,
    IP_ID = 20,
    Mask_ID = 21,
    GW_ID = 22,
    Port_ID = 23,
    SNTP_ID = 24,
    Baud_ID = 25,
    Parity_ID = 26,
    stopbit_ID = 27,
    adrMB_ID = 28,
    IsNTP_ID = 29,
    IsPPS_ID = 30,
    in_typeA_IDB = 101,
    in_min_IDB = 102,
    in_max_IDB = 103,
    in_vmin_IDB = 104,
    in_vmax_IDB = 105,
    setminminA_IDB = 106,
    setminA_IDB = 107,
    setmaxA_IDB = 108,
    setmaxmaxA_IDB = 109,
    //   osc_IDB = 110,
    oscdly_IDB = 111,
    oscthr_IDB = 112,
    filterthr_IDB = 113,
    filtern_IDB = 114,
    hysteresisA_IDB = 115,
    Osc104_IDB = 116,
    OscI2C_IDB = 117,
    OscAlg_IDB = 118,
    in_typeA_IDE = 121,
    in_min_IDE = 122,
    in_max_IDE = 123,
    in_vmin_IDE = 124,
    in_vmax_IDE = 125,
    setminminA_IDE = 126,
    setminA_IDE = 127,
    setmaxA_IDE = 128,
    setmaxmaxA_IDE = 129,
    //  osc_IDE = 130,
    oscdly_IDE = 131,
    oscthr_IDE = 132,
    filterthr_IDE = 133,
    filtern_IDE = 134,
    hysteresisA_IDE = 135,
    Osc104_IDE = 136,
    OscI2C_IDE = 137,
    OscAlg_IDE = 138,
    in_typeD_IDB = 201,
    dly_time_IDB = 202,
    // Pair_IDB = 203,
    AC_per_IDB = 204,
    Invert_IDB = 205,
    typeU_IDB = 206,
    in_typeD32_IDB = 211,
    dly_time32_IDB = 212,
    in_typeD_IDE = 231,
    dly_time_IDE = 232,
    // Pair_IDE = 233,
    AC_per_IDE = 234,
    Invert_IDE = 235,
    typeU_IDE = 236,
    in_typeD32_IDE = 241,
    dly_time32_IDE = 242,
    in_typeT_IDB = 301,
    in_minT_IDB = 302,
    in_maxT_IDB = 303,
    setminminT_IDB = 304,
    setminT_IDB = 305,
    setmaxT_IDB = 306,
    setmaxmaxT_IDB = 307,
    kf_filtrT_IDB = 308,
    cnt_filtrT_IDB = 309,
    hysteresisT_IDB = 310,
    RzeroT_IDB = 311,
    in_typeT_IDE = 321,
    in_minT_IDE = 322,
    in_maxT_IDE = 323,
    setminminT_IDE = 324,
    setminT_IDE = 325,
    setmaxT_IDE = 326,
    setmaxmaxT_IDE = 327,
    kf_filtrT_IDE = 328,
    cnt_filtrT_IDE = 329,
    hysteresisT_IDE = 330,
    RzeroT_IDE = 331,
    wd_type_IDB = 401,
    wd_nout_IDB = 402,
    wd_tm_IDB = 403,
    pulse_short_IDB = 404,
    pulse_long_IDB = 405,
    //  out_type_IDB = 406,
    ispulse_IDB = 407,
    type_pulse_IDB = 408,
    wd_type_IDE = 421,
    wd_nout_IDE = 422,
    wd_tm_IDE = 423,
    pulse_short_IDE = 424,
    pulse_long_IDE = 425,
    // out_type_IDE = 426,
    ispulse_IDE = 427,
    type_pulse_IDE = 428,
    Eq_type_ID = 1001,
    NPOINTS_ID = 1002,
    NFiltr_ID = 1003,
    NHarmFilt_ID = 1004,
    DDOsc_ID = 1005,
    Unom1 = 1006,
    Unom2 = 1007,
    I1nom = 1008,
    I2nom = 1009,
    DUosc = 1010,
    DIosc_ID = 1011,
    DUImin_ID = 1012,
    Imin = 1013,
    Imax = 1014,
    C_init = 1015,
    Tg_init = 1016,
    corTg = 1017,
    dC_pred = 1018,
    dC_alarm = 1019,
    dTg_pred = 1020,
    dTg_alarm = 1021,
    dIunb_pred = 1022,
    dIunb_alarm = 1023,
    GdC = 1024,
    GdTg = 1025,
    GdIunb = 1026,
    RTerm = 1027,
    W100 = 1028,
    Iunb_init = 1029,
    Phy_unb_init = 1030,
    T_Data_Rec = 1031,
    LowU = 1032,
    IsU = 1033,
    IsIuIunb = 1034,
    Tevent_pred = 1035,
    Tevent_alarm = 1036,
    Trele_pred = 1037,
    Trele_alarm = 1038,
    Tg_Pasp_ID = 1039,
    C_Pasp_ID = 1040,
    U2nom = 1050,
    U2nom_2 = 1051,
    ITT1nom_KTF_KDV = 1052,
    ITT2nom_KTF_KDV = 1053,
    TTconf1 = 1054,
    TTconf2 = 1055,
    MBMaster = 1060,
    MBMab1 = 1061,
    MBMab2 = 1062,
    MBMab3 = 1063,
    MBMab4 = 1064,
    Cool_type = 1070,
    W_mat = 1071,
    TNNTdop = 1072,
    TNNTpred = 1073,
    Tamb_nom = 1074,
    dTNNTnom = 1075,
    Kdob = 1076,
    TauWnom = 1077,
    Umaxm = 1078,
    Imaxm = 1079,
    Iwnom = 1080,
    OscPoints = 1081,
    GTnnt = 1082,
    GOvc = 1083,
    TdatNum = 1084,
    Eq_type = 1101,
    Core_type = 1102,
    D_win = 1103,
    N_type = 1104,
    Op_type = 1105,
    Is_U2 = 1106,
    Residue = 1107,
    // RESnom_max = 1111,
    RESkz_max = 1112,
    Unom = 1113,
    Inom = 1114,
    Ikz = 1115,
    ITT1nom_8085 = 1116,
    ITT2nom_8085 = 1117,
    T_arc = 1118,
    RDDS = 1119,
    RRDS = 1120,
    Ts_OFF = 1121,
    Ts_ON = 1122,
    Tzad_OFF = 1123,
    Tzad_ON = 1124,
    Tbk_OFF_ON = 1125,
    Tbk_ON_ON = 1126,
    Duosc_8085 = 1127,
    Diosc_8085 = 1128,
    Dumin_8085 = 1129,
    Dimin_8085 = 1130,
    Ts_OFFpred = 1131,
    Ts_OFFalarm = 1132,
    Ts_Onpred = 1133,
    Ts_Onalarm = 1134,
    Tmain_OFFpred = 1135,
    Tmain_OFFalarm = 1136,
    Tmain_Onpred = 1137,
    Tmain_ONalarm = 1138,
    dT_OFFpred = 1139,
    dT_OFFalarm = 1140,
    dT_Onpred = 1141,
    dT_Onalarm = 1142,
    Tras_pred = 1143,
    Tras_alarm = 1144,
    Tarc_pred = 1145,
    Tarc_alarm = 1146,
    RESnom_pred = 1147,
    RESnom_alarm = 1148,
    RESkz_pred = 1149,
    RESkz_alarm = 1150,
    I_pred = 1151,
    I_alarm = 1152,
    Aux = 1153,
    Tbk_OFF_OFF = 1154,
    Tbk_ON_OFF = 1155,
    Adapt = 1162,
    TsolON = 1163,
    TsolOFF = 1164,
    NumA_8085 = 1165,
    TypeA = 1166,
    //   RESnom_max = 1168,
    Pdrive_nom = 1169,
    Pdrive_pred = 1170,
    Pdrive_alarm = 1171,
    dTs_OFF = 1172,
    dTs_ON = 1173,
    RES_const_A_big = 1176,
    RES_const_a_small = 1177,
    RES_const_n_small = 1178,
    RES_const_b = 1179,
    RES_const_N_big = 1180,
    Algorithm_Choice = 1181,
    Iotc = 1182,
    Tdis_OFF = 1183,
    Tdis_ON = 1184,
    K_on_volt = 1190,
    K_off_volt = 1191,
    K_on_tamb = 1192,
    K_off_tamb = 1193,
    K_on_tdr = 1194,
    K_off_tdr = 1195,
    K_on_hdr = 1196,
    K_off_hdr = 1197,
    K_on_tbo = 1198,
    K_off_tbo = 1199,
    Kad = 1200,
    Fnom = 1201,
    nom_slip = 1202,
    UVmax = 1203,
    Poles = 1204,
    Stator_Slotes = 1205,
    Rotor_bars = 1206,
    VibroType = 1207,
    Sensors = 1208,
    Kvibr = 1209,
    VibrA_pred = 1210,
    VibrV_pred = 1211,
    VibrD_pred = 1212,
    VibrA_alarm = 1213,
    VibrV_alarm = 1214,
    VibrD_alarm = 1215,
    VVibrA_pred = 1216,
    VVibrV_pred = 1217,
    VVibrD_pred = 1218,
    VVibrA_alarm = 1219,
    VVibrV_alarm = 1220,
    VVibrD_alarm = 1221,
    NumA_KDV = 1222,
    Istart_pred = 1223,
    Istart_alarm = 1224,
    I2_pred = 1225,
    I2_alarm = 1226,
    Ih_pred = 1227,
    Ih_alarm = 1228,

};

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
const QStringList Health

    {
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
/// AVTUK Central Controller Unit
namespace AVTUK_CCU
{
constexpr uint16_t MainBlock = 1;
constexpr uint16_t IndicationBlock = 2;
struct Main
{
    uint8_t PWRIN;
    bool resetReq;
};
struct Indication
{
    uint8_t PulseCnt1;
    uint16_t PulseFreq1;
    uint8_t PulseCnt2;
    uint16_t PulseFreq2;
};

}

// Oscillogram / SwitchJournal Part

/// ID заголовка осциллограммы
constexpr int MT_HEAD_ID = 9000;

constexpr int ID_OSC_CH0 = 10040; // ID осциллограммы канала 0
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
constexpr int ID_SPECTR_CH7 = 10057; // ID спектрограммы канала 7

namespace AVTUK_85
{
/// ID журнала переключений с осциллограммой модуля 8085/8585
constexpr int SWJ_ID = 10031;
/// ID осциллограммы по модулю 8085/8585
constexpr int OSC_ID = 10030;
}

namespace AVTUK_8X
{
/// ID осциллограммы по модулю 808x
constexpr int OSC_ID = 10020;
}

namespace AVTUK_87
{
/// ID осциллограммы и спектрограммы модуля 87
constexpr int OSC_SPC_ID = 9050;
}

namespace AVTUK_21
{
/// первый ID осциллограммы по модулю 21
constexpr int OSC_ID_MIN = 10001;
/// последний ID осциллограммы по модулю 21
constexpr int OSC_ID_MAX = 10016;
}

/// максимальный размер буфера для осциллограмм
constexpr int MAXOSCBUFSIZE = 262144;
