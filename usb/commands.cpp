// commands.cpp
#include "commands.h"

#include "../gen/error.h"
#include "../gen/timefunc.h"

#include <QCoreApplication>

Commands::Commands()
{
    cn = nullptr;
}

int Commands::Connect()
{
    if (cn != nullptr)
    {
        if (cn->Connect())
            return NOERROR;
    }
    return GENERALERROR;
}

bool Commands::isConnected()
{
    if (cn != nullptr)
        return cn->Connected;
    return false;
}

void Commands::Disconnect()
{
    if (cn != nullptr)
        cn->Disconnect();
}

int Commands::GetBsi(ModuleBSI::Bsi &bsi)
{
    QByteArray ba;
    if (cn != nullptr)
    {
        cn->SendIn(CN::Read::BlkStartInfo, BoardTypes::BT_NONE, ba, sizeof(ModuleBSI::Bsi));
        memcpy(&bsi, &(ba.data()[0]), sizeof(ModuleBSI::Bsi));
        return cn->Result;
    }
    return GENERALERROR;
}

int Commands::GetFileWithRestore(int filenum, QVector<S2::DataRec> *data)
{
    QByteArray ba;
    if (cn != nullptr)
    {
        cn->SendFile(CN::Read::File, BoardTypes::BT_NONE, filenum, ba);
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
    return GENERALERROR;
}

int Commands::GetFile(int filenum, QByteArray &ba)
{
    if (cn != 0)
    {
        cn->SendFile(CN::Read::File, BoardTypes::BT_NONE, filenum, ba);
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
        return cn->Result;
    }
    return GENERALERROR;
}

int Commands::WriteFile(int filenum, QVector<S2::DataRec> *data)
{
    QByteArray ba;

    if (cn != 0)
    {
        ba.resize(CN::Limits::MaxFileSize);
        S2::StoreDataMem(&(ba.data()[0]), data, filenum);
        // считываем длину файла из полученной в StoreDataMem и вычисляем количество сегментов
        quint32 wrlength = static_cast<quint8>(ba.at(7)) * 16777216; // с 4 байта начинается FileHeader.size
        wrlength += static_cast<quint8>(ba.at(6)) * 65536;
        wrlength += static_cast<quint8>(ba.at(5)) * 256;
        wrlength += static_cast<quint8>(ba.at(4));
        wrlength += sizeof(S2::FileHeader); // sizeof(FileHeader)
        ba.resize(wrlength);
        cn->SendFile(CN::Write::File, BoardTypes::BT_BASE, filenum, ba);
        return cn->Result;
    }
    return GENERALERROR;
}

int Commands::WriteHiddenBlock(char board, void *HPtr, int HPtrSize)
{
    QByteArray ba;

    if (cn != 0)
    {
        ba.append(static_cast<const char *>(HPtr), HPtrSize);
        cn->SendOut(CN::Write::Hardware, board, ba);
        return cn->Result;
    }
    return GENERALERROR;
}

int Commands::GetBac(char BacNum, void *BacPtr, int BacPtrSize)
{
    QByteArray ba;
    if (cn != 0)
    {
        cn->SendIn(CN::Read::BlkAC, BacNum, ba, BacPtrSize);
        memcpy(BacPtr, &(ba.data()[0]), BacPtrSize);
        return cn->Result;
    }
    return GENERALERROR;
}

int Commands::GetBd(char BdNum, void *BdPtr, int BdPtrSize)
{
    QByteArray ba;
    if (cn != 0)
    {
        cn->SendIn(CN::Read::BlkData, BdNum, ba, BdPtrSize);
        memcpy(BdPtr, &(ba.data()[0]), BdPtrSize);
        return cn->Result;
    }
    return GENERALERROR;
}

int Commands::GetBda(char board, void *BdPtr, int BdPtrSize)
{
    QByteArray ba;
    if (cn != 0)
    {
        cn->SendIn(CN::Read::BlkDataA, board, ba, BdPtrSize);
        memcpy(BdPtr, &(ba.data()[0]), BdPtrSize);
        return cn->Result;
    }
    return GENERALERROR;
}

int Commands::GetBt(char BtNum, void *BtPtr, int &BtPtrSize)
{
    QByteArray ba;
    if (cn != 0)
    {
        cn->SendIn(CN::Read::BlkTech, BtNum, ba, BtPtrSize);
        memcpy(BtPtr, &(ba.data()[0]), BtPtrSize);
        return cn->Result;
    }
    return GENERALERROR;
}

int Commands::WriteBac(char BacNum, void *BacPtr, int BacPtrSize)
{
    QByteArray ba;
    if (cn != 0)
    {
        ba.append(static_cast<const char *>(BacPtr), BacPtrSize);
        cn->SendOut(CN::Write::BlkAC, BacNum, ba);
        return cn->Result;
    }
    return GENERALERROR;
}

int Commands::EraseTechBlock(char block)
{
    if (cn != nullptr)
    {
        cn->SendCmd(CN::Write::EraseTech, block);
        return cn->Result;
    }
    return GENERALERROR;
}

int Commands::WriteTimeMNK(uint32_t Time, int TimeSize)
{
    QByteArray ba;
    if (cn != nullptr)
    {
        ba.append(reinterpret_cast<const char *>(&Time), TimeSize);
        cn->SendOut(CN::Write::Time, BoardTypes::BT_NONE, ba);
        return cn->Result;
    }
    return GENERALERROR;
}

int Commands::GetTimeMNK(uint &Time)
{
    QByteArray ba;
    if (cn != nullptr)
    {
        cn->SendIn(CN::Read::Time, BoardTypes::BT_NONE, ba, sizeof(uint));
        memcpy(&Time, &(ba.data()[0]), sizeof(uint));
        return cn->Result;
    }
    return GENERALERROR;
}

int Commands::WriteBd(char BdNum, void *BdPtr, int BdPtrSize)
{
    QByteArray ba;
    if (cn != 0)
    {
        ba.append(static_cast<const char *>(BdPtr), BdPtrSize);
        cn->SendOut(CN::Write::BlkData, BdNum, ba);
        return cn->Result;
    }
    return GENERALERROR;
}

int Commands::WriteCom(char ComNum)
{
    if (cn != nullptr)
    {
        cn->SendCmd(CN::Write::BlkCmd, ComNum);
        return cn->Result;
    }
    return GENERALERROR;
}

int Commands::RunVPO()
{
    if (cn != nullptr)
    {
        cn->SendCmd(CN::Write::Upgrade);
        return cn->Result;
    }
    return GENERALERROR;
}

int Commands::TestCom(char OnOff)
{
    if (cn != nullptr)
    {
        cn->SendCmd(CN::Test, OnOff);
        return cn->Result;
    }
    return GENERALERROR;
}
