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
    static Error::Msg GetBsi(ModuleBSI::Bsi &bsi);
    static Error::Msg GetFileWithRestore(int filenum, S2ConfigType *data);
    static Error::Msg GetFile(int filenum, QByteArray &ba);
    static Error::Msg GetBac(char BacNum, void *ptr, int ptrsize);
    static Error::Msg GetBd(char BdNum, void *BdPtr, int BdPtrSize);
    static Error::Msg GetBda(char board, void *BdPtr, int BdPtrSize);
    static Error::Msg GetBt(char BtNum, void *BoPtr, int &BoPtrSize);
    static Error::Msg WriteBac(char BacNum, void *BacPtr, int BacPtrSize);
    static Error::Msg WriteFile(int filenum, S2ConfigType *data);
    static Error::Msg WriteHiddenBlock(char board, void *HPtr, int HPtrSize);
    static Error::Msg EraseTechBlock(char block);
    static Error::Msg Connect();
    static void Disconnect();
    // static bool isConnected();
    static Error::Msg WriteTimeMNK(uint32_t Time, int TimeSize);
    static Error::Msg GetTimeMNK(uint &Time);
    static Error::Msg WriteBd(char BdNum, void *BdPtr, int BdPtrSize);
    static Error::Msg WriteCom(char ComNum);
    static Error::Msg RunVPO();
    static Error::Msg TestCom(char OnOff);

signals:
    void setupConnection();

private:
};

#endif // COMMANDS_H
