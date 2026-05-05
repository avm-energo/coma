#include "modulebsi.h"

#include "../config/config.h"
#include "../widgets/emessagebox.h"
#include "commands.h"
#include "error.h"
#include <avm-gen/stdfunc.h>

quint32 ModuleBSI::MType = 0;
ModuleBSI::Bsi ModuleBSI::ModuleBsi;
QString ModuleBSI::ModuleTypeString = "";

ModuleBSI::ModuleBSI()
{
    ModuleBsi.MTypeB = ModuleBsi.MTypeM = 0xFFFFFFFF;
}

int ModuleBSI::SetupBSI()
{
    if (Commands::GetBsi(ModuleBsi) != Error::ER_NOERROR)
        return Error::ER_CANAL;
    ModuleTypeString = Config::ModuleBaseBoards()[ModuleBsi.MTypeB << 8].TextString
        + Config::ModuleMezzanineBoards()[ModuleBsi.MTypeM].TextString;
    ModuleBSI::Bsi bsi = ModuleBsi;
    QString tmps = ModuleTypeString;
#if PROGSIZE >= PROGSIZE_LARGE
    if (!IsKnownModule())
        return Error::ER_RESEMPTY;
#endif
    return Error::ER_NOERROR;
}

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

bool ModuleBSI::IsKnownModule()
{
    // проверка серийных номеров
    if ((ModuleBsi.SerialNumB == 0xFFFFFFFF)
        || ((ModuleBsi.SerialNumM == 0xFFFFFFFF) && (ModuleBsi.MTypeM != Config::MTM_00))
        || (ModuleBsi.SerialNum == 0xFFFFFFFF)) // серийный номер не задан, выдадим предупреждение
        return false;
    // проверка известных типов плат
    if (Config::ModuleBaseBoards().keys().contains(ModuleBsi.MTypeB << 8))
    {
        if (ModuleBsi.MTypeM != Config::MTM_00)
        {
            if (Config::ModuleMezzanineBoards().keys().contains(ModuleBsi.MTypeM))
                return true;
            else
                return false;
        }
        return true;
    }
    return false;
}

int ModuleBSI::PrereadConf(QWidget *w, QList<S2::DataRec> S2Config)
{
    int res;

    if ((ModuleBSI::Health() & HTH_CONFIG)) // если в модуле нет конфигурации, заполнить поля по умолчанию
        return Error::ER_RESEMPTY;
    else                                    // иначе заполнить значениями из модуля
    {
        if ((res = Commands::GetFile(1, S2Config)) != Error::ER_NOERROR)
        {
            QString tmps = "прибора ";
            EMessageBox::error(w, "ошибка", "Ошибка чтения конфигурации из " + tmps + QString::number(res));
            return Error::ER_GENERALERROR;
        }
    }
    return Error::ER_NOERROR;
}
