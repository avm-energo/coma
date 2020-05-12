// commands.cpp
#include <QCoreApplication>
#include "commands.h"
#include "error.h"

EUsbHid *cn;

Commands::Commands()
{
    cn = 0;
}

int Commands::GetBsi(ModuleBSI::Bsi &bsi)
{
#if PROGSIZE != PROGSIZE_EMUL
    QByteArray ba;
    if (cn != 0)
    {
        cn->Send(CN_GBsi, BoardTypes::BT_NONE, ba, sizeof(ModuleBSI::Bsi));
        memcpy(&bsi, &(ba.data()[0]), sizeof(ModuleBSI::Bsi));
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    return 0;
#endif
}

int Commands::GetFileWithRestore(int filenum, QVector<S2::DataRec> *data)
{
#if PROGSIZE != PROGSIZE_EMUL
    QByteArray ba;
    if (cn != 0)
    {
//        cn->Send(CN_GF, BoardTypes::BT_NONE, tmp, 0, filenum, data);
        cn->SendPtr(CN_GF, BoardTypes::BT_NONE, ba, filenum);
        // проверка контрольной суммы файла
        quint32 crctocheck;
        quint32 basize = ba.size();
        memcpy(&crctocheck, &(ba.data())[8], sizeof(quint32));
        if (!S2::CheckCRC32(&(ba.data())[16], (basize-16), crctocheck))
            return Error::ER_GENERALERROR;
        return S2::RestoreDataMem(&ba.data()[0], basize, data);
//        return cn->result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(filenum);
    Q_UNUSED(data);
    return 0;
#endif
}

int Commands::GetFile(int filenum, QByteArray &ba)
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn != 0)
    {
//        cn->Send(CN_GF, BoardTypes::BT_NONE, ptr, 0, filenum);
        cn->SendPtr(CN_GF, BoardTypes::BT_NONE, ba, filenum);
        quint32 crctocheck;
        quint32 basize = ba.size();
        memcpy(&crctocheck, &(ba.data())[8], sizeof(quint32));
        if (!S2::CheckCRC32(&(ba.data())[16], (basize-16), crctocheck))
            return Error::ER_GENERALERROR;
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(filenum)
    Q_UNUSED(ptr)
    return 0;
#endif
}

int Commands::WriteFile(void *ptr, int filenum, QVector<S2::DataRec> *data)
{
#if PROGSIZE != PROGSIZE_EMUL

    if (cn != 0)
    {
        cn->Send(CN_WF, BoardTypes::BT_BASE, ptr, 0, filenum, data);
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
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
    if (cn != 0)
    {
        cn->Send(CN_WHv, board, HPtr, HPtrSize);
        return cn->result;
    }
    return Error::ER_GENERALERROR;
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
    if (cn != 0)
    {
        cn->Send(CN_GBac, BacNum, BacPtr, BacPtrSize);
        return cn->result;
    }
    return Error::ER_GENERALERROR;
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
    if (cn != 0)
    {
        if (cn->Connect())
            return Error::ER_NOERROR;
    }
    return Error::ER_GENERALERROR;
#else
    return 0;
#endif
}

void Commands::Disconnect()
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn != 0)
        cn->Disconnect();
#endif
}

int Commands::GetBd(char BdNum, void *BdPtr, int BdPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn != 0)
    {
        cn->Send(CN_GBd, BdNum, BdPtr, BdPtrSize);
        return cn->result;
    }
    return Error::ER_GENERALERROR;
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
    if (cn != 0)
    {
        cn->Send(CN_GBda, board, BdPtr, BdPtrSize);
        return cn->result;
    }
    return Error::ER_GENERALERROR;
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
    if (cn != 0)
    {
        cn->Send(CN_GBt, BtNum, BtPtr, BtPtrSize);
        BtPtrSize = cn->RDSize;
        return cn->result;
    }
    return Error::ER_GENERALERROR;
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
    if (cn != 0)
    {
        cn->Send(CN_WBac, BacNum, BacPtr, BacPtrSize);
        return cn->result;
    }
    return Error::ER_GENERALERROR;
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
    if (cn != 0)
    {
        cn->Send(CN_Ert, block);
        return cn->result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(block);
    return 0;
#endif
}

bool Commands::isConnected()
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn != 0)
        return cn->Connected;
    return false;
#else
    return false;
#endif
}

int Commands::WriteTimeMNK(uint32_t *Time,  int TimeSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn != 0)
    {
        cn->Send(CN_WTime, BoardTypes::BT_NONE, Time, TimeSize);
        return cn->result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(Time);
    return 0;
#endif
}

int Commands::GetTimeMNK(uint &Time)
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn != 0)
    {
        //quint8 tmpi;
        cn->Send(CN_GTime, BoardTypes::BT_NONE, &Time, sizeof(uint));
        //Time = tmpi;
        return cn->result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(Time);
    return 0;
#endif
}

int Commands::WriteBd(char BdNum, void *BdPtr, int BdPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn != 0)
    {
        cn->Send(CN_WBd, BdNum, BdPtr, BdPtrSize);
        return cn->result;
    }
    return Error::ER_GENERALERROR;
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
    if (cn != 0)
    {
        cn->Send(CN_WCom, ComNum);
        return cn->result;
    }
    return Error::ER_GENERALERROR;
#else
    return 0;
#endif
}

int Commands::RunVPO()
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn != 0)
    {
        cn->Send(CN_VPO);
        return cn->result;
    }
    return Error::ER_GENERALERROR;
#else
    return 0;
#endif
}

int Commands::TestCom(char OnOff)
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn != 0)
    {
        cn->Send(CN_STest, OnOff);
        return cn->result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(OnOff);
    return 0;
#endif
}
