#ifndef COMMANDS_H
#define COMMANDS_H

#include "publicclass.h"
#ifdef USBENABLE
#include "eusbhid.h"
#else
#ifdef COMPORTENABLE
#include "canal.h"
#endif
#endif

    int CM_GetBsi();
    int CM_GetFile(quint32 filenum, QVector<publicclass::DataRec> *data);
    int CM_WriteFile(void *ptr, quint32 filenum, QVector<publicclass::DataRec> *data);
    int CM_GetBac(void *ptr, int ptrsize, int BacNum);
    int CM_Connect();
    void CM_Disconnect();
    int CM_GetBd(int BdNum, void *BdPtr, int BdPtrSize);

#endif // COMMANDS_H
