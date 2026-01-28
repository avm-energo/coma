// commands.cpp
#include "commands.h"

#include "error.h"

#include <QCoreApplication>

EUsbHid *cn;

Commands::Commands() { }

int Commands::GetBsi(ModuleBSI::Bsi &bsi)
{
    cn->Send(CN_GBsi, BoardTypes::BT_NONE, &bsi, sizeof(ModuleBSI::Bsi));
    return cn->result;
}

int Commands::GetFile(int filenum, QList<S2::DataRec> data)
{
    cn->Send(CN_GF, BoardTypes::BT_NONE, nullptr, 0, filenum, data);
    return cn->result;
}

int Commands::GetOsc(int filenum, void *ptr)
{
    /*if ((filenum < CN_MINOSCID) || (filenum > CN_MAXOSCID))
        return Error::ER_GENERALERROR;*/
    cn->Send(CN_GF, BoardTypes::BT_NONE, ptr, 0, filenum);
    return cn->result;
}

int Commands::WriteFile(void *ptr, int filenum, QList<S2::DataRec> data)
{
    cn->Send(CN_WF, BoardTypes::BT_BASE, ptr, 0, filenum, data);
    return cn->result;
}

int Commands::WriteHiddenBlock(char board, void *HPtr, int HPtrSize)
{
    cn->Send(CN_WHv, board, HPtr, HPtrSize);
    return cn->result;
}

int Commands::GetBac(char BacNum, void *BacPtr, int BacPtrSize)
{
    cn->Send(CN_GBac, BacNum, BacPtr, BacPtrSize);
    return cn->result;
}

int Commands::Connect()
{
    if (cn->Connect())
        return Error::ER_NOERROR;
    return Error::ER_GENERALERROR;
}

void Commands::Disconnect()
{
    cn->Disconnect();
}

int Commands::GetBd(char BdNum, void *BdPtr, int BdPtrSize)
{
    cn->Send(CN_GBd, BdNum, BdPtr, BdPtrSize);
    return cn->result;
}

int Commands::GetBda(char board, void *BdPtr, int BdPtrSize)
{
    cn->Send(CN_GBda, board, BdPtr, BdPtrSize);
    return cn->result;
}

int Commands::GetBt(char BtNum, void *BtPtr, int &BtPtrSize)
{
    cn->Send(CN_GBt, BtNum, BtPtr, BtPtrSize);
    BtPtrSize = cn->RDSize;
    return cn->result;
}

int Commands::WriteBac(char BacNum, void *BacPtr, int BacPtrSize)
{
    cn->Send(CN_WBac, BacNum, BacPtr, BacPtrSize);
    return cn->result;
}

int Commands::EraseTechBlock(char block)
{
    cn->Send(CN_Ert, block);
    return cn->result;
}

bool Commands::isConnected()
{
    return cn->Connected;
}

int Commands::SetUsingVariant(char variant)
{
    cn->Send(CN_NVar, variant);
    return cn->result;
}

int Commands::GetUsingVariant(char &variant)
{
    char tmpi;
    cn->Send(CN_GVar, BoardTypes::BT_NONE, &tmpi, 1);
    variant = tmpi;
    return cn->result;
}

int Commands::SetMode(char mode)
{
    cn->Send(CN_SMode, mode);
    return cn->result;
}

int Commands::GetMode(int &mode)
{
    quint8 tmpi;
    cn->Send(CN_GMode, BoardTypes::BT_NONE, &tmpi, 1);
    mode = tmpi;
    return cn->result;
}

int Commands::WriteTimeMNK(qint64 *Time, int TimeSize)
{
    cn->Send(CN_WTime, BoardTypes::BT_NONE, Time, TimeSize);
    return cn->result;
}

int Commands::GetTimeMNK(uint &Time)
{
    // quint8 tmpi;
    cn->Send(CN_GTime, BoardTypes::BT_NONE, &Time, sizeof(uint));
    // Time = tmpi;
    return cn->result;
}

int Commands::WriteBd(char BdNum, void *BdPtr, int BdPtrSize)
{
    cn->Send(CN_WBd, BdNum, BdPtr, BdPtrSize);
    return cn->result;
}
int Commands::WriteCom(char ComNum)
{
    cn->Send(CN_WCom, ComNum);
    return cn->result;
}

int Commands::RunVPO()
{
    cn->Send(CN_VPO);
    return cn->result;
}

int Commands::TestCom(char OnOff)
{
    cn->Send(CN_STest, OnOff);
    return cn->result;
}
