#include "modulebsi.h"

#include "../config/config.h"
#include "../gen/board.h"
#include "../usb/commands.h"
#include "error.h"
#include "stdfunc.h"

#include <QDebug>
#include <QMessageBox>

quint32 ModuleBSI::MType = 0;
ModuleBSI::Bsi ModuleBSI::ModuleBsi;
QString ModuleBSI::ModuleTypeString = "";

ModuleBSI::ModuleBSI()
{
    ModuleBsi.MTypeB = ModuleBsi.MTypeM = 0xFFFFFFFF;
}

Error::Msg ModuleBSI::SetupBSI()
{
    if (Commands::GetBsi(ModuleBsi) != Error::Msg::NoError)
        return Error::Msg::GeneralError;
    auto *typeb = &Config::ModuleBaseBoards().value(ModuleBsi.MTypeB);
    auto *typem = &Config::ModuleMezzanineBoards().value(ModuleBsi.MTypeM);
    if ((ModuleBsi.MTypeB << 8) >= 0xA000 || (Config::ModuleMezzanineBoards()[ModuleBsi.MTypeM].Hex) >= 0x00A0)
    {
        quint32 Type = (typeb->Hex << 8) + typem->Hex;
        ModuleTypeString = Config::ModuleBoards()[Type];
    }
    else
    {
        ModuleTypeString = typeb->TextString + typem->TextString;
    }

    if (!IsKnownModule())
        return Error::Msg::ResEmpty;
    return Error::Msg::NoError;
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
    if (Config::ModuleBaseBoards().keys().contains(ModuleBsi.MTypeB))
    {
        if (ModuleBsi.MTypeM != Config::MTM_00)
        {
            return Config::ModuleMezzanineBoards().keys().contains(ModuleBsi.MTypeM);
        }
        return true;
    }
    return false;
}

Error::Msg ModuleBSI::PrereadConf(QWidget *w, S2ConfigType *S2Config)
{
    quint32 Bsi;

    /*    if(!StopRead)
        { */
    Bsi = ModuleBSI::Health();
    if ((Bsi & HTH_CONFIG) || (StdFunc::IsInEmulateMode())) // если в модуле нет конфигурации, заполнить
                                                            // поля по умолчанию
        return Error::Msg::ResEmpty;
    else // иначе заполнить значениями из модуля
    {
        // iec104::GetFile();
        if ((Commands::GetFileWithRestore(1, S2Config)) != Error::Msg::NoError)
        {
            QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля " : "прибора ");
            QMessageBox::critical(w, "ошибка", "Ошибка чтения конфигурации из " + tmps);
            return Error::Msg::GeneralError;
        }
    }
    //    }
    return Error::Msg::NoError;
}
