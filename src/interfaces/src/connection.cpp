#include "interfaces/connection.h"

/// TODO: этого инклуда здесь быть не должно...
#include "../../module/modules.h"

#include <QCoreApplication>
#include <QMutexLocker>
#include <gen/std_ext.h>
#include <interfaces/ifaces/baseinterface.h>
#include <s2/s2util.h>

namespace Interface
{

// Static members
Connection::InterfacePointer Connection::s_connection;

Connection::Connection(QObject *parent) : QObject(parent), m_timeoutTimer(new QTimer(this))
{
    qRegisterMetaType<State>();
    qRegisterMetaType<DeviceResponse>();
    m_timeoutTimer->setInterval(MAINTIMEOUT);
    connect(m_timeoutTimer, &QTimer::timeout, this, &Connection::timeout);
}

Connection *Connection::iface() noexcept
{
    return s_connection.get();
}

void Connection::update(InterfacePointer iface) noexcept
{
    s_connection = std::move(iface);
}

RequestQueue &Connection::getQueue() noexcept
{
    return m_queue;
}

void Connection::reqAlarms(quint32 sigAdr, quint32 sigCount)
{
    setToQueue(CommandStruct { Commands::C_ReqAlarms, sigAdr, sigCount });
}

void Connection::reqFloats(quint32 sigAdr, quint32 sigCount)
{
    setToQueue(CommandStruct { Commands::C_ReqFloats, sigAdr, sigCount });
}

void Connection::reqBitStrings(quint32 sigAdr, quint32 sigCount)
{
    if (sigAdr == Regs::bsiStartReg)
        reqBSI();
    else if (sigAdr == Regs::bsiExtStartReg)
        reqBSIExt();
    else
        setToQueue(CommandStruct { Commands::C_ReqBitStrings, sigAdr, sigCount });
}

bool Connection::supportBSIExt()
{
    m_busy = true;
    m_timeout = false;
    bool status = false;
    auto connBitString = connection(this, [&, &busy = m_busy](const DataTypes::BitStringStruct &bs) {
        if (bs.sigAdr != Regs::bsiExtStartReg)
            return;
        busy = false;
        status = true;
    });

    auto connError = connection(this, [&, &busy = m_busy](const DataTypes::GeneralResponseStruct &resp) {
        if (resp.type == DataTypes::Error)
        {
            busy = false;
            status = false;
        }
    });

    m_timeoutTimer->start();
    reqBSIExt();
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait();
    }
    QObject::disconnect(connBitString);
    QObject::disconnect(connError);
    return status;
}

void Connection::reqStartup(quint32 sigAdr, quint32 sigCount)
{
    setToQueue(CommandStruct { Commands::C_ReqStartup, sigAdr, sigCount });
}

void Connection::reqBSI()
{
    setToQueue(CommandStruct { Commands::C_ReqBSI, Regs::bsiStartReg, Regs::bsiCountRegs });
}

void Connection::reqBSIExt()
{
    quint32 regCount = sizeof(Modules::StartupInfoBlockExt0) / sizeof(quint32);
    setToQueue(CommandStruct { Commands::C_ReqBSIExt, Regs::bsiExtStartReg, regCount });
}

void Connection::reqFile(quint32 id, FileFormat format, quint32 expectedSize)
{
    if (expectedSize != 0)
    {
        DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataSize, expectedSize };
        emit response(resp);
    }
    setToQueue(CommandStruct { Commands::C_ReqFile, id, format });
}

void Connection::writeFile(quint32 id, const QByteArray &ba)
{
    setToQueue(CommandStruct { Commands::C_WriteFile, id, ba });
}

void Connection::writeConfiguration(const QByteArray &ba)
{
    constexpr auto fileType = std_ext::to_underlying(S2::FilesEnum::Config);
    writeFile(fileType, ba);
    emit silentReconnectMode();
}

void Connection::writeFirmware(const QByteArray &ba)
{
    constexpr auto fileType = std_ext::to_underlying(S2::FilesEnum::Firmware);
    writeFile(fileType, ba);
}

void Connection::reqTime()
{
    setToQueue(CommandStruct { Commands::C_ReqTime, 0, 0 });
}

void Connection::writeTime(quint32 time)
{
    setToQueue(CommandStruct { Commands::C_WriteTime, time, 0 });
}

#ifdef Q_OS_LINUX
void Connection::writeTime(const timespec &time)
{
    setToQueue(CommandStruct { Commands::C_WriteTime, QVariant::fromValue(time), QVariant() });
}
#endif

void Connection::writeCommand(Commands cmd, QVariant value)
{
    setToQueue(CommandStruct { cmd, value, QVariant() });
    if (cmd == Commands::C_StartFirmwareUpgrade)
        emit silentReconnectMode();
}

void Interface::Connection::writeCommand(Commands cmd, const QVariantList &list)
{
    // const quint16 start_addr = list.first().value<DataTypes::FloatStruct>().sigAdr;
    // if (isValidRegs(start_addr, list.size()))
    setToQueue(CommandStruct { cmd, list, QVariant() });
}

void Interface::Connection::responseHandle(const Interface::DeviceResponse &resp)
{
    std::visit([this](auto &&var) { emit response(var); }, resp);
}

void Connection::resultReady(const DataTypes::BlockStruct &result)
{
    m_byteArrayResult = result.data;
    m_busy = false;
}

void Connection::responseReceived(const DataTypes::GeneralResponseStruct &response)
{
    if ((response.type == DataTypes::GeneralResponseTypes::DataSize)
        || (response.type == DataTypes::GeneralResponseTypes::DataCount))
        return;
    m_responseResult = (response.type == DataTypes::GeneralResponseTypes::Ok);
    m_busy = false;
}

void Connection::fileReceived(const S2::FileStruct &file)
{
    m_byteArrayResult = file.data;
    m_busy = false;
}

void Connection::timeout()
{
    m_busy = false;
    m_timeout = true;
}

void Connection::setToQueue(CommandStruct &&cmd)
{
    m_queue.addToQueue(std::move(cmd));
}

// ===============================================================================
// =============================== SYNC METHODS ==================================
// ===============================================================================

Error::Msg Connection::reqBlockSync(
    quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize)
{
    m_busy = true;
    m_timeout = false;
    auto conn = connection(this, &Connection::resultReady);
    QMap<DataTypes::DataBlockTypes, Commands> blockmap;
    blockmap[DataTypes::DataBlockTypes::BacBlock] = Commands::C_ReqTuningCoef;
    blockmap[DataTypes::DataBlockTypes::BdaBlock] = Commands::C_ReqBlkDataA;
    blockmap[DataTypes::DataBlockTypes::BdBlock] = Commands::C_ReqBlkData;

    Q_ASSERT(blockmap.contains(blocktype));
    writeCommand(blockmap.value(blocktype), blocknum);

    m_timeoutTimer->start();
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    disconnect(conn);

    quint32 resultsize = m_byteArrayResult.size();
    if ((m_timeout) || (resultsize < blocksize))
        return Error::Msg::Timeout;
    memcpy(block, &m_byteArrayResult.data()[0], blocksize);
    return Error::Msg::NoError;
}

Error::Msg Connection::writeBlockSync(
    quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize)
{
    DataTypes::BlockStruct bs;
    bs.ID = blocknum;
    bs.data.resize(blocksize);
    memcpy(&bs.data.data()[0], block, blocksize);
    m_busy = true;
    m_timeout = false;
    auto conn = connection(this, &Connection::responseReceived);
    if (blocktype == DataTypes::DataBlockTypes::BacBlock)
    {
        writeCommand(Commands::C_WriteTuningCoef, QVariant::fromValue(bs));
        m_timeoutTimer->start();
        while (m_busy)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            StdFunc::Wait();
        }
        disconnect(conn);
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

Error::Msg Connection::writeFileSync(S2::FilesEnum filenum, const QByteArray &ba)
{
    m_busy = true;
    m_timeout = false;
    auto conn = connection(this, &Connection::responseReceived);
    writeFile(quint32(filenum), ba);
    m_timeoutTimer->start();
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait();
    }
    disconnect(conn);
    if (m_timeout)
        return Error::Msg::Timeout;
    return (m_responseResult) ? Error::Msg::NoError : Error::Msg::GeneralError;
}

Error::Msg Connection::readS2FileSync(S2::FilesEnum filenum)
{
    m_busy = true;
    m_timeout = false;
    auto conn = connection(this, [=](const QList<S2::DataItem> &) { m_busy = false; });
    reqFile(quint32(filenum), FileFormat::DefaultS2);
    m_timeoutTimer->start();
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait();
    }
    disconnect(conn);
    if (m_timeout)
        return Error::Msg::Timeout;

    return (m_responseResult) ? Error::Msg::NoError : Error::Msg::GeneralError;
}

Error::Msg Connection::readFileSync(S2::FilesEnum filenum, QByteArray &ba)
{
    m_busy = true;
    m_timeout = false;
    auto conn = connection(this, &Connection::fileReceived);
    reqFile(quint32(filenum), FileFormat::Binary);
    m_timeoutTimer->start();
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait();
    }
    disconnect(conn);
    if (m_timeout)
        return Error::Msg::Timeout;
    ba = m_byteArrayResult;
    return Error::Msg::NoError;
}

Error::Msg Connection::reqTimeSync(void *block, quint32 blocksize)
{
    Q_ASSERT(blocksize == sizeof(timespec) || blocksize == sizeof(DataTypes::BitStringStruct));
    m_busy = true;
    m_timeout = false;
    QMetaObject::Connection conn;
    if (blocksize == sizeof(DataTypes::BitStringStruct))
    {
        conn = connection(this, [&](const DataTypes::BitStringStruct &bs) {
            *static_cast<DataTypes::BitStringStruct *>(block) = bs;
            m_busy = false;
        });
    }
#ifdef Q_OS_LINUX
    else if (blocksize == sizeof(timespec))
    {
        conn = connection(this, [&](const timespec &ts) {
            *static_cast<timespec *>(block) = ts;
            m_busy = false;
        });
    }
#endif
    m_timeoutTimer->start();
    reqTime();
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait();
    }
    disconnect(conn);
    if (m_timeout)
        return Error::Msg::Timeout;
    memcpy(block, &m_byteArrayResult.data()[0], blocksize);
    return Error::Msg::NoError;
}

}
