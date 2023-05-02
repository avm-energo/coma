#include "baseinterface.h"

#include "../s2/s2.h"

#include <QCoreApplication>
#include <QMutexLocker>
#include <gen/registers.h>
#include <gen/stdfunc.h>
#include <memory>

// Static members
BaseInterface::InterfacePointer BaseInterface::m_iface;

BaseInterface::BaseInterface(QObject *parent) : QObject(parent), /* m_working(false),*/ Log(new LogClass(this))
{
    ProxyInit();
    qRegisterMetaType<BaseInterface::State>();
    timeoutTimer = new QTimer(this);
    timeoutTimer->setInterval(MAINTIMEOUT);
    connect(timeoutTimer, &QTimer::timeout, this, &BaseInterface::timeout);
    m_state = State::None;
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
    proxyFS->RegisterType<DataTypes::FileStruct>();
    proxyDRL->RegisterType<QList<DataTypes::DataRecV>>();
    proxyBStr->RegisterType<DataTypes::BitStringStruct>();

#ifdef __linux__
    proxyTS = UniquePointer<DataTypesProxy>(new DataTypesProxy(&DataManager::GetInstance()));
    proxyTS->RegisterType<timespec>();
#endif
}

BaseInterface::~BaseInterface()
{
}

BaseInterface *BaseInterface::iface()
{
    return m_iface.get();
}

void BaseInterface::setIface(InterfacePointer iface)
{
    m_iface = std::move(iface);
}

void BaseInterface::pause()
{
    setState(State::Wait);
}

void BaseInterface::resume()
{
    // Only for case Wait to Run
    Q_ASSERT(state() == State::Wait);
    setState(State::Run);
}

void BaseInterface::reqAlarms(quint32 sigAdr, quint32 sigCount)
{
    // NOTE Избежать сужающих кастов
    DataTypes::Signal signal { static_cast<quint16>(sigAdr), static_cast<quint16>(sigCount) };
    writeCommand(Queries::QC_ReqAlarms, QVariant::fromValue(signal));
}

void BaseInterface::reqFloats(quint32 sigAdr, quint32 sigCount)
{
    BI_CommandStruct bi { C_ReqFloats, sigAdr, sigCount };
    DataManager::GetInstance().addToInQueue(bi);
}

void BaseInterface::reqBitStrings(quint32 sigAdr, quint32 sigCount)
{
    BI_CommandStruct bi { C_ReqBitStrings, sigAdr, sigCount };
    DataManager::GetInstance().addToInQueue(bi);
}

Error::Msg BaseInterface::reqBlockSync(
    quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize)
{
    m_busy = true;
    m_timeout = false;
    connect(proxyBS.get(), &DataTypesProxy::DataStorable, this, &BaseInterface::resultReady);
    QMap<DataTypes::DataBlockTypes, Queries::Commands> blockmap;
    blockmap[DataTypes::DataBlockTypes::BacBlock] = Queries::QUSB_ReqTuningCoef;
    blockmap[DataTypes::DataBlockTypes::BdaBlock] = Queries::QUSB_ReqBlkDataA;
    blockmap[DataTypes::DataBlockTypes::BdBlock] = Queries::QUSB_ReqBlkData;

    Q_ASSERT(blockmap.contains(blocktype));
    writeCommand(blockmap.value(blocktype), blocknum);

    timeoutTimer->start();
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        // StdFunc::Wait();
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
        writeCommand(Queries::QUSB_WriteTuningCoef, QVariant::fromValue(bs));
        timeoutTimer->start();
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

Error::Msg BaseInterface::writeConfFileSync(const QList<DataTypes::DataRecV> &config)
{
    S2DataTypes::S2ConfigType buffer;

    std::transform(config.begin(), config.end(), std::back_inserter(buffer),
        [](const auto &record) -> S2DataTypes::DataRec { return record.serialize(); });
    S2::tester(buffer);

    buffer.push_back({ { S2DataTypes::dummyElement, 0 }, nullptr });
    return writeS2FileSync(DataTypes::Config, &buffer);
}

Error::Msg BaseInterface::pushAndWriteConfFileSync(ConfigV *config, const QList<DataTypes::DataRecV> recordList)
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

Error::Msg BaseInterface::writeFileSync(int filenum, QByteArray &ba)
{
    m_busy = true;
    m_timeout = false;
    connect(proxyGRS.get(), &DataTypesProxy::DataStorable, this, &BaseInterface::responseReceived);
    writeFile(filenum, ba);
    timeoutTimer->start();
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait();
    }
    if (m_timeout)
        return Error::Msg::Timeout;
    return (m_responseResult) ? Error::Msg::NoError : Error::Msg::GeneralError;
}

Error::Msg BaseInterface::writeS2FileSync(DataTypes::FilesEnum number, S2DataTypes::S2ConfigType *file)
{
    QByteArray ba;
    S2::StoreDataMem(ba, *file, number);

    // с 4 байта начинается FileHeader.size
    quint32 length = *reinterpret_cast<quint32 *>(&ba.data()[4]);
    length += sizeof(S2DataTypes::FileHeader);
    Q_ASSERT(length == quint32(ba.size()));
    return writeFileSync(DataTypes::Config, ba);
}

Error::Msg BaseInterface::readS2FileSync(quint32 filenum)
{
    m_busy = true;
    m_timeout = false;
    auto connection = std::shared_ptr<QMetaObject::Connection>(new QMetaObject::Connection);
    *connection = connect(proxyDRL.get(), &DataTypesProxy::DataStorable, this, [=] {
        QObject::disconnect(*connection);
        m_busy = false;
    });
    reqFile(filenum, FileFormat::DefaultS2);
    timeoutTimer->start();
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait();
    }
    if (m_timeout)
        return Error::Msg::Timeout;

    return (m_responseResult) ? Error::Msg::NoError : Error::Msg::GeneralError;
}

Error::Msg BaseInterface::readFileSync(quint32 filenum, QByteArray &ba)
{
    m_busy = true;
    m_timeout = false;
    connect(proxyFS.get(), &DataTypesProxy::DataStorable, this, &BaseInterface::fileReceived);
    reqFile(filenum, FileFormat::Binary);
    timeoutTimer->start();
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
    timeoutTimer->start();
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

InterfaceSettings BaseInterface::settings() const
{
    return m_settings;
}

void BaseInterface::setSettings(const InterfaceSettings &settings)
{
    m_settings = settings;
}

BaseInterface::State BaseInterface::state()
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

    timeoutTimer->start();
    reqBSIExt();
    while (m_busy)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        StdFunc::Wait(100);
    }
    return status;
}

void BaseInterface::stop()
{
    Log->info("Stop()");
    setState(BaseInterface::State::Stop);
    qInfo() << metaObject()->className() << "disconnected";
}

void BaseInterface::reqStartup(quint32 sigAdr, quint32 sigCount)
{
    BI_CommandStruct bi { C_ReqStartup, sigAdr, sigCount };
    DataManager::GetInstance().addToInQueue(bi);
}

void BaseInterface::reqBSI()
{
    BI_CommandStruct bi { C_ReqBSI, 0, 0 };
    DataManager::GetInstance().addToInQueue(bi);
}

void BaseInterface::reqBSIExt()
{
    BI_CommandStruct bi { C_ReqBSIExt, 0, 0 };
    DataManager::GetInstance().addToInQueue(bi);
}

void BaseInterface::reqFile(quint32 id, FileFormat format, quint32 expectedSize)
{
    if (expectedSize != 0)
    {
        DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataSize, expectedSize };
        (&DataManager::GetInstance())->addSignalToOutList(resp);
    }
    BI_CommandStruct bi { C_ReqFile, id, format };
    DataManager::GetInstance().addToInQueue(bi);
}

void BaseInterface::writeFile(quint32 id, const QByteArray &ba)
{
    BI_CommandStruct bi { C_WriteFile, id, ba };
    DataManager::GetInstance().addToInQueue(bi);
}

void BaseInterface::writeS2File(DataTypes::FilesEnum number, S2DataTypes::S2ConfigType *file)
{
    QByteArray ba;
    S2::StoreDataMem(ba, *file, number);
    writeFile(number, ba);
}

void BaseInterface::reqTime()
{
    BI_CommandStruct bi { C_ReqTime, 0, 0 };
    DataManager::GetInstance().addToInQueue(bi);
}

void BaseInterface::writeTime(quint32 time)
{
    BI_CommandStruct bi { C_WriteTime, time, 0 };
    DataManager::GetInstance().addToInQueue(bi);
}

void BaseInterface::writeCommand(Queries::Commands cmd, QVariant value)
{
    BI_CommandStruct bi { C_WriteCommand, cmd, value };
    DataManager::GetInstance().addToInQueue(bi);
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
    auto file = msg.value<DataTypes::FileStruct>();
    disconnect(proxyFS.get(), &DataTypesProxy::DataStorable, this, &BaseInterface::fileReceived);
    m_byteArrayResult = file.data;
    m_busy = false;
}

void BaseInterface::timeout()
{
    m_busy = false;
}
