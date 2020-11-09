// commands.cpp
#include "commands.h"

#include "../gen/error.h"
#include "../gen/timefunc.h"

#include <QCoreApplication>

Commands::Commands()
{
}

Error::Msg Commands::Connect()
{

    if (ProtocomThread::GetInstance().Connect())
        return Error::Msg::NoError;
    else
        return Error::Msg::GeneralError;
}

void Commands::Disconnect()
{
    ProtocomThread::GetInstance().Disconnect();
}

Error::Msg Commands::GetBsi(ModuleBSI::Bsi &bsi)
{
    QByteArray ba;
    ProtocomThread::GetInstance().SendIn(CN::Read::BlkStartInfo, BoardTypes::BT_NONE, ba, sizeof(ModuleBSI::Bsi));
    // cn->SendIn(CN::Read::BlkStartInfo, BoardTypes::BT_NONE, ba, sizeof(ModuleBSI::Bsi));
    memcpy(&bsi, &(ba.data()[0]), sizeof(ModuleBSI::Bsi));
    return ProtocomThread::GetInstance().result();
}

Error::Msg Commands::GetFileWithRestore(int filenum, S2ConfigType *data)
{
    QByteArray ba;
    ProtocomThread::GetInstance().SendFile(CN::Read::File, BoardTypes::BT_NONE, filenum, ba);
    // проверка контрольной суммы файла
    quint32 crctocheck;
    quint32 basize = ba.size();
    if (basize < 17)
    {
        ERMSG("Ошибка basize");
        return Error::Msg::GeneralError;
    }

    memcpy(&crctocheck, &(ba.data())[8], sizeof(quint32));
    if (!S2::CheckCRC32(&(ba.data())[16], (basize - 16), crctocheck))
        return Error::Msg::GeneralError;
    return S2::RestoreDataMem(&ba.data()[0], basize, data);
}

Error::Msg Commands::GetFile(int filenum, QByteArray &ba)
{
    ProtocomThread::GetInstance().SendFile(CN::Read::File, BoardTypes::BT_NONE, filenum, ba);
    quint32 crctocheck;
    quint32 basize = ba.size();
    if (basize < 17)
    {
        ERMSG("basize");
        return Error::Msg::GeneralError;
    }
    memcpy(&crctocheck, &(ba.data())[8], sizeof(quint32));
    if (!S2::CheckCRC32(&(ba.data())[16], (basize - 16), crctocheck))
        return Error::Msg::GeneralError;
    return ProtocomThread::GetInstance().result();
}

Error::Msg Commands::WriteFile(int filenum, S2ConfigType *data)
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
    ProtocomThread::GetInstance().SendFile(CN::Write::File, BoardTypes::BT_BASE, filenum, ba);
    return ProtocomThread::GetInstance().result();
}

Error::Msg Commands::WriteHiddenBlock(char board, void *HPtr, int HPtrSize)
{
    QByteArray ba = QByteArray::fromRawData(static_cast<const char *>(HPtr), HPtrSize);
    ProtocomThread::GetInstance().SendOut(CN::Write::Hardware, board, ba);
    return ProtocomThread::GetInstance().result();
}

Error::Msg Commands::GetBac(char BacNum, void *BacPtr, int BacPtrSize)
{
    QByteArray ba;
    ProtocomThread::GetInstance().SendIn(CN::Read::BlkAC, BacNum, ba, BacPtrSize);
    memcpy(BacPtr, &(ba.data()[0]), BacPtrSize);
    return ProtocomThread::GetInstance().result();
}

Error::Msg Commands::GetBd(char BdNum, void *BdPtr, int BdPtrSize)
{
    QByteArray ba;
    ProtocomThread::GetInstance().SendIn(CN::Read::BlkData, BdNum, ba, BdPtrSize);
    memcpy(BdPtr, &(ba.data()[0]), BdPtrSize);
    return ProtocomThread::GetInstance().result();
}

Error::Msg Commands::GetBda(char board, void *BdPtr, int BdPtrSize)
{
    QByteArray ba;
    ProtocomThread::GetInstance().SendIn(CN::Read::BlkDataA, board, ba, BdPtrSize);
    memcpy(BdPtr, &(ba.data()[0]), BdPtrSize);
    return ProtocomThread::GetInstance().result();
}

Error::Msg Commands::GetBt(char BtNum, void *BtPtr, int &BtPtrSize)
{
    QByteArray ba;
    ProtocomThread::GetInstance().SendIn(CN::Read::BlkTech, BtNum, ba, BtPtrSize);
    memcpy(BtPtr, &(ba.data()[0]), BtPtrSize);
    return ProtocomThread::GetInstance().result();
}

Error::Msg Commands::WriteBac(char BacNum, void *BacPtr, int BacPtrSize)
{
    QByteArray ba = QByteArray::fromRawData(static_cast<const char *>(BacPtr), BacPtrSize);
    ProtocomThread::GetInstance().SendOut(CN::Write::BlkAC, BacNum, ba);
    return ProtocomThread::GetInstance().result();
}

Error::Msg Commands::EraseTechBlock(char block)
{
    ProtocomThread::GetInstance().SendCmd(CN::Write::EraseTech, block);
    return ProtocomThread::GetInstance().result();
}

Error::Msg Commands::WriteTimeMNK(uint32_t Time, int TimeSize)
{
    QByteArray ba = QByteArray::fromRawData(reinterpret_cast<const char *>(&Time), TimeSize);
    ProtocomThread::GetInstance().SendOut(CN::Write::Time, BoardTypes::BT_NONE, ba);
    return ProtocomThread::GetInstance().result();
}

Error::Msg Commands::GetTimeMNK(uint &Time)
{
    QByteArray ba;

    ProtocomThread::GetInstance().SendIn(CN::Read::Time, BoardTypes::BT_NONE, ba, sizeof(uint));
    memcpy(&Time, &(ba.data()[0]), sizeof(uint));
    return ProtocomThread::GetInstance().result();
}

Error::Msg Commands::WriteBd(char BdNum, void *BdPtr, int BdPtrSize)
{
    QByteArray ba = QByteArray::fromRawData(static_cast<const char *>(BdPtr), BdPtrSize);
    ProtocomThread::GetInstance().SendOut(CN::Write::BlkData, BdNum, ba);
    return ProtocomThread::GetInstance().result();
}

Error::Msg Commands::WriteCom(char ComNum)
{
    ProtocomThread::GetInstance().SendCmd(CN::Write::BlkCmd, ComNum);
    return ProtocomThread::GetInstance().result();
}

Error::Msg Commands::RunVPO()
{
    ProtocomThread::GetInstance().SendCmd(CN::Write::Upgrade);
    return ProtocomThread::GetInstance().result();
}

// Не нашел использования
Error::Msg Commands::TestCom(char OnOff)
{
    ProtocomThread::GetInstance().SendCmd(CN::Test, OnOff);
    return ProtocomThread::GetInstance().result();
}

Error::Msg Commands::SetMode(int mode)
{
    ProtocomThread::GetInstance().SendCmd(CN::Write::Mode, mode);
    return ProtocomThread::GetInstance().result();
}

int Commands::GetMode()
{
    QByteArray ba;
    ProtocomThread::GetInstance().SendIn(CN::Read::Mode, 0, ba, 0);
    if (ProtocomThread::GetInstance().result() != Error::Msg::GeneralError)
        return ba.at(0);
    else
        return -1;
}
