// commands.cpp
#include <QCoreApplication>
#include "commands.h"

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

int Commands::GetBsi()
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_GBsi, BT_NONE, &pc.ModuleBsi, sizeof(publicclass::Bsi));
    return cn->result;
#else
    return 0;
#endif
}

int Commands::GetFile(quint32 filenum, QVector<publicclass::DataRec> *data)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_GF, BT_NONE, NULL, 0, filenum, data);
    return cn->result;
#else
    Q_UNUSED(filenum);
    Q_UNUSED(data);
    return 0;
#endif
}

int Commands::WriteFile(void *ptr, quint32 filenum, QVector<publicclass::DataRec> *data)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_WF, BT_NONE, ptr, 0, filenum, data);
    return cn->result;
#else
    Q_UNUSED(ptr);
    Q_UNUSED(filenum);
    Q_UNUSED(data);
    return 0;
#endif
}

int Commands::WriteHiddenBlock(int board, void *HPtr, int HPtrSize)
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

int Commands::GetBac(int BacNum, void *BacPtr, int BacPtrSize)
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
        return NOERROR;
    return GENERALERROR;
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

int Commands::GetBd(int BdNum, void *BdPtr, int BdPtrSize)
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

int Commands::GetBda(int board, void *BdPtr, int BdPtrSize)
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

int Commands::GetBo(void *BoPtr, quint32 &BoPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_GBo, BT_NONE, BoPtr, BoPtrSize);
    BoPtrSize = cn->RDSize;
    return cn->result;
#else
    Q_UNUSED(BoPtr);
    Q_UNUSED(BoPtrSize);
    return 0;
#endif
}

int Commands::GetIP(void *IpPtr, int IpPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_IP, BT_NONE, IpPtr, IpPtrSize);
    return cn->result;
#else
    Q_UNUSED(IpPtr);
    Q_UNUSED(IpPtrSize);
    return 0;
#endif
}

int Commands::WriteBac(int BacNum, void *BacPtr, int BacPtrSize)
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

int Commands::EraseOsc()
{
#if PROGSIZE != PROGSIZE_EMUL
    cn->Send(CN_OscEr);
    return cn->result;
#else
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
