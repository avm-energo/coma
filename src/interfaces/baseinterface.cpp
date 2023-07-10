#include "baseinterface.h"

#include "../s2/s2.h"
#include "baseport.h"

#include <QCoreApplication>
#include <QMutexLocker>
#include <QTimer>
#include <gen/stdfunc.h>
#include <memory>

namespace Interface
{

// Static members
BaseInterface::InterfacePointer BaseInterface::m_iface;

BaseInterface::BaseInterface(QObject *parent) : QObject(parent) /* m_working(false),*/, ifacePort(nullptr)
{
    ProxyInit();
    qRegisterMetaType<State>();
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setInterval(MAINTIMEOUT);
    connect(m_timeoutTimer, &QTimer::timeout, this, &BaseInterface::timeout);
    m_state = State::Connect;
    m_settings = std::unique_ptr<ProtocolDescription>(new ProtocolDescription());
}

void BaseInterface::ProxyInit()
{
    proxyBS = UniquePointer<DataTypesProxy>(new DataTypesProxy());
    proxyGRS = UniquePointer<DataTypesProxy>(new DataTypesProxy());
    proxyFS = UniquePointer<DataTypesProxy>(new DataTypesProxy());
    proxyDRL = UniquePointer<DataTypesProxy>(new DataTypesProxy());
    proxyBStr = UniquePointer<DataTypesProxy>(new DataTypesProxy());

    proxyBS->RegisterType<DataTypes::BlockStruct>();
    proxyGRS->RegisterType<DataTypes::GeneralResponseStruct>();
    proxyFS->RegisterType<S2DataTypes::FileStruct>();
    proxyDRL->RegisterType<QList<S2DataTypes::DataRecV>>();
    proxyBStr->RegisterType<DataTypes::BitStringStruct>();

#ifdef __linux__
    proxyTS = UniquePointer<DataTypesProxy>(new DataTypesProxy(&DataManager::GetInstance()));
    proxyTS->RegisterType<timespec>();
#endif
}

void BaseInterface::reqAlarms(quint32 sigAdr, quint32 sigCount)
{
    if (isValidRegs(sigAdr, sigCount))
    {
        CommandStruct bi { C_ReqAlarms, sigAdr, sigCount };
        setToQueue(bi);
    }
}

void BaseInterface::reqFloats(quint32 sigAdr, quint32 sigCount)
{
    if (isValidRegs(sigAdr, sigCount))
    {
        CommandStruct bi { C_ReqFloats, sigAdr, sigCount };
        setToQueue(bi);
    }
}

void BaseInterface::reqBitStrings(quint32 sigAdr, quint32 sigCount)
{
    if (isValidRegs(sigAdr, sigCount))
    {
        CommandStruct bi { C_ReqBitStrings, sigAdr, sigCount };
        setToQueue(bi);
    }
}

bool BaseInterface::supportBSIExt()
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

void BaseInterface::reqStartup(quint32 sigAdr, quint32 sigCount)
{
    CommandStruct bi { C_ReqStartup, sigAdr, sigCount };
    setToQueue(bi);
}

void BaseInterface::reqBSI()
{
    CommandStruct bi { C_ReqBSI, Regs::bsiStartReg, Regs::bsiCountRegs };
    setToQueue(bi);
}

void BaseInterface::reqBSIExt()
{
    quint32 regCount = sizeof(Modules::StartupInfoBlockExt0) / sizeof(quint32);
    CommandStruct bi { C_ReqBSIExt, Regs::bsiExtStartReg, regCount };
    setToQueue(bi);
}

void BaseInterface::reqFile(quint32 id, FileFormat format, quint32 expectedSize)
{
    if (expectedSize != 0)
    {
        DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataSize, expectedSize };
        (&DataManager::GetInstance())->addSignalToOutList(resp);
    }
    CommandStruct bi { C_ReqFile, id, format };
    setToQueue(bi);
}

void BaseInterface::writeFile(quint32 id, const QByteArray &ba)
{
    CommandStruct bi { C_WriteFile, id, ba };
    setToQueue(bi);
}

void BaseInterface::writeS2File(S2DataTypes::FilesEnum number, const S2DataTypes::S2ConfigType &file)
{
    QByteArray ba;
    S2Util::StoreDataMem(ba, file, int(number));
    writeFile(quint32(number), ba);
}

void BaseInterface::reqTime()
{
    CommandStruct bi { C_ReqTime, 0, 0 };
    setToQueue(bi);
}

void BaseInterface::writeTime(quint32 time)
{
    CommandStruct bi { C_WriteTime, time, 0 };
    setToQueue(bi);
}

void BaseInterface::writeCommand(Commands cmd, QVariant value)
{
    CommandStruct bi { cmd, value, QVariant() };
    setToQueue(bi);
}

void Interface::BaseInterface::writeCommand(Commands cmd, const QVariantList &list)
{
    const quint16 start_addr = list.first().value<DataTypes::FloatStruct>().sigAdr;
    if (isValidRegs(start_addr, list.size()))
    {
        CommandStruct bi { cmd, list, QVariant() };
        setToQueue(bi);
    }
}

void BaseInterface::resultReady(const QVariant &msg)
{
    auto result = msg.value<DataTypes::BlockStruct>();
    disconnect(proxyBS.get(), &DataTypesProxy::DataStorable, this, &BaseInterface::resultReady);
    m_byteArrayResult = result.data;
    m_busy = false;
}

void BaseInterface::responseReceived(const QVariant &msg)
{
    auto response = msg.value<DataTypes::GeneralResponseStruct>();
    if ((response.type == DataTypes::GeneralResponseTypes::DataSize)
        || (response.type == DataTypes::GeneralResponseTypes::DataCount))
        return;
    disconnect(proxyGRS.get(), &DataTypesProxy::DataStorable, this, &BaseInterface::responseReceived);
    m_responseResult = (response.type == DataTypes::GeneralResponseTypes::Ok);
    m_busy = false;
}

void BaseInterface::fileReceived(const QVariant &msg)
{
    auto file = msg.value<S2DataTypes::FileStruct>();
    disconnect(proxyFS.get(), &DataTypesProxy::DataStorable, this, &BaseInterface::fileReceived);
    m_byteArrayResult = file.data;
    m_busy = false;
}

void BaseInterface::timeout()
{
    m_busy = false;
}

void BaseInterface::setToQueue(CommandStruct &cmd)
{
    DataManager::GetInstance().addToInQueue(cmd);
    emit wakeUpParser();
}

// helper methods

bool BaseInterface::isValidRegs(const quint32 sigAdr, const quint32 sigCount, const quint32 command)
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

ProtocolDescription *BaseInterface::settings()
{
    return m_settings.get();
}

State BaseInterface::state()
{
    QMutexLocker locker(&_stateMutex);
    return m_state;
}

void BaseInterface::setState(const State &state)
{
    QMutexLocker locker(&_stateMutex);
    m_state = state;
    emit stateChanged(m_state);
}

void BaseInterface::close()
{
    DataManager::GetInstance().clearQueue();
    if (ifacePort)
        ifacePort->closeConnection();
}

// ===============================================================================
// =============================== SYNC METHODS ==================================
// ===============================================================================

Error::Msg BaseInterface::reqBlockSync(
    quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize)
{
    m_busy = true;
    m_timeout = false;
    connect(proxyBS.get(), &DataTypesProxy::DataStorable, this, &BaseInterface::resultReady);
    QMap<DataTypes::DataBlockTypes, Commands> blockmap;
    blockmap[DataTypes::DataBlockTypes::BacBlock] = C_ReqTuningCoef;
    blockmap[DataTypes::DataBlockTypes::BdaBlock] = C_ReqBlkDataA;
    blockmap[DataTypes::DataBlockTypes::BdBlock] = C_ReqBlkData;

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

Error::Msg BaseInterface::writeBlockSync(
    quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize)
{
    DataTypes::BlockStruct bs;
    bs.ID = blocknum;
    bs.data.resize(blocksize);
    memcpy(&bs.data.data()[0], block, blocksize);
    m_busy = true;
    m_timeout = false;
    connect(proxyGRS.get(), &DataTypesProxy::DataStorable, this, &BaseInterface::responseReceived);
    if (blocktype == DataTypes::DataBlockTypes::BacBlock)
    {
        writeCommand(C_WriteTuningCoef, QVariant::fromValue(bs));
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

Error::Msg BaseInterface::writeConfFileSync(const QList<S2DataTypes::DataRecV> &config)
{
    S2DataTypes::S2ConfigType buffer;

    std::transform(config.begin(), config.end(), std::back_inserter(buffer),
        [](const auto &record) -> S2DataTypes::DataRec { return record.serialize(); });
    S2Util::tester(buffer);

    buffer.push_back({ { S2DataTypes::dummyElement, 0 }, nullptr });
    return writeS2FileSync(S2DataTypes::FilesEnum::Config, buffer);
}

Error::Msg BaseInterface::pushAndWriteConfFileSync(ConfigV *config, const QList<S2DataTypes::DataRecV> recordList)
{
    config->pushConfig();
    for (auto record : recordList)
        config->setRecordValue(record.getId(), record.getData());
    return writeConfFileSync(config->getConfig());
}

Error::Msg BaseInterface::popAndWriteConfFileSync(ConfigV *config)
{
    config->popConfig();
    return writeConfFileSync(config->getConfig());
}

Error::Msg BaseInterface::writeFileSync(S2DataTypes::FilesEnum filenum, QByteArray &ba)
{
    m_busy = true;
    m_timeout = false;
    connect(proxyGRS.get(), &DataTypesProxy::DataStorable, this, &BaseInterface::responseReceived);
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

Error::Msg BaseInterface::writeS2FileSync(S2DataTypes::FilesEnum number, const S2DataTypes::S2ConfigType &file)
{
    QByteArray ba;
    S2Util::StoreDataMem(ba, file, int(number));

    // с 4 байта начинается FileHeader.size
    quint32 length = *reinterpret_cast<quint32 *>(&ba.data()[4]);
    length += sizeof(S2DataTypes::S2FileHeader);
    Q_ASSERT(length == quint32(ba.size()));
    return writeFileSync(S2DataTypes::FilesEnum::Config, ba);
}

Error::Msg BaseInterface::readS2FileSync(S2DataTypes::FilesEnum filenum)
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

Error::Msg BaseInterface::readFileSync(S2DataTypes::FilesEnum filenum, QByteArray &ba)
{
    m_busy = true;
    m_timeout = false;
    connect(proxyFS.get(), &DataTypesProxy::DataStorable, this, &BaseInterface::fileReceived);
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

Error::Msg BaseInterface::reqTimeSync(void *block, quint32 blocksize)
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
