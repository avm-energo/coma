// commands.cpp
#include <QCoreApplication>
#include "commands.h"
#include "error.h"

#ifdef USBENABLE
    EUsbHid *cn;
#else
#ifdef COMPORTENABLE
    EUsbCom *cn;
#endif
#endif

Commands::Commands()
{
}

int Commands::GetBsi(ModuleBSI::Bsi &bsi)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_GBsi, BoardTypes::BT_NONE, &bsi, sizeof(ModuleBSI::Bsi));
    return cn->result;
#else
    return 0;
#endif
}

int Commands::GetFile(int filenum, QVector<S2::DataRec> *data)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_GF, BoardTypes::BT_NONE, nullptr, 0, filenum, data);
    return cn->result;
#else
    Q_UNUSED(filenum);
    Q_UNUSED(data);
    return 0;
#endif
}

int Commands::GetOsc(int filenum, void *ptr)
{
#if PROGSIZE != PROGSIZE_EMUL
    /*if ((filenum < CN_MINOSCID) || (filenum > CN_MAXOSCID))
        return Error::ER_GENERALERROR;*/
    cn->Send(CN_GF, BoardTypes::BT_NONE, ptr, 0, filenum);
    return cn->result;
#else
    Q_UNUSED(filenum);
    Q_UNUSED(ptr);
    return 0;
#endif
}

int Commands::WriteFile(void *ptr, int filenum, QVector<S2::DataRec> *data)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_WF, BoardTypes::BT_BASE, ptr, 0, filenum, data);
    return cn->result;
#else
    Q_UNUSED(ptr);
    Q_UNUSED(filenum);
    Q_UNUSED(data);
    return 0;
#endif
}

int Commands::WriteHiddenBlock(char board, void *HPtr, int HPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_WHv, board, HPtr, HPtrSize);
    return cn->result;
#else
    Q_UNUSED(board);
    Q_UNUSED(HPtr);
    Q_UNUSED(HPtrSize);
    return 0;
#endif
}

int Commands::GetBac(char BacNum, void *BacPtr, int BacPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_GBac, BacNum, BacPtr, BacPtrSize);
    return cn->result;
#else
    Q_UNUSED(BacNum);
    Q_UNUSED(BacPtr);
    Q_UNUSED(BacPtrSize);
    return 0;
#endif
}

int Commands::Connect()
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn->Connect())
        return Error::ER_NOERROR;
    return Error::ER_GENERALERROR;
#else
    return 0;
#endif
}

void Commands::Disconnect()
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Disconnect();
#endif
}

int Commands::GetBd(char BdNum, void *BdPtr, int BdPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_GBd, BdNum, BdPtr, BdPtrSize);
    return cn->result;
#else
    Q_UNUSED(BdNum);
    Q_UNUSED(BdPtr);
    Q_UNUSED(BdPtrSize);
    return 0;
#endif
}

int Commands::GetBda(char board, void *BdPtr, int BdPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_GBda, board, BdPtr, BdPtrSize);
    return cn->result;
#else
    Q_UNUSED(board);
    Q_UNUSED(BdPtr);
    Q_UNUSED(BdPtrSize);
    return 0;
#endif
}

int Commands::GetBt(char BtNum, void *BtPtr, int &BtPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_GBt, BtNum, BtPtr, BtPtrSize);
    BtPtrSize = cn->RDSize;
    return cn->result;
#else
    Q_UNUSED(BtNum);
    Q_UNUSED(BtPtr);
    Q_UNUSED(BtPtrSize);
    return 0;
#endif
}

int Commands::WriteBac(char BacNum, void *BacPtr, int BacPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_WBac, BacNum, BacPtr, BacPtrSize);
    return cn->result;
#else
    Q_UNUSED(BacNum);
    Q_UNUSED(BacPtr);
    Q_UNUSED(BacPtrSize);
    return 0;
#endif
}

int Commands::EraseTechBlock(char block)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_Ert, block);
    return cn->result;
#else
    Q_UNUSED(block);
    return 0;
#endif
}

bool Commands::isConnected()
{
#if PROGSIZE != PROGSIZE_EMUL
    return cn->Connected;
#else
    return false;
#endif
}

int Commands::SetUsingVariant(char variant)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_NVar, variant);
    return cn->result;
#else
    Q_UNUSED(variant);
    return 0;
#endif
}

int Commands::GetUsingVariant(char &variant)
{
#if PROGSIZE != PROGSIZE_EMUL
    char tmpi;
    cn->Send(CN_GVar, BoardTypes::BT_NONE, &tmpi, 1);
    variant = tmpi;
    return cn->result;
#else
    Q_UNUSED(variant);
    return 0;
#endif
}

int Commands::SetMode(char mode)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_SMode, mode);
    return cn->result;
#else
    Q_UNUSED(mode);
    return 0;
#endif
}

int Commands::GetMode(int &mode)
{
#if PROGSIZE != PROGSIZE_EMUL
    quint8 tmpi;
    cn->Send(CN_GMode, BoardTypes::BT_NONE, &tmpi, 1);
    mode = tmpi;
    return cn->result;
#else
    Q_UNUSED(mode);
    return 0;
#endif
}

int Commands::WriteTimeMNK(uint32_t *Time,  int TimeSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_WTime, BoardTypes::BT_NONE, Time, TimeSize);
    return cn->result;
#else
    Q_UNUSED(Time);
    return 0;
#endif
}

int Commands::GetTimeMNK(uint &Time)
{
#if PROGSIZE != PROGSIZE_EMUL
    //quint8 tmpi;
    cn->Send(CN_GTime, BoardTypes::BT_NONE, &Time, sizeof(uint));
    //Time = tmpi;
    return cn->result;
#else
    Q_UNUSED(Time);
    return 0;
#endif
}

int Commands::WriteBd(char BdNum, void *BdPtr, int BdPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_WBd, BdNum, BdPtr, BdPtrSize);
    return cn->result;
#else
    Q_UNUSED(BdNum);
    Q_UNUSED(BdPtr);
    Q_UNUSED(BdPtrSize);
    return 0;
#endif
}
int Commands::WriteCom(char ComNum)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_WCom, ComNum);
    return cn->result;
#else
    Q_UNUSED(BdNum);
    Q_UNUSED(BdPtr);
    Q_UNUSED(BdPtrSize);
    return 0;
#endif
}

int Commands::RunVPO()
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_VPO);
    return cn->result;
#endif
}
