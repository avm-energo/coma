#pragma once
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
