// commands.cpp
#include <QCoreApplication>
//#include "canal.h"
#include "eusbhid.h"

/*int CN_GetBsi(void *ptr, quint32 size)
{
    cn->Send(CN_GBsi, BT_NONE, ptr, size);
    return cn->result;
}

int CN_GetFile(void *ptr, quint32 filenum)
{
    cn->Send(CN_GF, BT_NONE, ptr, 0, filenum);
    return NOERROR;
}

int CN_PutFile(void *ptr, quint32 filenum, quint32 size)
{
    cn->Send(CN_WF, BT_NONE, ptr, size, filenum);
    return NOERROR;
} */

int UH_GetBsi(void *ptr, quint32 size)
{
    uh->Send(CN_GBsi, BT_NONE, ptr, size);
    return uh->result;
}

int UH_GetFile(void *ptr, quint32 filenum)
{
    uh->Send(CN_GF, BT_NONE, ptr, 0, filenum);
    return NOERROR;
}

int UH_PutFile(void *ptr, quint32 filenum, quint32 size)
{
    uh->Send(CN_WF, BT_NONE, ptr, size, filenum);
    return NOERROR;
}
