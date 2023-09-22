#include "interfaces/conn/baseconnection.h"

#include <QCoreApplication>
#include <QMutexLocker>
#include <QTimer>
#include <gen/stdfunc.h>
#include <interfaces/ports/baseport.h>
#include <memory>
#include <s2/s2util.h>

namespace Interface
{

// Static members
BaseConnection::InterfacePointer BaseConnection::s_connection;

BaseConnection::BaseConnection(QObject *parent) : QObject(parent) /* m_working(false),*/, ifacePort(nullptr)
{
    ProxyInit();
    qRegisterMetaType<State>();
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setInterval(MAINTIMEOUT);
    connect(m_timeoutTimer, &QTimer::timeout, this, &BaseConnection::timeout);
    m_state = State::Connect;
    m_settings = std::unique_ptr<ProtocolDescription>(new ProtocolDescription());
}

void BaseConnection::ProxyInit()
{
    proxyBS = UniquePointer<DataTypesProxy>(new DataTypesProxy());
    proxyGRS = UniquePointer<DataTypesProxy>(new DataTypesProxy());
    proxyFS = UniquePointer<DataTypesProxy>(new DataTypesProxy());
    proxyDRL = UniquePointer<DataTypesProxy>(new DataTypesProxy());
    proxyBStr = UniquePointer<DataTypesProxy>(new DataTypesProxy());

    proxyBS->RegisterType<DataTypes::BlockStruct>();
    proxyGRS->RegisterType<DataTypes::GeneralResponseStruct>();
    proxyFS->RegisterType<S2::FileStruct>();
    proxyDRL->RegisterType<QList<S2::DataItem>>();
    proxyBStr->RegisterType<DataTypes::BitStringStruct>();

#ifdef __linux__
    proxyTS = UniquePointer<DataTypesProxy>(new DataTypesProxy(&DataManager::GetInstance()));
    proxyTS->RegisterType<timespec>();
#endif
}

void BaseConnection::reqAlarms(quint32 sigAdr, quint32 sigCount)
{
    if (isValidRegs(sigAdr, sigCount))
    {
        // CommandStruct bi { C_ReqAlarms, sigAdr, sigCount };
        setToQueue(CommandStruct { Commands::C_ReqAlarms, sigAdr, sigCount });
    }
}

void BaseConnection::reqFloats(quint32 sigAdr, quint32 sigCount)
{
    if (isValidRegs(sigAdr, sigCount))
    {
        // CommandStruct bi { C_ReqFloats, sigAdr, sigCount };
        setToQueue(CommandStruct { Commands::C_ReqFloats, sigAdr, sigCount });
    }
}

void BaseConnection::reqBitStrings(quint32 sigAdr, quint32 sigCount)
{
    if (isValidRegs(sigAdr, sigCount))
    {
        // CommandStruct bi { C_ReqBitStrings, sigAdr, sigCount };
        setToQueue(CommandStruct { Commands::C_ReqBitStrings, sigAdr, sigCount });
    }
}

bool BaseConnection::supportBSIExt()
{
    m_busy = true;
    m_timeout = false;
    bool status = false;
    auto connBitString = std::shared_ptr<QMetaObject::Connection>(new QMetaObject::Connection);
    auto connError = std::shared_ptr<QMetaObject::Connection>(new QMetaObject::Connection);

    *connBitString = connect(
        proxyBStr.get(), &DataTypesProxy::DataStorable, this, [=, &busy = m_busy, &status](const QVariant &data) {
            auto bs = data.value<DataTypes::BitStringStruct>();
            if (bs.sigAdr != Regs::bsiExtStartReg)
                return;
            if (connBitString)
                QObject::disconnect(*connBitString);
            if (connError)
                QObject::disconnect(*connError);
            busy = false;
            status = true;
        });

    *connError = connect(
        proxyGRS.get(), &DataTypesProxy::DataStorable, this, [=, &busy = m_busy, &status](const QVariant &data) {
            auto resp = data.value<DataTypes::GeneralResponseStruct>();
            if (resp.type == DataTypes::Error)
            {
                if (connBitString)
                    QObject::disconnect(*connBitString);
                if (connError)
                    QObject::disconnect(*connError);
                busy = false;
                status = false;
            }
        });

    m_timeoutTimer->start();
    reqBSIExt();
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait(100);
    }
    return status;
}

void BaseConnection::reqStartup(quint32 sigAdr, quint32 sigCount)
{
    // CommandStruct bi { C_ReqStartup, sigAdr, sigCount };
    setToQueue(CommandStruct { Commands::C_ReqStartup, sigAdr, sigCount });
}

void BaseConnection::reqBSI()
{
    // CommandStruct bi { C_ReqBSI, Regs::bsiStartReg, Regs::bsiCountRegs };
    setToQueue(CommandStruct { Commands::C_ReqBSI, Regs::bsiStartReg, Regs::bsiCountRegs });
}

void BaseConnection::reqBSIExt()
{
    quint32 regCount = sizeof(Modules::StartupInfoBlockExt0) / sizeof(quint32);
    // CommandStruct bi { C_ReqBSIExt, Regs::bsiExtStartReg, regCount };
    setToQueue(CommandStruct { Commands::C_ReqBSIExt, Regs::bsiExtStartReg, regCount });
}

void BaseConnection::reqFile(quint32 id, FileFormat format, quint32 expectedSize)
{
    if (expectedSize != 0)
    {
        DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataSize, expectedSize };
        (&DataManager::GetInstance())->addSignalToOutList(resp);
    }
    // CommandStruct bi { C_ReqFile, id, format };
    setToQueue(CommandStruct { Commands::C_ReqFile, id, format });
}

void BaseConnection::writeFile(quint32 id, const QByteArray &ba)
{
    // CommandStruct bi { C_WriteFile, id, ba };
    setToQueue(CommandStruct { Commands::C_WriteFile, id, ba });
}

void BaseConnection::reqTime()
{
    // CommandStruct bi { C_ReqTime, 0, 0 };
    setToQueue(CommandStruct { Commands::C_ReqTime, 0, 0 });
}

void BaseConnection::writeTime(quint32 time)
{
    // CommandStruct bi { C_WriteTime, time, 0 };
    setToQueue(CommandStruct { Commands::C_WriteTime, time, 0 });
}

#ifdef __linux__
void BaseConnection::writeTime(const timespec &time)
{
    CommandStruct bi { C_WriteTime, QVariant::fromValue(time), QVariant() };
    setToQueue(bi);
}
#endif

void BaseConnection::writeCommand(Commands cmd, QVariant value)
{
    // CommandStruct bi { cmd, value, QVariant() };
    setToQueue(CommandStruct { cmd, value, QVariant() });
}

void Interface::BaseConnection::writeCommand(Commands cmd, const QVariantList &list)
{
    const quint16 start_addr = list.first().value<DataTypes::FloatStruct>().sigAdr;
    if (isValidRegs(start_addr, list.size()))
    {
        // CommandStruct bi { cmd, list, QVariant() };
        setToQueue(CommandStruct { cmd, list, QVariant() });
    }
}

void BaseConnection::resultReady(const QVariant &msg)
{
    auto result = msg.value<DataTypes::BlockStruct>();
    disconnect(proxyBS.get(), &DataTypesProxy::DataStorable, this, &BaseConnection::resultReady);
    m_byteArrayResult = result.data;
    m_busy = false;
}

void BaseConnection::responseReceived(const QVariant &msg)
{
    auto response = msg.value<DataTypes::GeneralResponseStruct>();
    if ((response.type == DataTypes::GeneralResponseTypes::DataSize)
        || (response.type == DataTypes::GeneralResponseTypes::DataCount))
        return;
    disconnect(proxyGRS.get(), &DataTypesProxy::DataStorable, this, &BaseConnection::responseReceived);
    m_responseResult = (response.type == DataTypes::GeneralResponseTypes::Ok);
    m_busy = false;
}

void BaseConnection::fileReceived(const QVariant &msg)
{
    auto file = msg.value<S2::FileStruct>();
    disconnect(proxyFS.get(), &DataTypesProxy::DataStorable, this, &BaseConnection::fileReceived);
    m_byteArrayResult = file.data;
    m_busy = false;
}

void BaseConnection::timeout()
{
    m_busy = false;
}

void BaseConnection::setToQueue(CommandStruct &&cmd)
{
    // DataManager::GetInstance().addToInQueue(cmd);
    m_queue.addToQueue(std::move(cmd));
    emit wakeUpParser();
}

// helper methods

bool BaseConnection::isValidRegs(const quint32 sigAdr, const quint32 sigCount, const quint32 command)
{
    const auto &st = settings();
    if (!st->dictionary().contains(sigAdr))
        return false;
    const auto val = st->dictionary().value(sigAdr);
    if (command != 0)
    {
        if (command != val.function)
            return false;
    }
    return val.count == sigCount;
}

ProtocolDescription *BaseConnection::settings()
{
    return m_settings.get();
}

State BaseConnection::state()
{
    QMutexLocker locker(&_stateMutex);
    return m_state;
}

void BaseConnection::setState(const State &state)
{
    QMutexLocker locker(&_stateMutex);
    m_state = state;
    emit stateChanged(m_state);
}

void BaseConnection::close()
{
    // DataManager::GetInstance().clearQueue();
    if (ifacePort)
        ifacePort->closeConnection();
    setState(State::Disconnect);
    // TODO: dummy solution, maybe better to get signals from working threads
    StdFunc::Wait(100);
    disconnect();
    s_connection.reset();
}

// ===============================================================================
// =============================== SYNC METHODS ==================================
// ===============================================================================

Error::Msg BaseConnection::reqBlockSync(
    quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize)
{
    m_busy = true;
    m_timeout = false;
    connect(proxyBS.get(), &DataTypesProxy::DataStorable, this, &BaseConnection::resultReady);
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

    quint32 resultsize = m_byteArrayResult.size();
    if ((m_timeout) || (resultsize < blocksize))
        return Error::Msg::Timeout;
    memcpy(block, &m_byteArrayResult.data()[0], blocksize);
    return Error::Msg::NoError;
}

Error::Msg BaseConnection::writeBlockSync(
    quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize)
{
    DataTypes::BlockStruct bs;
    bs.ID = blocknum;
    bs.data.resize(blocksize);
    memcpy(&bs.data.data()[0], block, blocksize);
    m_busy = true;
    m_timeout = false;
    connect(proxyGRS.get(), &DataTypesProxy::DataStorable, this, &BaseConnection::responseReceived);
    if (blocktype == DataTypes::DataBlockTypes::BacBlock)
    {
        writeCommand(Commands::C_WriteTuningCoef, QVariant::fromValue(bs));
        m_timeoutTimer->start();
        while (m_busy)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            StdFunc::Wait();
        }
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

Error::Msg BaseConnection::writeFileSync(S2::FilesEnum filenum, const QByteArray &ba)
{
    m_busy = true;
    m_timeout = false;
    connect(proxyGRS.get(), &DataTypesProxy::DataStorable, this, &BaseConnection::responseReceived);
    writeFile(quint32(filenum), ba);
    m_timeoutTimer->start();
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait();
    }
    if (m_timeout)
        return Error::Msg::Timeout;
    return (m_responseResult) ? Error::Msg::NoError : Error::Msg::GeneralError;
}

Error::Msg BaseConnection::readS2FileSync(S2::FilesEnum filenum)
{
    m_busy = true;
    m_timeout = false;
    auto connection = std::shared_ptr<QMetaObject::Connection>(new QMetaObject::Connection);
    *connection = connect(proxyDRL.get(), &DataTypesProxy::DataStorable, this, [=] {
        QObject::disconnect(*connection);
        m_busy = false;
    });
    reqFile(quint32(filenum), FileFormat::DefaultS2);
    m_timeoutTimer->start();
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait();
    }
    if (m_timeout)
        return Error::Msg::Timeout;

    return (m_responseResult) ? Error::Msg::NoError : Error::Msg::GeneralError;
}

Error::Msg BaseConnection::readFileSync(S2::FilesEnum filenum, QByteArray &ba)
{
    m_busy = true;
    m_timeout = false;
    connect(proxyFS.get(), &DataTypesProxy::DataStorable, this, &BaseConnection::fileReceived);
    reqFile(quint32(filenum), FileFormat::Binary);
    m_timeoutTimer->start();
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait();
    }
    if (m_timeout)
        return Error::Msg::Timeout;
    ba = m_byteArrayResult;
    return Error::Msg::NoError;
}

Error::Msg BaseConnection::reqTimeSync(void *block, quint32 blocksize)
{
    Q_ASSERT(blocksize == sizeof(timespec) || blocksize == sizeof(DataTypes::BitStringStruct));
    m_busy = true;
    m_timeout = false;
    auto connection = std::shared_ptr<QMetaObject::Connection>(new QMetaObject::Connection);
    if (blocksize == sizeof(DataTypes::BitStringStruct))
    {
        *connection = connect(proxyBStr.get(), &DataTypesProxy::DataStorable, this, [&](const QVariant &data) {
            auto bs = data.value<DataTypes::BitStringStruct>();
            QObject::disconnect(*connection);
            *static_cast<DataTypes::BitStringStruct *>(block) = bs;
            m_busy = false;
        });
    }
#ifdef Q_OS_LINUX
    else if (blocksize == sizeof(timespec))
    {
        *connection = connect(proxyTS.get(), &DataTypesProxy::DataStorable, this, [&](const QVariant &data) {
            auto ts = data.value<timespec>();
            QObject::disconnect(*connection);
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
    if (m_timeout)
        return Error::Msg::Timeout;
    memcpy(block, &m_byteArrayResult.data()[0], blocksize);
    return Error::Msg::NoError;
}

}
