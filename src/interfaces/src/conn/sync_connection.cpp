#include "interfaces/conn/sync_connection.h"

#include <QCoreApplication>
#include <QDebug>
#include <gen/std_ext.h>
#include <gen/stdfunc.h>
#include <interfaces/conn/async_connection.h>

namespace Interface
{

SyncConnection::SyncConnection(AsyncConnection *connection) noexcept
    : QObject(connection), m_connection(connection), m_timeoutTimer(new QTimer(this))
{
    m_timeoutTimer->setInterval(MAINTIMEOUT);
    connect(m_timeoutTimer, &QTimer::timeout, this, &SyncConnection::timeout);
}

void SyncConnection::eventloop() noexcept
{
    m_timeoutTimer->start();
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait();
    }
}

void SyncConnection::reset() noexcept
{
    m_busy = true;
    m_timeout = false;
}

void SyncConnection::resultReady(const DataTypes::BlockStruct &result)
{
    m_byteArrayResult = result.data;
    m_busy = false;
}

void SyncConnection::responseReceived(const DataTypes::GeneralResponseStruct &response)
{
    if ((response.type == DataTypes::GeneralResponseTypes::DataSize)
        || (response.type == DataTypes::GeneralResponseTypes::DataCount))
        return;
    m_responseResult = (response.type == DataTypes::GeneralResponseTypes::Ok);
    m_busy = false;
}

void SyncConnection::fileReceived(const S2::FileStruct &file)
{
    m_byteArrayResult = file.data;
    m_busy = false;
}

void SyncConnection::timeout()
{
    m_busy = false;
    m_timeout = true;
}

Error::Msg SyncConnection::reqBSI()
{
    reset();
    int count = 0;
    auto conn = m_connection->connection(this, [&, &busy = m_busy](const DataTypes::BitStringStruct &bs) {
        if (bs.sigAdr >= addr::bsiStartReg && bs.sigAdr < addr::bsiStartReg + addr::bsiCountRegs)
            ++count;
        if (count == addr::bsiCountRegs)
            busy = false;
    });
    m_connection->reqBSI();
    eventloop();
    QObject::disconnect(conn);
    if (m_timeout)
        return Error::Msg::Timeout;
    else
        return Error::Msg::NoError;
}

bool SyncConnection::supportBSIExt()
{
    reset();
    bool status = false;
    auto connBitString = m_connection->connection(this, [&, &busy = m_busy](const DataTypes::BitStringStruct &bs) {
        if (bs.sigAdr != addr::bsiExtStartReg)
            return;
        busy = false;
        status = true;
    });

    auto connError = m_connection->connection(this, [&, &busy = m_busy](const DataTypes::GeneralResponseStruct &resp) {
        if (resp.type == DataTypes::Error)
        {
            busy = false;
            status = false;
        }
    });

    m_connection->reqBSIExt();
    eventloop();
    QObject::disconnect(connBitString);
    QObject::disconnect(connError);
    return status;
}

Error::Msg SyncConnection::reqBlockSync(
    quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize)
{
    reset();
    auto conn = m_connection->connection(this, &SyncConnection::resultReady);
    QMap<DataTypes::DataBlockTypes, Commands> blockmap;
    blockmap[DataTypes::DataBlockTypes::BacBlock] = Commands::C_ReqTuningCoef;
    blockmap[DataTypes::DataBlockTypes::BdaBlock] = Commands::C_ReqBlkDataA;
    blockmap[DataTypes::DataBlockTypes::BdBlock] = Commands::C_ReqBlkData;

    Q_ASSERT(blockmap.contains(blocktype));
    m_connection->writeCommand(blockmap.value(blocktype), blocknum);
    eventloop();
    QObject::disconnect(conn);

    quint32 resultsize = m_byteArrayResult.size();
    if ((m_timeout) || (resultsize < blocksize))
        return Error::Msg::Timeout;
    memcpy(block, &m_byteArrayResult.data()[0], blocksize);
    return Error::Msg::NoError;
}

Error::Msg SyncConnection::writeBlockSync(
    quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize)
{
    DataTypes::BlockStruct bs;
    bs.ID = blocknum;
    bs.data.resize(blocksize);
    memcpy(&bs.data.data()[0], block, blocksize);
    reset();
    if (blocktype == DataTypes::DataBlockTypes::BacBlock)
    {
        auto conn = m_connection->connection(this, &SyncConnection::responseReceived);
        m_connection->writeCommand(Commands::C_WriteTuningCoef, QVariant::fromValue(bs));
        eventloop();
        QObject::disconnect(conn);
        if (m_timeout)
            return Error::Msg::Timeout;
        return (m_responseResult) ? Error::Msg::NoError : Error::Msg::GeneralError;
    }
    else
    {
        WARNMSG("Некорректный номер блока");
        return Error::Msg::GeneralError;
    }
}

Error::Msg SyncConnection::writeFileSync(S2::FilesEnum filenum, const QByteArray &ba)
{
    reset();
    auto conn = m_connection->connection(this, &SyncConnection::responseReceived);
    m_connection->writeFile(quint32(filenum), ba);
    eventloop();
    QObject::disconnect(conn);
    if (m_timeout)
        return Error::Msg::Timeout;
    return (m_responseResult) ? Error::Msg::NoError : Error::Msg::GeneralError;
}

Error::Msg SyncConnection::writeConfigurationSync(const QByteArray &ba)
{
    return writeFileSync(S2::FilesEnum::Config, ba);
}

Error::Msg SyncConnection::readS2FileSync(S2::FilesEnum filenum)
{
    reset();
    auto conn = m_connection->connection(this, [=](const QList<S2::DataItem> &) { m_busy = false; });
    m_connection->reqFile(quint32(filenum), DataTypes::FileFormat::DefaultS2);
    eventloop();
    QObject::disconnect(conn);
    if (m_timeout)
        return Error::Msg::Timeout;

    return (m_responseResult) ? Error::Msg::NoError : Error::Msg::GeneralError;
}

Error::Msg SyncConnection::readFileSync(S2::FilesEnum filenum, QByteArray &ba)
{
    reset();
    auto conn = m_connection->connection(this, &SyncConnection::fileReceived);
    m_connection->reqFile(quint32(filenum), DataTypes::FileFormat::Binary);
    eventloop();
    QObject::disconnect(conn);
    if (m_timeout)
        return Error::Msg::Timeout;
    ba = m_byteArrayResult;
    return Error::Msg::NoError;
}

Error::Msg SyncConnection::reqTimeSync(void *block, quint32 blocksize)
{
    Q_ASSERT(blocksize == sizeof(timespec) || blocksize == sizeof(DataTypes::BitStringStruct));
    reset();
    QMetaObject::Connection conn;
    if (blocksize == sizeof(DataTypes::BitStringStruct))
    {
        conn = m_connection->connection(this, [&](const DataTypes::BitStringStruct &bs) {
            *static_cast<DataTypes::BitStringStruct *>(block) = bs;
            m_busy = false;
        });
    }
#ifdef Q_OS_LINUX
    else if (blocksize == sizeof(timespec))
    {
        conn = m_connection->connection(this, [&](const timespec &ts) {
            *static_cast<timespec *>(block) = ts;
            m_busy = false;
        });
    }
#endif
    m_connection->reqTime();
    eventloop();
    QObject::disconnect(conn);
    if (m_timeout)
        return Error::Msg::Timeout;
    memcpy(block, &m_byteArrayResult.data()[0], blocksize);
    return Error::Msg::NoError;
}

} // namespace Interface
