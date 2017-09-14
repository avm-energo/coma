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

class Commands
{
public:
    static int GetBsi();
    static int GetFile(quint32 filenum, QVector<publicclass::DataRec> *data);
    static int WriteFile(void *ptr, quint32 filenum, QVector<publicclass::DataRec> *data);
    static int GetBac(void *ptr, int ptrsize, int BacNum);
    static int Connect();
    static void Disconnect();
    static int GetBd(int BdNum, void *BdPtr, int BdPtrSize);
    static bool isConnected();
};

#endif // COMMANDS_H
