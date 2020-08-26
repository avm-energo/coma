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

    EProtocom::GetInstance()->Connect();
    return NOERROR;
}

bool Commands::isConnected()
{
    return EProtocom::GetInstance()->isConnected();
}

void Commands::Disconnect()
{
    EProtocom::GetInstance()->Disconnect();
}

int Commands::GetBsi(ModuleBSI::Bsi &bsi)
{
    QByteArray ba;
    EProtocom::GetInstance()->SendIn(CN::Read::BlkStartInfo, BoardTypes::BT_NONE, ba, sizeof(ModuleBSI::Bsi));
    // cn->SendIn(CN::Read::BlkStartInfo, BoardTypes::BT_NONE, ba, sizeof(ModuleBSI::Bsi));
    memcpy(&bsi, &(ba.data()[0]), sizeof(ModuleBSI::Bsi));
    return EProtocom::GetInstance()->result();
}

int Commands::GetFileWithRestore(int filenum, QVector<S2::DataRec> *data)
{
    QByteArray ba;
    EProtocom::GetInstance()->SendFile(CN::Read::File, BoardTypes::BT_NONE, filenum, ba);
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
    EProtocom::GetInstance()->SendFile(CN::Read::File, BoardTypes::BT_NONE, filenum, ba);
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
    return EProtocom::GetInstance()->result();
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
    EProtocom::GetInstance()->SendFile(CN::Write::File, BoardTypes::BT_BASE, filenum, ba);
    return EProtocom::GetInstance()->result();
}

int Commands::WriteHiddenBlock(char board, void *HPtr, int HPtrSize)
{
    QByteArray ba = QByteArray::fromRawData(static_cast<const char *>(HPtr), HPtrSize);
    EProtocom::GetInstance()->SendOut(CN::Write::Hardware, board, ba);
    return EProtocom::GetInstance()->result();
}

int Commands::GetBac(char BacNum, void *BacPtr, int BacPtrSize)
{
    QByteArray ba;
    EProtocom::GetInstance()->SendIn(CN::Read::BlkAC, BacNum, ba, BacPtrSize);
    memcpy(BacPtr, &(ba.data()[0]), BacPtrSize);
    return EProtocom::GetInstance()->result();
}

int Commands::GetBd(char BdNum, void *BdPtr, int BdPtrSize)
{
    QByteArray ba;
    EProtocom::GetInstance()->SendIn(CN::Read::BlkData, BdNum, ba, BdPtrSize);
    memcpy(BdPtr, &(ba.data()[0]), BdPtrSize);
    return EProtocom::GetInstance()->result();
}

int Commands::GetBda(char board, void *BdPtr, int BdPtrSize)
{
    QByteArray ba;
    EProtocom::GetInstance()->SendIn(CN::Read::BlkDataA, board, ba, BdPtrSize);
    memcpy(BdPtr, &(ba.data()[0]), BdPtrSize);
    return EProtocom::GetInstance()->result();
}

int Commands::GetBt(char BtNum, void *BtPtr, int &BtPtrSize)
{
    QByteArray ba;
    EProtocom::GetInstance()->SendIn(CN::Read::BlkTech, BtNum, ba, BtPtrSize);
    memcpy(BtPtr, &(ba.data()[0]), BtPtrSize);
    return EProtocom::GetInstance()->result();
}

int Commands::WriteBac(char BacNum, void *BacPtr, int BacPtrSize)
{
    QByteArray ba = QByteArray::fromRawData(static_cast<const char *>(BacPtr), BacPtrSize);
    EProtocom::GetInstance()->SendOut(CN::Write::BlkAC, BacNum, ba);
    return EProtocom::GetInstance()->result();
}

int Commands::EraseTechBlock(char block)
{
    EProtocom::GetInstance()->SendCmd(CN::Write::EraseTech, block);
    return EProtocom::GetInstance()->result();
}

int Commands::WriteTimeMNK(uint32_t Time, int TimeSize)
{
    QByteArray ba = QByteArray::fromRawData(reinterpret_cast<const char *>(&Time), TimeSize);
    EProtocom::GetInstance()->SendOut(CN::Write::Time, BoardTypes::BT_NONE, ba);
    return EProtocom::GetInstance()->result();
}

int Commands::GetTimeMNK(uint &Time)
{
    QByteArray ba;

    EProtocom::GetInstance()->SendIn(CN::Read::Time, BoardTypes::BT_NONE, ba, sizeof(uint));
    memcpy(&Time, &(ba.data()[0]), sizeof(uint));
    return EProtocom::GetInstance()->result();
}

int Commands::WriteBd(char BdNum, void *BdPtr, int BdPtrSize)
{
    QByteArray ba = QByteArray::fromRawData(static_cast<const char *>(BdPtr), BdPtrSize);
    EProtocom::GetInstance()->SendOut(CN::Write::BlkData, BdNum, ba);
    return EProtocom::GetInstance()->result();
}

int Commands::WriteCom(char ComNum)
{
    EProtocom::GetInstance()->SendCmd(CN::Write::BlkCmd, ComNum);
    return EProtocom::GetInstance()->result();
}

int Commands::RunVPO()
{
    EProtocom::GetInstance()->SendCmd(CN::Write::Upgrade);
    return EProtocom::GetInstance()->result();
}

int Commands::TestCom(char OnOff)
{
    EProtocom::GetInstance()->SendCmd(CN::Test, OnOff);
    return EProtocom::GetInstance()->result();
}