#ifndef COMMANDS_H
#define COMMANDS_H

#include "s2.h"
#ifdef USBENABLE
#include "eusbhid.h"
#else
#ifdef COMPORTENABLE
#include "eusbcom.h"
#endif
#endif
#include "modulebsi.h"

//#define TECH_Bd0    0   // блок данных с температурой кристалла и напряжением батареи
#define TECH_Bo     1   // технологический блок осциллограмм
#define TECH_Be     2   // технологический блок событий
#define TECH_Bte    3   // технологический блок технологических событий
#define TECH_SWJ    4   // технологический блок журнала переключений
#define TECH_RA     5   // технологический блок рабочего архива

#define CM_CONFIGFILE   1
#define CM_FIRMWAREFILE 3
#define CM_SWJFILE      17
#define CM_EVENTSFILE   18
#define WORK_MODE       0
#define TUNE_MODE_1000  1
#define TUNE_MODE_100   2

class Commands
{
    explicit Commands();
public:
    static int GetBsi(ModuleBSI::Bsi &bsi);
    static int GetFile(int filenum, QVector<S2::DataRec> *data);
    static int GetOsc(int filenum, void *ptr); // приём осциллограмм, в ptr возвращается целиком весь считанный буфер
    static int GetBac(char BacNum, void *ptr, int ptrsize);
    static int GetBd(char BdNum, void *BdPtr, int BdPtrSize);
    static int GetBda(char board, void *BdPtr, int BdPtrSize);
    static int GetBt(char BtNum, void *BoPtr, int &BoPtrSize);
    static int WriteBac(char BacNum, void *BacPtr, int BacPtrSize);
    static int WriteFile(void *ptr, int filenum, QVector<S2::DataRec> *data);
    static int WriteHiddenBlock(char board, void *HPtr, int HPtrSize);
    static int EraseTechBlock(char block);
    static int Connect();
    static void Disconnect();
    static bool isConnected();
    static int SetUsingVariant(char variant); // задать вариант использования (выбор набора регулировочных параметров)
    static int GetUsingVariant(char &variant); // получить вариант использования (выбор набора регулировочных параметров)
    static int SetMode(char mode); // задать режим работы (коэффициенты при регулировке/рабочий режим)
    static int GetMode(int &mode); // прочитать текущий режим работы
    static int WriteTimeMNK(uint32_t *Time,  int TimeSize);
    static int GetTimeMNK(uint &Time);

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
