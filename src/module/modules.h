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
enum BaseBoards
{
    MTB_00 = 0x0000,
    MTB_12 = 0x1200,
    MTB_21 = 0x2100,
    MTB_22 = 0x2200,
    MTB_31 = 0x3100,
    MTB_35 = 0x3500,
    MTB_80 = 0x8000,
    MTB_81 = 0x8100,
    MTB_84 = 0x8400,
    MTB_85 = 0x8500,
    MTB_87 = 0x8700,
    MTB_A1 = 0xA100,
    MTB_A2 = 0xA200
};

enum MezzanineBoards
{
    MTM_00 = 0x0000,
    MTM_21 = 0x0021,
    MTM_22 = 0x0022,
    MTM_31 = 0x0031,
    MTM_35 = 0x0035,
    MTM_81 = 0x0081,
    MTM_82 = 0x0082,
    MTM_83 = 0x0083,
    MTM_84 = 0x0084,
    MTM_85 = 0x0085
};

static const QMap<quint16, QString> BaseBoards {
    { MTB_00, "АВ_ТУК-00" }, // нет базовой платы
    //        {MTB_12,{"АВ-ТУК-12 (ЦКУ)", 0x1200}}, // процессорный, не комбинируется
    { MTB_21, "АВ-ТУК-21" },     // аналоговый ввод DC, комбинируется с 2x,3x
    { MTB_22, "АВ-ТУК-22" },     // аналоговый ввод DC, комбинируется с 2x,3x
    { MTB_31, "АВ-ТУК-31" },     // дискретный ввод, комбинируется с 2x,3x
    { MTB_35, "АВ-ТУК-35" },     // дискретный вывод, комбинируется с 2x,3x
    { MTB_80, "АВТУК-" },        // аналоговый ввод AC, комбинируется с 81,82,83
    { MTB_81, "АВ-ТУК-81" },     // МНК3, комбинируется с 84
    { MTB_84, "АВ-ТУК-84" },     // НКВВ, комбинируется с 81,82,83
    { MTB_85, "АВ-ТУК-85" },     // УСК, комбинируется с 85
    { MTB_87, "АВ-ТУК-87 (ЧР)" } // ЧР, не комбинируется
    //        {MTB_A1,{"ПКС-1", 0xA100}}, // ПКДН, прибор для Туркота, не комбинируется
    //        {MTB_A2,{"АВМ", 0xA200}}, // МНК3, не комбинируется
};

static const QMap<quint16, QString> MezzanineBoards {
    { MTM_00, "" },           // нет мезонина
    { MTM_21, "21" },         // аналоговый ввод, комбинируется с 2x,3x
    { MTM_22, "22" },         // аналоговый ввод, комбинируется с 2x,3x
    { MTM_31, "31" },         // дискретный ввод, комбинируется с 2x,3x
    { MTM_35, "35" },         // дискретный вывод, комбинируется с 2x,3x
    { MTM_81, "81 (Э2Т0Н)" }, // 0U6I, комбинируется с 80,84
    { MTM_82, "82 (Э1Т1Н)" }, // 3U3I, комбинируется с 80,84
    { MTM_83, "83 (Э0Т2Н)" }, // 6U0I, комбинируется с 80,84
    { MTM_84, "84" },         // 3U3I, комбинируется с 80,81
    { MTM_85, "85 (УСК)" }    // 3U3I УСК (перегрузка по току 20х), комбинируется с 85
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
};

struct StartupInfoBlockExt
{
};

}
#define MAXERRORFLAGNUM 32

#define HTH_FNC 0x00000200     // частота не в допуске (Э)
#define HTH_LS 0x00000100      // сигналы малы (Э)
#define HTH_CONFIG 0x00000080  // в модуле нет конфигурации
#define HTH_REGPARS 0x00000040 // в модуле нет настроечных параметров
#define HTH_ADC 0x00000020     // проблема с АЦП (Э)
#define HTH_1PPS 0x00000010    // нет сигнала 1PPS
#define HTH_REL 0x00000008     // неисправность выходных реле (Д)
#define HTH_TUPP 0x00000004    // перегрев модуля

/// Bci
namespace Bci
{
Q_NAMESPACE
// enum class BciMainEnum : byte
//{
//    MTypeB = 1,
//    MTypeM = 2,
//    Ctype = 3,
//    Abs_104 = 4,
//    Cycle_104 = 5,
//    T1_104 = 6,
//    T2_104 = 7,
//    T3_104 = 8,
//    K_104 = 9,
//    W_104 = 10
//};
// Q_ENUM_NS(Bci::BciMainEnum);

// enum class BciDefMainValues : byte
//{
//    MTypeB = 0,
//    MTypeM = 0,
//    Ctype = 1,
//    Abs_104 = 205,
//    Cycle_104 = 5,
//    T1_104 = 15,
//    T2_104 = 10,
//    T3_104 = 20,
//    K_104 = 12,
//    W_104 = 8
//};
// Q_ENUM_NS(Bci::BciDefMainValues);
constexpr int DEF_CTYPE = 1;
constexpr int DEF_ABS_104 = 205;
constexpr int DEF_CYCLE_104 = 5;
constexpr int DEF_T1_104 = 15;
constexpr int DEF_T2_104 = 10;
constexpr int DEF_T3_104 = 20;
constexpr int DEF_K_104 = 12;
constexpr int DEF_W_104 = 8;
struct BciMain
{
    BciMain()
    {
        MTypeB = Modules::MTB_00;
        MTypeM = Modules::MTM_00;
        Ctype = DEF_CTYPE;
        Abs_104 = DEF_ABS_104;
        Cycle_104 = DEF_CYCLE_104;
        T1_104 = DEF_T1_104;
        T2_104 = DEF_T2_104;
        T3_104 = DEF_T3_104;
        K_104 = DEF_K_104;
        W_104 = DEF_W_104;
    }
    quint32 MTypeB;
    quint32 MTypeM;
    // Тип синхронизации времени от модуля Ц
    quint32 Ctype;
    // Адрес базовой станции для протокола 104
    quint32 Abs_104;
    // Интервал циклического опроса по протоколу МЭК 60870-5-104
    quint32 Cycle_104;
    // тайм-аут Т1 для протокола 104
    quint32 T1_104;
    // тайм-аут Т2 для протокола 104
    quint32 T2_104;
    // тайм-аут Т3 для протокола 104
    quint32 T3_104;
    // макс. кол-во неподтв. сообщений
    quint32 K_104;
    // макс. кол-во сообщений, после которых необх. выдать подтверждение
    quint32 W_104;
};
}

enum BciNumber : unsigned
{
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
    osc_IDB = 110,
    oscdly_IDB = 111,
    oscthr_IDB = 112,
    filterthr_IDB = 113,
    filtern_IDB = 114,
    hysteresisA_IDB = 115,
    in_typeA_IDE = 121,
    in_min_IDE = 122,
    in_max_IDE = 123,
    in_vmin_IDE = 124,
    in_vmax_IDE = 125,
    setminminA_IDE = 126,
    setminA_IDE = 127,
    setmaxA_IDE = 128,
    setmaxmaxA_IDE = 129,
    osc_IDE = 130,
    oscdly_IDE = 131,
    oscthr_IDE = 132,
    filterthr_IDE = 133,
    filtern_IDE = 134,
    hysteresisA_IDE = 135,
    in_typeD_IDB = 201,
    dly_time_IDB = 202,
    Pair_IDB = 203,
    Signal_type_IDB = 204,
    in_typeD2_IDB = 205,
    dly_time2_IDB = 206,
    in_typeD_IDE = 231,
    dly_time_IDE = 232,
    Pair_IDE = 233,
    Signal_type_IDE = 234,
    in_typeD2_IDE = 235,
    dly_time2_IDE = 236,
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
    out_type_IDB = 406,
    out_type32_IDB = 407,
    wd_type_IDE = 421,
    wd_nout_IDE = 422,
    wd_tm_IDE = 423,
    pulse_short_IDE = 424,
    pulse_long_IDE = 425,
    out_type_IDE = 426,
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
    ITT1nom_KTF = 1052,
    ITT2nom_KTF = 1053,
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
    Duosc = 1127,
    Diosc = 1128,
    Dumin = 1129,
    Dimin = 1130,
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
/// Bci

namespace AVM
{
const QStringList sysJourDescription {
    "Рестарт модуля",                                                    //
    "Произошла запись и переход на новую конфигурацию",                  //
    "Произошла запись и переход на новую версию ВПО",                    //
    "Произошла запись в блок Bhb (hidden block)",                        //
    "Произошла запись в блок Bhbm (hidden block)( Мезонин)",             //
    "Произошёл отказ внешней Flash-памяти 4 Мбайт на базовой плате",     //
    "Произошёл отказ внешней Flash-памяти 512 байт на мезонинной плате", //
    "Ошибка работы внешней FRAM памяти",                                 //
    "Произошёл отказ при обновлении конфигурации",                       //
    "Ошибка загрузки конфигурации из flash памяти",                      //
    "Ошибка регулировочных коэффициентов (базовая)",                     //
    "Ошибка регулировочных коэффициентов (Мезонин)",                     //
    "Ошибка перегрев модуля",                                            //
    "Напряжение батарейки низкое (< 2,5 В)",                             //
    "Ошибка связи с NTP сервером",                                       //
    "Ошибка связи с 1PPS от антенны",                                    //
    "Ошибка АЦП (Мезонин)",                                              //
    "Ошибка АЦП (базовая)",                                              //
    "Произошла запись регулировочных коэффициентов (базовая)",           //
    "Произошла запись регулировочных коэффициентов (Мезонин)",           //
    "Произошло стирание системного журнала",                             //
    "Произошло стирание рабочего журнала",                               //
    "Произошло стирание осциллограмм",                                   //
    "Произошло стирание журнала измерений",                              //
    "Ошибка ВПО", "Ошибка встроенного АЦП",                              //
    "Произошла запись в блок Bhb (hidden block)",                        //
    "Произошла запись в блок Bhbm (hidden block)( Мезонин)"              //
};
const QStringList eventJourHeaders {
    " № ",              //
    "Дата/Время UTC",   //
    "Описание события", //
    "Тип события"       //
};
constexpr quint32 sysJourId = 0;
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
const QStringList criticalEvents {
    "Аварийное сообщение по недопустимому превышению температуры обмотки", //
    "Аварийное сообщение по недопустимому уровню пускового тока         "  //
};
const quint32 criticalCount = criticalEvents.size();
constexpr quint32 criticalFlags = 0x00000005;
const QStringList warningEvents {
    "Отсутствует сигнал напряжения фазы A                   ", //
    "Отсутствует сигнал напряжения фазы B                   ", //
    "Отсутствует сигнал напряжения фазы С                   ", //
    "Отсутствует сигнал тока  фазы А                        ", //
    "Отсутствует сигнал тока  фазы B                        ", //
    "Отсутствует сигнал тока  фазы C                        ", //
    "Перегрузка по току фазы A                              ", //
    "Перегрузка по току фазы В                              ", //
    "Перегрузка по току фазы С                              ", //
    //"Машина включена ",//
    "Сигнализация по опасному превышению температуры обмотки", //
    "Неисправны все датчики температуры обмотки             ", //
    "Сигнализация по опасному уровню пускового тока         "  //
};
constexpr quint32 warningFlags = 0xfffa;
constexpr quint32 workJourId = 5011;
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

const QStringList workJourDescription {
    "Отсутствует сигнал напряжения фазы A",                                //
    "Отсутствует сигнал напряжения фазы B",                                //
    "Отсутствует сигнал напряжения фазы С",                                //
    "Отсутствует сигнал тока фазы A",                                      //
    "Отсутствует сигнал тока фазы B",                                      //
    "Отсутствует сигнал тока фазы С",                                      //
    "Перегрузка по току фазы A",                                           //
    "Перегрузка по току фазы B",                                           //
    "Перегрузка по току фазы C",                                           //
    "Машина включена",                                                     //
    "Сигнализация по опасному превышению температуры обмотки",             //
    "Неисправны все датчики температуры об-мотки",                         //
    "Аварийное сообщение по недопустимому превышению температуры обмотки", //
    "Сигнализация по опасному уровню пускового тока",                      //
    "Аварийное сообщение по недопустимому уровню пускового тока"           //
};
const QStringList measJourHeaders {
    " № ",            //
    "Дата/Время UTC", //
    "Ueff фA",        //
    "Ueff фB",        //
    "Ueff фC",        //
    "Ieff фA",        //
    "Ieff фB",        //
    "Ieff фC",        //
    "Frequency",      //
    "U0",             //
    "U1",             //
    "U2",             //
    "I0",             //
    "I1",             //
    "I2",             //
    "Pf фA",          //
    "Pf фB",          //
    "Pf фC",          //
    "Pf сумм.",       //
    "Qf фA",          //
    "Qf фB",          //
    "Qf фC",          //
    "Qf сумм.",       //
    "Sf фA",          //
    "Sf фB",          //
    "Sf фC",          //
    "Sf сумм.",       //
    "CosPhi",         //
    "Tmk,°С",         //
    "Tamb,°С",        //
    "Twin,°С"         //
};
}

namespace AVM_KDV
{
const QStringList criticalEvents {
    "Аварийное сообщение по недопустимому превышению температуры обмотки", //
    "Аварийное сообщение по недопустимому уровню пускового тока         "  //
};
const quint32 criticalCount = criticalEvents.size();
constexpr quint32 criticalFlags = 0x00000005;
const QStringList warningEvents {
    "Отсутствует сигнал напряжения фазы A                   ", //
    "Отсутствует сигнал напряжения фазы B                   ", //
    "Отсутствует сигнал напряжения фазы С                   ", //
    "Отсутствует ток ввода фазы А (ток меньше 2мА)          ", //
    "Отсутствует ток ввода фазы B (ток меньше 2мА)          ", //
    "Отсутствует ток ввода фазы C (ток меньше 2мА)          ", //
    "Не заданы начальные значения                           ", //
    "Низкое напряжение фазы A                               ", //
    "Низкое напряжение фазы B                               ", //
    "Низкое напряжение фазы C                               ", //
    "Сигнализация по приращению тангенса дельта ввода фазы А", //
    "Сигнализация по приращению тангенса дельта ввода фазы B", //
    "Сигнализация по приращению тангенса дельта ввода фазы C", //
    "Сигнализация по приращению C ввода фазы А              ", //
    "Сигнализация по приращению C ввода фазы B              ", //
    "Сигнализация по приращению C ввода фазы C              ", //
    "Не заданы паспортные значения                          ", //
    "Сигнализация по повышенному небалансу токов            "  //
};
constexpr quint32 warningFlags = 0x1fff1ec6;
constexpr quint32 workJourId = 3010;
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

const QStringList workJourDescription {
    "Отсутствует сигнал напряжения фазы A",                    //
    "Отсутствует сигнал напряжения фазы B",                    //
    "Отсутствует сигнал напряжения фазы С",                    //
    "Нет реактивного тока канала А (ток меньше 2мА)",          //
    "Нет реактивного тока канала B (ток меньше 2мА)",          //
    "Нет реактивного тока канала C (ток меньше 2мА)",          //
    "Не заданы начальные значения",                            //
    "Низкое напряжение фазы A",                                //
    "Низкое напряжение фазы B",                                //
    "Низкое напряжение фазы C",                                //
    "Сигнализация по приращению тангенса дельта ввода фазы А", //
    "Сигнализация по приращению тангенса дельта ввода фазы B", //
    "Сигнализация по приращению тангенса дельта ввода фазы C", //
    "Авария по приращению тангенса дельта ввода фазы А",       //
    "Авария по приращению тангенса дельта ввода фазы B",       //
    "Авария по приращению тангенса дельта ввода фазы C",       //
    "Сигнализация по приращению C ввода фазы А",               //
    "Сигнализация по приращению C ввода фазы B",               //
    "Сигнализация по приращению C ввода фазы C",               //
    "Авария по приращению C ввода фазы А",                     //
    "Авария по приращению C ввода фазы B",                     //
    "Авария по приращению C ввода фазы C",                     //
    "Не заданы паспортные значения",                           //
    "Сигнализация по изменению небаланса тока",                //
    "Авария по изменению небаланса тока"                       //
};
const QStringList measJourHeaders {
    " № ",            //
    "Дата/Время UTC", //
    "Ueff фA",        //
    "Ueff фB",        //
    "Ueff фC",        //
    "Ieff фA",        //
    "Ieff фB",        //
    "Ieff фC",        //
    "Freq",           //
    "U1",             //
    "U2",             //
    "I1",             //
    "I2",             //
    "Pf_sum",         //
    "Qf_sum",         //
    "Sf_sum",         //
    "CosPhi",         //
    "Tmk",            //
    "Tamb",           //
    "Twin",           //
    "VibrSCZ_V",      //
    "VibrSCZ_V",      //
    "VibrSCZ_V",      //
    "VibrSCZ_V",      //
    "VibrSCZ_V",      //
    "VibrSCZ_V",      //
    "VibrSCZ_D",      //
    "VibrSCZ_D",      //
    "VibrSCZ_D",      //
    "VibrSCZ_D",      //
    "VibrSCZ_D",      //
    "VibrSCZ_D"       //
};
}
