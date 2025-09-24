#include "interfaces/conn/sync_connection.h"

#include "device/bsi.h"
#include <gen/std_ext.h>
#include <gen/stdfunc.h>
#include <interfaces/conn/async_connection.h>

#include <QCoreApplication>
#include <QDebug>

namespace Interface
{

SyncConnection::SyncConnection(AsyncConnection *connection) noexcept : QObject(connection), m_connection(connection)
{
    connect(m_connection, &AsyncConnection::responseError, this, &SyncConnection::responseError);
}

void SyncConnection::eventLoop() noexcept
{
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait();
    }
}

void SyncConnection::reset() noexcept
{
    m_busy = true;
}

void SyncConnection::resultReady(const DataTypes::BlockStruct &result)
{
    m_byteArrayResult = result.data;
    m_busy = false;
}

void SyncConnection::responseReceived(const DataTypes::GeneralResponseStruct &response)
{
    if (response.type == DataTypes::GeneralResponseTypes::DataSize)
    {
        emit setRange(response.data);
        return;
    }
    if (response.type == DataTypes::GeneralResponseTypes::DataCount)
    {
        emit setValue(response.data);
        return;
    }
    m_responseResult
        = (response.type == DataTypes::GeneralResponseTypes::Ok) ? Error::Msg::NoError : Error::Msg::GeneralError;
    m_busy = false;
}

void SyncConnection::fileReceived(const S2::FileStruct &file)
{
    m_byteArrayResult = file.data;
    m_busy = false;
}

void SyncConnection::s2bfileReceived(const S2::S2BFile &file)
{
    m_s2bFile = file;
    m_busy = false;
    m_responseResult = Error::Msg::NoError;
}

void SyncConnection::timeout()
{
    m_busy = false;
    m_responseResult = Error::Msg::Timeout;
}

void SyncConnection::responseError(Error::Msg msg)
{
    m_responseResult = msg;
    m_busy = false;
}

Error::Msg SyncConnection::reqBSI()
{
    reset();
    int count = 0;
    auto conn = m_connection->connection(this,
        [&, &busy = m_busy](const DataTypes::BitStringStruct &bs)
        {
            if (bs.sigAdr >= Device::bsiStartReg && bs.sigAdr < Device::bsiStartReg + Device::bsiCountRegs)
                ++count;
            if (count == Device::bsiCountRegs)
                busy = false;
        });
    m_connection->reqBSI();
    eventLoop();
    QObject::disconnect(conn);
    return m_responseResult;
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
    eventLoop();
    QObject::disconnect(conn);

    quint32 resultsize = m_byteArrayResult.size();
    if (resultsize < blocksize)
        return Error::Msg::SizeError;
    else if (m_responseResult == Error::Msg::Timeout)
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
        eventLoop();
        QObject::disconnect(conn);
        if (m_responseResult == Error::Msg::GeneralError)
            return Error::Msg::WriteError;
        else
            return m_responseResult;
    }
    else
    {
        WARNMSG("Некорректный номер блока");
        return Error::Msg::WriteError;
    }
}

Error::Msg SyncConnection::writeFileSync(S2::FilesEnum filenum, const QByteArray &ba)
{
    reset();
    auto conn = m_connection->connection(this, &SyncConnection::responseReceived);
    m_connection->writeFile(quint32(filenum), ba);
    eventLoop();
    QObject::disconnect(conn);
    if (m_responseResult == Error::Msg::GeneralError)
        return Error::Msg::FileWriteError;
    else
        return m_responseResult;
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
    eventLoop();
    QObject::disconnect(conn);
    if (m_responseResult == Error::Msg::GeneralError)
        return Error::Msg::ReadError;
    else
        return m_responseResult;
}

Error::Msg SyncConnection::readFileSync(S2::FilesEnum filenum, QByteArray &ba)
{
    reset();
    auto conn = m_connection->connection(this, &SyncConnection::fileReceived);
    m_connection->reqFile(quint32(filenum), DataTypes::FileFormat::Binary);
    eventLoop();
    QObject::disconnect(conn);
    if (m_responseResult != Error::Msg::NoError)
        return Error::Msg::ReadError;
    ba = m_byteArrayResult;
    return Error::Msg::NoError;
}

Error::Msg SyncConnection::readS2BFileSync(S2::FilesEnum filenum, S2::S2BFile &file)
{
    reset();
    auto conn = m_connection->connection(this, &SyncConnection::s2bfileReceived);
    auto conn2 = m_connection->connection(this, &SyncConnection::responseReceived);
    m_connection->reqFile(quint32(filenum), DataTypes::FileFormat::Binary);
    eventLoop();
    QObject::disconnect(conn);
    QObject::disconnect(conn2);
    if (m_responseResult != Error::Msg::NoError)
        return m_responseResult;
    file = m_s2bFile;
    return m_responseResult;
}

Error::Msg SyncConnection::reqTimeSync(void *block, quint32 blocksize)
{
    Q_ASSERT(blocksize == sizeof(timespec) || blocksize == sizeof(DataTypes::BitStringStruct));
    reset();
    QMetaObject::Connection conn;
    if (blocksize == sizeof(DataTypes::BitStringStruct))
    {
        conn = m_connection->connection(this,
            [&](const DataTypes::BitStringStruct &bs)
            {
                *static_cast<DataTypes::BitStringStruct *>(block) = bs;
                m_busy = false;
            });
    }
#ifdef Q_OS_LINUX
    else if (blocksize == sizeof(timespec))
    {
        conn = m_connection->connection(this,
            [&](const timespec &ts)
            {
                *static_cast<timespec *>(block) = ts;
                m_busy = false;
            });
    }
#endif
    m_connection->reqTime();
    eventLoop();
    QObject::disconnect(conn);
    if (m_responseResult != Error::Msg::NoError)
        return m_responseResult;
    memcpy(block, &m_byteArrayResult.data()[0], blocksize);
    return Error::Msg::NoError;
}

} // namespace Interface
