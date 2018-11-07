#include "../config/config.h"
#include "modulebsi.h"
#include "error.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "commands.h"
#endif

quint32 ModuleBSI::MType = 0;
ModuleBSI::Bsi ModuleBSI::ModuleBsi;
QString ModuleBSI::ModuleTypeString = "";

ModuleBSI::ModuleBSI()
{
    ModuleBsi.MTypeB = ModuleBsi.MTypeM = 0xFFFFFFFF;
}

#if PROGSIZE != PROGSIZE_EMUL
int ModuleBSI::SetupBSI()
{
    if (Commands::GetBsi(ModuleBsi) != Error::ER_NOERROR)
        return Error::ER_CANAL;
    quint32 mtype;
    QString mtypestring;
    mtype = ((ModuleBsi.MTypeB & 0x000000FF) << 8) | (ModuleBsi.MTypeM & 0x000000FF);
#ifndef MODULE_A1
    mtypestring = "АВТУК-";
    mtypestring.append(QString::number(mtype, 16));
#else
    mtypestring = "ПКС-1";
#endif
    ModuleTypeString = mtypestring;
#if PROGSIZE >= PROGSIZE_LARGE
    if ((ModuleBsi.SerialNumB == 0xFFFFFFFF) || ((ModuleBsi.SerialNumM == 0xFFFFFFFF) && (ModuleBsi.MTypeM != MTM_00)) || \
            (ModuleBsi.SerialNum == 0xFFFFFFFF)) // серийный номер не задан, выдадим предупреждение
        return Error::ER_RESEMPTY;
#endif
    return Error::ER_NOERROR;
}
#endif
void ModuleBSI::SetupEmulatedBsi(ModuleBSI::Bsi &bsi)
{
    ModuleBsi = bsi;
}

QString ModuleBSI::GetModuleTypeString()
{
    return ModuleTypeString;
}

quint32 ModuleBSI::GetMType(BoardTypes type)
{
    return (type == BT_MEZONIN) ? ModuleBsi.MTypeM : ModuleBsi.MTypeB;
}

quint32 ModuleBSI::Health()
{
    return ModuleBsi.Hth;
}

quint32 ModuleBSI::SerialNum(BoardTypes type)
{
    switch (type)
    {
    case BT_BASE:
        return ModuleBsi.SerialNumB;
    case BT_MEZONIN:
        return ModuleBsi.SerialNumM;
    default:
        return ModuleBsi.SerialNum;
    }
}

ModuleBSI::Bsi ModuleBSI::GetBsi()
{
    Bsi bsi = ModuleBsi;
    return bsi;
}

quint32 ModuleBSI::GetHealth()
{
    return ModuleBsi.Hth;
}

