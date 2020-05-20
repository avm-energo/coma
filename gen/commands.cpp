// commands.cpp
#include <QCoreApplication>
#include "commands.h"
#include "error.h"

EUsbHid *cn;

Commands::Commands()
{
    cn = 0;
}

int Commands::Connect()
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn != 0)
    {
        if (cn->Connect())
            return Error::ER_NOERROR;
    }
    return Error::ER_GENERALERROR;
#else
    return 0;
#endif
}

bool Commands::isConnected()
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn != 0)
        return cn->Connected;
    return false;
#else
    return false;
#endif
}

void Commands::Disconnect()
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn != 0)
        cn->Disconnect();
#endif
}

int Commands::GetBsi(ModuleBSI::Bsi &bsi)
{
#if PROGSIZE != PROGSIZE_EMUL
    QByteArray ba;
    if (cn != 0)
    {
        cn->SendIn(CN_GBsi, BoardTypes::BT_NONE, ba, sizeof(ModuleBSI::Bsi));
        memcpy(&bsi, &(ba.data()[0]), sizeof(ModuleBSI::Bsi));
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    return 0;
#endif
}

int Commands::GetFileWithRestore(int filenum, QVector<S2::DataRec> *data)
{
#if PROGSIZE != PROGSIZE_EMUL
    QByteArray ba;
    if (cn != 0)
    {
//        cn->Send(CN_GF, BoardTypes::BT_NONE, tmp, 0, filenum, data);
        cn->SendFile(CN_GF, BoardTypes::BT_NONE, filenum, ba);
        // проверка контрольной суммы файла
        quint32 crctocheck;
        quint32 basize = ba.size();
        if (basize < 17)
            return Error::ER_GENERALERROR;
        memcpy(&crctocheck, &(ba.data())[8], sizeof(quint32));
        if (!S2::CheckCRC32(&(ba.data())[16], (basize-16), crctocheck))
            return Error::ER_GENERALERROR;
        return S2::RestoreDataMem(&ba.data()[0], basize, data);
//        return cn->result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(filenum);
    Q_UNUSED(data);
    return 0;
#endif
}

int Commands::GetFile(int filenum, QByteArray &ba)
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn != 0)
    {
//        cn->Send(CN_GF, BoardTypes::BT_NONE, ptr, 0, filenum);
        cn->SendFile(CN_GF, BoardTypes::BT_NONE, filenum, ba);
        quint32 crctocheck;
        quint32 basize = ba.size();
        if (basize < 17)
            return Error::ER_GENERALERROR;
        memcpy(&crctocheck, &(ba.data())[8], sizeof(quint32));
        if (!S2::CheckCRC32(&(ba.data())[16], (basize-16), crctocheck))
            return Error::ER_GENERALERROR;
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(filenum)
    Q_UNUSED(ptr)
    return 0;
#endif
}

int Commands::WriteFile(int filenum, QVector<S2::DataRec> *data)
{
#if PROGSIZE != PROGSIZE_EMUL
    QByteArray ba;

    if (cn != 0)
    {
        ba.resize(CN_MAXFILESIZE);
        S2::StoreDataMem(&(ba.data()[0]), data, filenum);
        // считываем длину файла из полученной в StoreDataMem и вычисляем количество сегментов
        quint32 wrlength = static_cast<quint8>(ba.at(7))*16777216; // с 4 байта начинается FileHeader.size
        wrlength += static_cast<quint8>(ba.at(6))*65536;
        wrlength += static_cast<quint8>(ba.at(5))*256;
        wrlength += static_cast<quint8>(ba.at(4));
        wrlength += sizeof(S2::FileHeader); // sizeof(FileHeader)
        ba.resize(wrlength);
        cn->SendFile(CN_WF, BoardTypes::BT_BASE, filenum, ba);
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(ptr);
    Q_UNUSED(filenum);
    Q_UNUSED(data);
    return 0;
#endif
}

int Commands::WriteHiddenBlock(char board, void *HPtr, int HPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    QByteArray ba;

    if (cn != 0)
    {
        ba.append(static_cast<const char *>(HPtr), HPtrSize);
        cn->SendOut(CN_WHv, board, ba);
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(board);
    Q_UNUSED(HPtr);
    Q_UNUSED(HPtrSize);
    return 0;
#endif
}

int Commands::GetBac(char BacNum, void *BacPtr, int BacPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    QByteArray ba;
    if (cn != 0)
    {
        cn->SendIn(CN_GBac, BacNum, ba, BacPtrSize);
        memcpy(BacPtr, &(ba.data()[0]), BacPtrSize);
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(BacNum);
    Q_UNUSED(BacPtr);
    Q_UNUSED(BacPtrSize);
    return 0;
#endif
}

int Commands::GetBd(char BdNum, void *BdPtr, int BdPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    QByteArray ba;
    if (cn != 0)
    {
        cn->SendIn(CN_GBd, BdNum, ba, BdPtrSize);
        memcpy(BdPtr, &(ba.data()[0]), BdPtrSize);
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(BdNum);
    Q_UNUSED(BdPtr);
    Q_UNUSED(BdPtrSize);
    return 0;
#endif
}

int Commands::GetBda(char board, void *BdPtr, int BdPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    QByteArray ba;
    if (cn != 0)
    {
        cn->SendIn(CN_GBda, board, ba, BdPtrSize);
        memcpy(BdPtr, &(ba.data()[0]), BdPtrSize);
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(board);
    Q_UNUSED(BdPtr);
    Q_UNUSED(BdPtrSize);
    return 0;
#endif
}

int Commands::GetBt(char BtNum, void *BtPtr, int &BtPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    QByteArray ba;
    if (cn != 0)
    {
        cn->SendIn(CN_GBt, BtNum, ba, BtPtrSize);
        memcpy(BtPtr, &(ba.data()[0]), BtPtrSize);
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(BtNum);
    Q_UNUSED(BtPtr);
    Q_UNUSED(BtPtrSize);
    return 0;
#endif
}

int Commands::WriteBac(char BacNum, void *BacPtr, int BacPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    QByteArray ba;
    if (cn != 0)
    {
        ba.append(static_cast<const char *>(BacPtr), BacPtrSize);
        cn->SendOut(CN_WBac, BacNum, ba);
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(BacNum);
    Q_UNUSED(BacPtr);
    Q_UNUSED(BacPtrSize);
    return 0;
#endif
}

int Commands::EraseTechBlock(char block)
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn != 0)
    {
        cn->SendCmd(CN_Ert, block);
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(block);
    return 0;
#endif
}

int Commands::WriteTimeMNK(uint32_t *Time,  int TimeSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    QByteArray ba;
    if (cn != 0)
    {
        ba.append(reinterpret_cast<const char *>(Time), TimeSize);
        cn->SendOut(CN_WTime, BoardTypes::BT_NONE, ba);
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(Time);
    return 0;
#endif
}

int Commands::GetTimeMNK(uint &Time)
{
#if PROGSIZE != PROGSIZE_EMUL
    QByteArray ba;
    if (cn != 0)
    {
        cn->SendIn(CN_GTime, BoardTypes::BT_NONE, ba, sizeof(uint));
        memcpy(&Time, &(ba.data()[0]), sizeof(uint));
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(Time);
    return 0;
#endif
}

int Commands::WriteBd(char BdNum, void *BdPtr, int BdPtrSize)
{
#if PROGSIZE != PROGSIZE_EMUL
    QByteArray ba;
    if (cn != 0)
    {
        ba.append(static_cast<const char *>(BdPtr), BdPtrSize);
        cn->SendOut(CN_WBd, BdNum, ba);
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(BdNum);
    Q_UNUSED(BdPtr);
    Q_UNUSED(BdPtrSize);
    return 0;
#endif
}

int Commands::WriteCom(char ComNum)
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn != 0)
    {
        cn->SendCmd(CN_WCom, ComNum);
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    return 0;
#endif
}

int Commands::RunVPO()
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn != 0)
    {
        cn->SendCmd(CN_VPO);
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    return 0;
#endif
}

int Commands::TestCom(char OnOff)
{
#if PROGSIZE != PROGSIZE_EMUL
    if (cn != 0)
    {
        cn->SendCmd(CN_STest, OnOff);
        return cn->Result;
    }
    return Error::ER_GENERALERROR;
#else
    Q_UNUSED(OnOff);
    return 0;
#endif
}
