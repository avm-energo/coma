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

//#define TECH_Bd0    0   // блок данных с температурой кристалла и напряжением батареи
#define TECH_Bo     1   // технологический блок осциллограмм
#define TECH_Be     2   // технологический блок событий
#define TECH_Bte    3   // технологический блок технологических событий
#define TECH_SWJ    4   // технологический блок журнала переключений
#define TECH_RA     5   // технологический блок рабочего архива

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
    static int GetBt(int BtNum, void *BoPtr, quint32 &BoPtrSize);
    static int WriteBac(int BacNum, void *BacPtr, int BacPtrSize);
    static int WriteFile(void *ptr, quint32 filenum, QVector<publicclass::DataRec> *data);
    static int WriteHiddenBlock(int board, void *HPtr, int HPtrSize);
    static int EraseTechBlock(int block);
    static int Connect();
    static void Disconnect();
    static bool isConnected();
    static int SetUsingVariant(int variant); // задать вариант использования (выбор набора регулировочных параметров)
    static int GetUsingVariant(int &variant); // получить вариант использования (выбор набора регулировочных параметров)

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
