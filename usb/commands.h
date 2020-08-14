#ifndef COMMANDS_H
#define COMMANDS_H

#include "../gen/modulebsi.h"
#include "../gen/s2.h"
#include "eusbhid.h"

//#define TECH_Bd0    0   // блок данных с температурой кристалла и напряжением батареи
#define TECH_Bo 1 // технологический блок осциллограмм
#define TECH_Be 2 // технологический блок событий
#define TECH_Bte 3 // технологический блок технологических событий
#define TECH_SWJ 4 // технологический блок журнала переключений
#define TECH_RA 5 // технологический блок рабочего архива

#define CM_CONFIGFILE 1
#define CM_FIRMWAREFILE 3
#define CM_SWJFILE 17
#define CM_EVENTSFILE 18
#define WORK_MODE 0
#define TUNE_MODE_1000 1
#define TUNE_MODE_100 2

#define MAXFLOAT 3.40282347E+38F

class Commands
{
    explicit Commands();

public:
    static int GetBsi(ModuleBSI::Bsi &bsi);
    static int GetFileWithRestore(int filenum, QVector<S2::DataRec> *data);
    static int GetFile(int filenum, QByteArray &ba);
    static int GetBac(char BacNum, void *ptr, int ptrsize);
    static int GetBd(char BdNum, void *BdPtr, int BdPtrSize);
    static int GetBda(char board, void *BdPtr, int BdPtrSize);
    static int GetBt(char BtNum, void *BoPtr, int &BoPtrSize);
    static int WriteBac(char BacNum, void *BacPtr, int BacPtrSize);
    static int WriteFile(int filenum, QVector<S2::DataRec> *data);
    static int WriteHiddenBlock(char board, void *HPtr, int HPtrSize);
    static int EraseTechBlock(char block);
    static int Connect();
    static void Disconnect();
    static bool isConnected();
    static int WriteTimeMNK(uint32_t Time, int TimeSize);
    static int GetTimeMNK(uint &Time);
    static int WriteBd(char BdNum, void *BdPtr, int BdPtrSize);
    static int WriteCom(char ComNum);
    static int RunVPO();
    static int TestCom(char OnOff);

private:
};

extern EUsbHid *cn;

#endif // COMMANDS_H
