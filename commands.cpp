// commands.cpp
#include <QCoreApplication>
#include "canal.h"

int CN_GetBsi(void *ptr, quint32 size)
{
    cn->Send(CN_GBsi, Canal::BT_NONE, ptr, size);
    while ((cn->Busy) && (cn->Connected))
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    return cn->result;
}

int CN_GetFile(void *ptr, quint32 filenum)
{
    return NOERROR;
}

int CN_PutFile(void *ptr, quint32 filenum, quint32 size)
{
    cn->Send(CN_WF, Canal::BT_NONE, ptr, size, filenum);
    return NOERROR;
}