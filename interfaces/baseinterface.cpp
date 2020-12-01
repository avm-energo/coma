#include "baseinterface.h"

#include "../gen/datamanager.h"
#include "../gen/files.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"

#include <QCoreApplication>
#include <QTimer>

BaseInterface::BaseInterface(QObject *parent) : QObject(parent), m_working(false), Log(new LogClass(this))
{
}

void BaseInterface::reqAlarms(quint32 sigAdr, quint32 sigCount)
{
    // NOTE Избежать сужающих кастов
    DataTypes::Signal signal { static_cast<quint16>(sigAdr), static_cast<quint16>(sigCount) };
    writeCommand(Queries::QC_ReqAlarms, QVariant::fromValue(signal));
}

Error::Msg BaseInterface::reqBlockSync(
    quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize)
{
    m_busy = true;
    m_timeout = false;
    QTimer *timer = new QTimer;
    timer->setInterval(MAINTIMEOUT);
    connect(timer, &QTimer::timeout, this, &BaseInterface::timeout);
    connect(&DataManager::GetInstance(), &DataManager::blockReceived, this, &BaseInterface::resultReady);
    QMap<DataTypes::DataBlockTypes, Queries::Commands> blockmap;
    blockmap[DataTypes::DataBlockTypes::BacBlock] = Queries::QUSB_ReqTuningCoef;
    blockmap[DataTypes::DataBlockTypes::BdaBlock] = Queries::QUSB_ReqBlkDataA;
    //    blockmap[DataBlockTypes::BdBlock] = Queries::;
    //    blockmap[DataBlockTypes::BciBlock] = Queries::;
    writeCommand(blockmap[blocktype], blocknum);
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait();
    }
    quint32 resultsize = m_byteArrayResult.size();
    if ((m_timeout) || (resultsize < blocksize))
        return Error::Msg::GeneralError;
    memcpy(block, &m_byteArrayResult.data()[0], blocksize);
    return Error::Msg::NoError;
}

Error::Msg BaseInterface::writeBlockSync(
    quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize)
{
    DataTypes::BlockStruct bs;
    bs.ID = blocknum;
    bs.data.resize(blocksize);
    memcpy(&bs.data.data()[0], block, blocksize);
    m_busy = true;
    m_timeout = false;
    QTimer *timer = new QTimer;
    timer->setInterval(MAINTIMEOUT);
    connect(timer, &QTimer::timeout, this, &BaseInterface::timeout);
    connect(&DataManager::GetInstance(), &DataManager::responseReceived, this, &BaseInterface::responseReceived);
    if (blocktype == DataTypes::DataBlockTypes::BacBlock)
    {
        writeCommand(Queries::QUSB_WriteTuningCoef, QVariant::fromValue(bs));
        while (m_busy)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            StdFunc::Wait();
        }
        if (m_timeout)
            return Error::Msg::GeneralError;
        return (m_responseResult) ? Error::Msg::NoError : Error::Msg::GeneralError;
    }
    else
    {
        WARNMSG("Некорректный номер блока");
        return Error::Msg::GeneralError;
    }
}

Error::Msg BaseInterface::writeConfFileSync()
{
    QByteArray ba;
    ba.resize(30000);
    S2::StoreDataMem(&ba.data()[0], S2::config, Files::Config);
    // считываем длину файла из полученной в StoreDataMem и вычисляем количество сегментов
    quint32 wrlength = static_cast<quint8>(ba.at(7)) * 16777216; // с 4 байта начинается FileHeader.size
    wrlength += static_cast<quint8>(ba.at(6)) * 65536;
    wrlength += static_cast<quint8>(ba.at(5)) * 256;
    wrlength += static_cast<quint8>(ba.at(4));
    wrlength += sizeof(S2DataTypes::FileHeader); // sizeof(FileHeader)
    ba.resize(wrlength);
    return writeFileSync(Files::Config, ba);
}

Error::Msg BaseInterface::writeFileSync(int filenum, QByteArray &ba)
{
    m_busy = true;
    m_timeout = false;
    QTimer *timer = new QTimer;
    timer->setInterval(MAINTIMEOUT);
    connect(timer, &QTimer::timeout, this, &BaseInterface::timeout);
    connect(&DataManager::GetInstance(), &DataManager::responseReceived, this, &BaseInterface::responseReceived);
    writeFile(filenum, ba);
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait();
    }
    if (m_timeout)
        return Error::Msg::GeneralError;
    return (m_responseResult) ? Error::Msg::NoError : Error::Msg::GeneralError;
}

Error::Msg BaseInterface::readS2FileSync(quint32 filenum)
{
    m_busy = true;
    m_timeout = false;
    QTimer *timer = new QTimer;
    timer->setInterval(MAINTIMEOUT);
    connect(timer, &QTimer::timeout, this, &BaseInterface::timeout);
    connect(&DataManager::GetInstance(), &DataManager::confParametersListReceived, this,
        &BaseInterface::confParameterBlockReceived);
    reqFile(filenum, true);
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait();
    }
    if (m_timeout)
        return Error::Msg::GeneralError;

    return (m_responseResult) ? Error::Msg::NoError : Error::Msg::GeneralError;
}

Error::Msg BaseInterface::readFileSync(quint32 filenum, QByteArray &ba)
{
    m_busy = true;
    m_timeout = false;
    QTimer *timer = new QTimer;
    timer->setInterval(MAINTIMEOUT);
    connect(timer, &QTimer::timeout, this, &BaseInterface::timeout);
    connect(&DataManager::GetInstance(), &DataManager::fileReceived, this, &BaseInterface::fileReceived);
    reqFile(filenum, true);
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait();
    }
    if (m_timeout)
        return Error::Msg::GeneralError;
    ba = m_byteArrayResult;
    return Error::Msg::NoError;
}

void BaseInterface::resultReady(const DataTypes::BlockStruct &result)
{
    disconnect(&DataManager::GetInstance(), &DataManager::blockReceived, this, &BaseInterface::resultReady);
    m_byteArrayResult = result.data;
    m_busy = false;
}

void BaseInterface::responseReceived(const DataTypes::GeneralResponseStruct &response)
{
    disconnect(&DataManager::GetInstance(), &DataManager::responseReceived, this, &BaseInterface::responseReceived);
    m_responseResult = (response.type == DataTypes::GeneralResponseTypes::Ok);
    m_busy = false;
}

void BaseInterface::confParameterBlockReceived(const DataTypes::ConfParametersListStruct &cfpl)
{
    disconnect(&DataManager::GetInstance(), &DataManager::confParametersListReceived, this,
        &BaseInterface::confParameterBlockReceived);
    m_responseResult = (response.type == DataTypes::GeneralResponseTypes::Ok);
    m_busy = false;
}

void BaseInterface::fileReceived(const DataTypes::FileStruct &file)
{
    disconnect(&DataManager::GetInstance(), &DataManager::fileReceived, this, &BaseInterface::fileReceived);
    m_byteArrayResult = file.filedata;
    m_busy = false;
}

void BaseInterface::timeout()
{
    QTimer *tmr = qobject_cast<QTimer *>(sender());
    tmr->deleteLater();
    m_busy = false;
}
