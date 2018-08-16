#ifndef MODULEBSI_H
#define MODULEBSI_H

#include <QtGlobal>
#include "maindef.h"

#define MAXERRORFLAGNUM 32

#define HTH_FNC             0x00000200 // частота не в допуске (Э)
#define HTH_LS              0x00000100 // сигналы малы (Э)
#define HTH_CONFIG          0x00000080 // в модуле нет конфигурации
#define HTH_REGPARS         0x00000040 // в модуле нет настроечных параметров
#define HTH_ADC             0x00000020 // проблема с АЦП (Э)
#define HTH_1PPS            0x00000010 // нет сигнала 1PPS
#define HTH_REL             0x00000008 // неисправность выходных реле (Д)
#define HTH_TUPP            0x00000004 // перегрев модуля

enum BoardTypes
{
    BT_NONE = 0,
    BT_BASE = 1,
    BT_MEZONIN = 2,
    BT_BSMZ = 3,
    BT_MODULE = 4
};

class ModuleBSI
{
public:
    typedef struct
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
    } Bsi;

    ModuleBSI();

#if PROGSIZE != PROGSIZE_EMUL
    static int SetupBSI();
#endif
    static void SetupEmulatedBsi(Bsi &bsi);
    static QString GetModuleTypeString();
    static quint32 GetMType(BoardTypes type);
    static quint32 Health();
    static quint32 SerialNum(BoardTypes type); // возвращает серийный номер, по-умолчанию номер модуля
    static Bsi GetBsi();
    static quint32 GetHealth();

private:
    static quint32 MType;
    static Bsi ModuleBsi;
    static QString ModuleTypeString;
};

#endif // MODULEBSI_H