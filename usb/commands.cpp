// commands.cpp
#include "commands.h"

#include "../gen/error.h"
#include "../gen/timefunc.h"

#include <QCoreApplication>

Commands::Commands()
{
}

int Commands::Connect()
{

    EUsbHid::GetInstance()->Connect();
    return NOERROR;
}

bool Commands::isConnected()
{
    return EUsbHid::GetInstance()->isConnected();
}

void Commands::Disconnect()
{
    EUsbHid::GetInstance()->Disconnect();
}

int Commands::GetBsi(ModuleBSI::Bsi &bsi)
{
    QByteArray ba;
    EUsbHid::GetInstance()->SendIn(CN::Read::BlkStartInfo, BoardTypes::BT_NONE, ba, sizeof(ModuleBSI::Bsi));
    // cn->SendIn(CN::Read::BlkStartInfo, BoardTypes::BT_NONE, ba, sizeof(ModuleBSI::Bsi));
    memcpy(&bsi, &(ba.data()[0]), sizeof(ModuleBSI::Bsi));
    return EUsbHid::GetInstance()->Result;
}

int Commands::GetFileWithRestore(int filenum, QVector<S2::DataRec> *data)
{
    QByteArray ba;
    EUsbHid::GetInstance()->SendFile(CN::Read::File, BoardTypes::BT_NONE, filenum, ba);
    // проверка контрольной суммы файла
    quint32 crctocheck;
    quint32 basize = ba.size();
    if (basize < 17)
    {
        ERMSG("Ошибка basize");
        return GENERALERROR;
    }

    memcpy(&crctocheck, &(ba.data())[8], sizeof(quint32));
    if (!S2::CheckCRC32(&(ba.data())[16], (basize - 16), crctocheck))
        return GENERALERROR;
    return S2::RestoreDataMem(&ba.data()[0], basize, data);
}

int Commands::GetFile(int filenum, QByteArray &ba)
{
    EUsbHid::GetInstance()->SendFile(CN::Read::File, BoardTypes::BT_NONE, filenum, ba);
    quint32 crctocheck;
    quint32 basize = ba.size();
    if (basize < 17)
    {
        ERMSG("basize");
        return GENERALERROR;
    }
    memcpy(&crctocheck, &(ba.data())[8], sizeof(quint32));
    if (!S2::CheckCRC32(&(ba.data())[16], (basize - 16), crctocheck))
        return GENERALERROR;
    return EUsbHid::GetInstance()->Result;
}

int Commands::WriteFile(int filenum, QVector<S2::DataRec> *data)
{
    QByteArray ba;
    ba.resize(CN::Limits::MaxFileSize);
    S2::StoreDataMem(&(ba.data()[0]), data, filenum);
    // считываем длину файла из полученной в StoreDataMem и вычисляем количество сегментов
    quint32 wrlength = static_cast<quint8>(ba.at(7)) * 16777216; // с 4 байта начинается FileHeader.size
    wrlength += static_cast<quint8>(ba.at(6)) * 65536;
    wrlength += static_cast<quint8>(ba.at(5)) * 256;
    wrlength += static_cast<quint8>(ba.at(4));
    wrlength += sizeof(S2::FileHeader); // sizeof(FileHeader)
    ba.resize(wrlength);
    EUsbHid::GetInstance()->SendFile(CN::Write::File, BoardTypes::BT_BASE, filenum, ba);
    return EUsbHid::GetInstance()->Result;
}

int Commands::WriteHiddenBlock(char board, void *HPtr, int HPtrSize)
{
    QByteArray ba = QByteArray::fromRawData(static_cast<const char *>(HPtr), HPtrSize);
    EUsbHid::GetInstance()->SendOut(CN::Write::Hardware, board, ba);
    return EUsbHid::GetInstance()->Result;
}

int Commands::GetBac(char BacNum, void *BacPtr, int BacPtrSize)
{
    QByteArray ba;
    EUsbHid::GetInstance()->SendIn(CN::Read::BlkAC, BacNum, ba, BacPtrSize);
    memcpy(BacPtr, &(ba.data()[0]), BacPtrSize);
    return EUsbHid::GetInstance()->Result;
}

int Commands::GetBd(char BdNum, void *BdPtr, int BdPtrSize)
{
    QByteArray ba;
    EUsbHid::GetInstance()->SendIn(CN::Read::BlkData, BdNum, ba, BdPtrSize);
    memcpy(BdPtr, &(ba.data()[0]), BdPtrSize);
    return EUsbHid::GetInstance()->Result;
}

int Commands::GetBda(char board, void *BdPtr, int BdPtrSize)
{
    QByteArray ba;
    EUsbHid::GetInstance()->SendIn(CN::Read::BlkDataA, board, ba, BdPtrSize);
    memcpy(BdPtr, &(ba.data()[0]), BdPtrSize);
    return EUsbHid::GetInstance()->Result;
}

int Commands::GetBt(char BtNum, void *BtPtr, int &BtPtrSize)
{
    QByteArray ba;
    EUsbHid::GetInstance()->SendIn(CN::Read::BlkTech, BtNum, ba, BtPtrSize);
    memcpy(BtPtr, &(ba.data()[0]), BtPtrSize);
    return EUsbHid::GetInstance()->Result;
}

int Commands::WriteBac(char BacNum, void *BacPtr, int BacPtrSize)
{
    QByteArray ba = QByteArray::fromRawData(static_cast<const char *>(BacPtr), BacPtrSize);
    EUsbHid::GetInstance()->SendOut(CN::Write::BlkAC, BacNum, ba);
    return EUsbHid::GetInstance()->Result;
}

int Commands::EraseTechBlock(char block)
{
    EUsbHid::GetInstance()->SendCmd(CN::Write::EraseTech, block);
    return EUsbHid::GetInstance()->Result;
}

int Commands::WriteTimeMNK(uint32_t Time, int TimeSize)
{
    QByteArray ba = QByteArray::fromRawData(reinterpret_cast<const char *>(&Time), TimeSize);
    EUsbHid::GetInstance()->SendOut(CN::Write::Time, BoardTypes::BT_NONE, ba);
    return EUsbHid::GetInstance()->Result;
}

int Commands::GetTimeMNK(uint &Time)
{
    QByteArray ba;

    EUsbHid::GetInstance()->SendIn(CN::Read::Time, BoardTypes::BT_NONE, ba, sizeof(uint));
    memcpy(&Time, &(ba.data()[0]), sizeof(uint));
    return EUsbHid::GetInstance()->Result;
}

int Commands::WriteBd(char BdNum, void *BdPtr, int BdPtrSize)
{
    QByteArray ba = QByteArray::fromRawData(static_cast<const char *>(BdPtr), BdPtrSize);
    EUsbHid::GetInstance()->SendOut(CN::Write::BlkData, BdNum, ba);
    return EUsbHid::GetInstance()->Result;
}

int Commands::WriteCom(char ComNum)
{
    EUsbHid::GetInstance()->SendCmd(CN::Write::BlkCmd, ComNum);
    return EUsbHid::GetInstance()->Result;
}

int Commands::RunVPO()
{
    EUsbHid::GetInstance()->SendCmd(CN::Write::Upgrade);
    return EUsbHid::GetInstance()->Result;
}

int Commands::TestCom(char OnOff)
{
    EUsbHid::GetInstance()->SendCmd(CN::Test, OnOff);
    return EUsbHid::GetInstance()->Result;
}
