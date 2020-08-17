#include "modulebsi.h"

#include "../config/config.h"
#include "../usb/commands.h"
#include "../widgets/emessagebox.h"
#include "error.h"
#include "stdfunc.h"

quint32 ModuleBSI::MType = 0;
ModuleBSI::Bsi ModuleBSI::ModuleBsi;
QString ModuleBSI::ModuleTypeString = "";

ModuleBSI::ModuleBSI()
{
    ModuleBsi.MTypeB = ModuleBsi.MTypeM = 0xFFFFFFFF;
}

int ModuleBSI::SetupBSI()
{
    if (Commands::GetBsi(ModuleBsi) != NOERROR)
        return GENERALERROR;
    /*    quint32 mtype;
        QString mtypestring;
        mtype = ((ModuleBsi.MTypeB & 0x000000FF) << 8) | (ModuleBsi.MTypeM & 0x000000FF); */
    ModuleTypeString = Config::ModuleBaseBoards()[ModuleBsi.MTypeB << 8].TextString
        + Config::ModuleMezzanineBoards()[ModuleBsi.MTypeM].TextString;
    //    ModuleBSI::Bsi bsi = ModuleBsi;
    QString tmps = ModuleTypeString;
    if (!IsKnownModule())
        return RESEMPTY;
    return NOERROR;
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

int ModuleBSI::PrereadConf(QWidget *w, QVector<S2::DataRec> *S2Config)
{
    int res;
    quint32 Bsi;

    /*    if(!StopRead)
        { */
    Bsi = ModuleBSI::Health();
    if ((Bsi & HTH_CONFIG)
        || (StdFunc::IsInEmulateMode())) // если в модуле нет конфигурации, заполнить поля по умолчанию
        return RESEMPTY;
    else // иначе заполнить значениями из модуля
    {
        // iec104::GetFile();
        if ((res = Commands::GetFileWithRestore(1, S2Config)) != NOERROR)
        {
            QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля " : "прибора ");
            EMessageBox::error(w, "ошибка", "Ошибка чтения конфигурации из " + tmps);
            return GENERALERROR;
        }
    }
    //    }
    return NOERROR;
}
