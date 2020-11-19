#include "modulebsi.h"

#include "../config/config.h"
#include "../gen/board.h"
#include "../gen/datamanager.h"
//#include "../iec104/iec104.h"
//#include "../usb/commands.h"
#include "error.h"
#include "stdfunc.h"

#include <QDebug>
#include <QMessageBox>

quint32 ModuleBSI::MType = 0;
ModuleBSI::Bsi ModuleBSI::ModuleBsi;
QString ModuleBSI::ModuleTypeString = "";

ModuleBSI::ModuleBSI(Singleton::token, QObject *parent) : QObject(parent)
{
    ModuleBsi.MTypeB = ModuleBsi.MTypeM = 0xFFFFFFFF;
    connect(&DataManager::GetInstance(), &DataManager::bitStringReceived, this, &ModuleBSI::update);
}

// void ModuleBSI::USBUpdate()
//{
//    if (Commands::GetBsi(ModuleBsi) != Error::Msg::NoError)
//        return;
//    Config::ModuleDesc typeb = Config::ModuleBaseBoards().value(ModuleBsi.MTypeB);
//    Config::ModuleDesc typem = Config::ModuleMezzanineBoards().value(ModuleBsi.MTypeM);
//    if ((ModuleBsi.MTypeB << 8) >= 0xA000 || (Config::ModuleMezzanineBoards()[ModuleBsi.MTypeM].Hex) >= 0x00A0)
//    {
//        quint32 Type = (typeb.Hex << 8) + typem.Hex;
//        ModuleTypeString = Config::ModuleBoards()[Type];
//    }
//    else
//    {
//        ModuleTypeString = typeb.TextString + typem.TextString;
//    }
//    Board::GetInstance().setTypeB(ModuleBSI::GetMType(BoardTypes::BT_BASE));
//    Board::GetInstance().setTypeM(ModuleBSI::GetMType(BoardTypes::BT_MEZONIN));

//    //    if (!IsKnownModule())
//    //        return Error::Msg::ResEmpty;
//    //    return Error::Msg::NoError;
//}

// void ModuleBSI::ETHUpdate() { }

// void ModuleBSI::MBSUpdate() { }

QString ModuleBSI::GetModuleTypeString()
{
    return ModuleTypeString;
}

quint32 ModuleBSI::GetMType(BoardTypes type)
{
    return (type == BT_MEZONIN) ? ModuleBsi.MTypeM : ModuleBsi.MTypeB;
}

QString ModuleBSI::UID()
{
    return (QString::number(ModuleBsi.UIDHigh, 16) + QString::number(ModuleBsi.UIDMid, 16)
        + QString::number(ModuleBsi.UIDLow, 16));
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

bool ModuleBSI::noConfig()
{
    return ((Health() & HTH_CONFIG) || (StdFunc::IsInEmulateMode()));
}

bool ModuleBSI::noRegPars()
{
    return Health() & HTH_REGPARS;
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

// Error::Msg ModuleBSI::PrereadConf(QWidget *w, S2ConfigType *S2Config)
//{
//    //    quint32 Bsi;

//    /*    if(!StopRead)
//        { */
//    //    Bsi = ModuleBSI::Health();
//    //    if ((Bsi & HTH_CONFIG) || (StdFunc::IsInEmulateMode())) // если в модуле нет конфигурации, заполнить
//    if (noConfig()) // если в модуле нет конфигурации, заполнить поля по умолчанию
//        return Error::Msg::ResEmpty;
//    else // иначе заполнить значениями из модуля
//    {
//        // iec104::GetFile();
//        if ((Commands::GetFileWithRestore(1, S2Config)) != Error::Msg::NoError)
//        {
//            QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля " : "прибора ");
//            QMessageBox::critical(w, "ошибка", "Ошибка чтения конфигурации из " + tmps);
//            return Error::Msg::GeneralError;
//        }
//    }
//    //    }
//    return Error::Msg::NoError;
//}

void ModuleBSI::update(DataTypes::BitStringStruct &bs)
{
    //    switch (Board::GetInstance().interfaceType())
    //    {
    //    case Board::InterfaceType::USB:
    //        USBUpdate();
    //        break;
    //    case Board::InterfaceType::Ethernet:
    //        ETHUpdate();
    //        break;
    //    case Board::InterfaceType::RS485:
    //        MBSUpdate();
    //        break;
    //    default:
    //        break;
    //    }
    //    QList<DataTypes::SignalsStruct> list;
    //    DataManager::getSignals(BSIREG, BSIENDREG, DataTypes::SignalTypes::BitString, list);
    //    if (!list.isEmpty())
    //    {
    //        foreach (DataTypes::SignalsStruct signal, list)
    //        {
    //            DataTypes::BitStringStruct bs = qvariant_cast<DataTypes::BitStringStruct>(signal.data);
    //            memcpy(&startadr, &(sig->BS.SigAdr[0]), sizeof(sig->BS.SigAdr));
    //            signum = sig->SigNumber;
    //            INFOMSG("FillBSIe(): address=" + QString::number(startadr));

    //            if ((signum < sizeof(ModuleBSI::ModuleBsi)) && (startadr >= BSIREG && startadr <= BSIENDREG))
    //            {
    //                for (size_t i = 0; i < signum; ++i)
    //                    memcpy(((quint32 *)(&ModuleBSI::ModuleBsi) + (i + startadr - 1)), (((quint32
    //                    *)(&sig->BS.SigVal) + 4 * i)),
    //                        sizeof(sig->BS.SigVal));
    memcpy(((quint32 *)(&ModuleBSI::ModuleBsi) + (bs.sigAdr - 1)), &bs.sigVal, sizeof(quint32));
    if (bs.sigAdr == BSIHTHREG)
        emit ModuleBSI::GetInstance().BSIHealthUpdated(GetHealth());
    //        }
    Board::GetInstance().setTypeB(ModuleBSI::GetMType(BoardTypes::BT_BASE));
    Board::GetInstance().setTypeM(ModuleBSI::GetMType(BoardTypes::BT_MEZONIN));
    //        return true;
    //    }
    //    return false;
}

// void MBSUpdate()
//{
//    Board::GetInstance().setTypeB(ModuleBSI::GetMType(BoardTypes::BT_BASE));
//    Board::GetInstance().setTypeM(ModuleBSI::GetMType(BoardTypes::BT_MEZONIN));
//}
