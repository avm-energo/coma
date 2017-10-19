#ifndef COMMANDS_H
#define COMMANDS_H

#include "publicclass.h"
#ifdef USBENABLE
#include "eusbhid.h"
#else
#ifdef COMPORTENABLE
#include "eusbcom.h"
#endif
#endif

class Commands
{
    explicit Commands();
public:
    static int GetBsi();
    static int GetFile(quint32 filenum, QVector<publicclass::DataRec> *data);
    static int GetOsc(quint32 filenum, void *ptr); // приём осциллограмм, в ptr возвращается целиком весь считанный буфер
    static int GetBac(int BacNum, void *ptr, int ptrsize);
    static int GetBd(int BdNum, void *BdPtr, int BdPtrSize);
    static int GetBda(int board, void *BdPtr, int BdPtrSize);
    static int GetBo(void *BoPtr, quint32 &BoPtrSize);
    static int GetIP(void *IpPtr, int IpPtrSize);
    static int WriteBac(int BacNum, void *BacPtr, int BacPtrSize);
    static int WriteFile(void *ptr, quint32 filenum, QVector<publicclass::DataRec> *data);
    static int WriteHiddenBlock(int board, void *HPtr, int HPtrSize);
    static int EraseOsc();
    static int Connect();
    static void Disconnect();
    static bool isConnected();
    static int SetUsingVariant(int variant); // задать вариант использования (выбор набора регулировочных параметров)

private:
};

#ifdef USBENABLE
    extern EUsbHid *cn;
#else
#ifdef COMPORTENABLE
    extern EUsbCom *cn;
#endif
#endif

#endif // COMMANDS_H
