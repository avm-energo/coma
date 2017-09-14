// commands.cpp
#include <QCoreApplication>
#include "commands.h"
#ifdef USBENABLE
#include "eusbhid.h"
#else
#ifdef COMPORTENABLE
#include "canal.h"
#endif
#endif

int Commands::GetBsi()
{
#ifdef USBENABLE
    uh->Send(CN_GBsi, BT_NONE, &pc.ModuleBsi, sizeof(publicclass::Bsi));
    return uh->result;
#endif
#ifdef COMPORTENABLE
    cn->Send(CN_GBsi, BT_NONE, &pc.ModuleBsi, sizeof(publicclass::Bsi));
    return cn->result;
#endif
}

int Commands::GetFile(quint32 filenum, QVector<publicclass::DataRec> *data)
{
#ifdef USBENABLE
    uh->Send(CN_GF, BT_NONE, NULL, 0, filenum, data);
    return uh->result;
#endif
#ifdef COMPORTENABLE
    cn->Send(CN_GF, BT_NONE, NULL, 0, filenum, data);
    return cn->result;
#endif
}

int Commands::WriteFile(void *ptr, quint32 filenum, QVector<publicclass::DataRec> *data)
{
#ifdef USBENABLE
    uh->Send(CN_WF, BT_NONE, ptr, 0, filenum, data);
    return uh->result;
#endif
#ifdef COMPORTENABLE
    cn->Send(CN_WF, BT_NONE, ptr, 0, filenum, data);
    return cn->result;
#endif
}

int Commands::GetBac(void *BacPtr, int BacPtrSize, int BacNum)
{
#ifdef USBENABLE
    uh->Send(CN_GBac, BacNum, BacPtr, BacPtrSize);
    return uh->result;
#endif
#ifdef COMPORTENABLE
    cn->Send(CN_GBac, BacNum, BacPtr, BacPtrSize);
    return cn->result;
#endif
}

int Commands::Connect()
{
#ifdef USBENABLE
    if (uh->Connect())
        return NOERROR;
#endif
#ifdef COMPORTENABLE
    if (cn->Connect())
        return NOERROR;
#endif
    return GENERALERROR;
}

void Commands::Disconnect()
{
#ifdef USBENABLE
    uh->Disconnect();
#endif
#ifdef COMPORTENABLE
    cn->Disconnect();
#endif
}

int Commands::GetBd(int BdNum, void *BdPtr, int BdPtrSize)
{
#ifdef USBENABLE
    uh->Send(CN_GBd, BdNum, BdPtr, BdPtrSize);
    return uh->result;
#endif
#ifdef COMPORTENABLE
    cn->Send(CN_GBd, BdNum, BdPtr, BdPtrSize);
    return cn->result;
#endif
}

bool Commands::isConnected()
{
#ifdef USBENABLE
    return uh->Connected;
#endif
#ifdef COMPORTENABLE
    return cn->Connected;
#endif
}
