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
    cn->Send(CN_GBsi, BT_NONE, &pc.ModuleBsi, sizeof(publicclass::Bsi));
    return cn->result;
}

int Commands::GetFile(quint32 filenum, QVector<publicclass::DataRec> *data)
{
    cn->Send(CN_GF, BT_NONE, NULL, 0, filenum, data);
    return cn->result;
}

int Commands::WriteFile(void *ptr, quint32 filenum, QVector<publicclass::DataRec> *data)
{
    cn->Send(CN_WF, BT_NONE, ptr, 0, filenum, data);
    return cn->result;
}

int Commands::WriteHiddenBlock(int board, void *HPtr, int HPtrSize)
{
    cn->Send(CN_WHv, board, HPtr, HPtrSize);
    return cn->result;
}

int Commands::GetBac(int BacNum, void *BacPtr, int BacPtrSize)
{
    cn->Send(CN_GBac, BacNum, BacPtr, BacPtrSize);
    return cn->result;
}

int Commands::Connect()
{
    if (cn->Connect())
        return NOERROR;
    return GENERALERROR;
}

void Commands::Disconnect()
{
    cn->Disconnect();
}

int Commands::GetBd(int BdNum, void *BdPtr, int BdPtrSize)
{
    cn->Send(CN_GBd, BdNum, BdPtr, BdPtrSize);
    return cn->result;
}

int Commands::GetBda(int board, void *BdPtr, int BdPtrSize)
{
    cn->Send(CN_GBda, board, BdPtr, BdPtrSize);
    return cn->result;
}

int Commands::GetBo(void *BoPtr, int BoPtrSize)
{
    cn->Send(CN_GBo, BT_NONE, BoPtr, BoPtrSize);
    return cn->result;
}

int Commands::GetIP(void *IpPtr, int IpPtrSize)
{
    cn->Send(CN_IP, BT_NONE, IpPtr, IpPtrSize);
    return cn->result;
}

int Commands::WriteBac(int BacNum, void *BacPtr, int BacPtrSize)
{
    cn->Send(CN_WBac, BacNum, BacPtr, BacPtrSize);
    return cn->result;
}

int Commands::EraseOsc()
{
    cn->Send(CN_OscEr);
    return cn->result;
}

bool Commands::isConnected()
{
    return cn->Connected;
}
