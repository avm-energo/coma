#ifndef COMMANDS_H
#define COMMANDS_H

#include "../gen/modulebsi.h"
#include "../gen/s2.h"
#include "eprotocom.h"

class Commands : QObject
{

    Q_OBJECT
public:
    explicit Commands();
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

signals:
    void setupConnection();

private:
};

#endif // COMMANDS_H
