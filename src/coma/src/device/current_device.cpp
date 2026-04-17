#include "device/current_device.h"

#include <device/device_list.h>

namespace Device
{

CurrentDevice::CurrentDevice(AsyncConnection *conn)
    : QObject(conn)
    , m_async(conn)
    , m_sync(m_async)
    , m_bsi {}
    , m_previous {}
    , m_bsiExt(this)
    , m_cfgStorage(this)
    , m_s2manager(this)
    , m_fileProvider(this)
    , m_isInitStage(true)
{
    m_async->connection(this, &CurrentDevice::updateBSI);
    connect(m_async, &AsyncConnection::responseError, this, [this](Error::Msg) { initBSIEvent(Error::Msg::Timeout); });
}

CurrentDevice *DeviceFabric::create(AsyncConnection *connection)
{
    Q_ASSERT(connection != nullptr);
    return new CurrentDevice(connection);
}

AsyncConnection *CurrentDevice::async() noexcept
{
    return m_async;
}

SyncConnection *CurrentDevice::sync() noexcept
{
    return &m_sync;
}

const Bsi &CurrentDevice::bsi() const noexcept
{
    return m_bsi;
}

BlockStartupInfoExtended &CurrentDevice::bsiExt() noexcept
{
    return m_bsiExt;
}

Health CurrentDevice::health() const noexcept
{
    return Health(m_bsi.data(BsiIndexes::Hth));
}

ConfigStorage *CurrentDevice::getConfigStorage() noexcept
{
    return &m_cfgStorage;
}

S2DataManager *CurrentDevice::getS2Datamanager() noexcept
{
    return &m_s2manager;
}

FileProvider *CurrentDevice::getFileProvider() noexcept
{
    return &m_fileProvider;
}

u16 CurrentDevice::getBaseType() const noexcept
{
    return static_cast<u16>(m_bsi.data(BsiIndexes::MTypeB));
}

u16 CurrentDevice::getMezzType() const noexcept
{
    return static_cast<u16>(m_bsi.data(BsiIndexes::MTypeM));
}

u16 CurrentDevice::getDeviceType() const noexcept
{
    return u16((m_bsi.data(BsiIndexes::MTypeB) << 8) + m_bsi.data(BsiIndexes::MTypeM));
}

QString CurrentDevice::getDeviceName() const noexcept
{
    auto search = BoxModules.find(static_cast<Model>(getDeviceType()));
    if (search == BoxModules.cend())
    {
        auto bSearch = BaseBoards.find(static_cast<BaseBoard>(m_bsi.data(BsiIndexes::MTypeB)));
        auto mSearch = MezzanineBoards.find(static_cast<MezzanineBoard>(m_bsi.data(BsiIndexes::MTypeM)));
        if (bSearch == BaseBoards.cend())
            return "undefined device";
        else if (mSearch == MezzanineBoards.cend())
            return bSearch->second.data();
        else
            return QString(bSearch->second.data()).append(mSearch->second.data());
    }
    else
        return search->second.data();
}

QString CurrentDevice::getUID() const noexcept
{
    return QString::number(m_bsi.data(BsiIndexes::UIDHigh), 16) + QString::number(m_bsi.data(BsiIndexes::UIDMid), 16)
        + QString::number(m_bsi.data(BsiIndexes::UIDLow), 16);
}

u32 CurrentDevice::getSerialNumber() const noexcept
{
    return m_bsi.data(BsiIndexes::SerialNum);
}

bool CurrentDevice::isOutdatedFirmware(const u32 configVersion) const noexcept
{
    return m_bsi.data(BsiIndexes::Fwver) < configVersion;
}

void CurrentDevice::initBSI() noexcept
{
    m_isInitStage = true;
    m_async->reqBitStrings(Device::bsiStartReg, Device::bsiCountRegs);
}

void CurrentDevice::internalProtocolUpdate() noexcept
{
    Q_ASSERT(m_cfgStorage.getProtocolDescription().groups().size() > 0);
    m_async->updateProtocol(m_cfgStorage.getProtocolDescription());
}

void CurrentDevice::updateBSI(const DataTypes::BitStringStruct &value)
{
    if (value.sigAdr >= Device::bsiStartReg && value.sigAdr <= Device::bsiCountRegs)
    {
        m_bsi.setData(value.sigAdr - Device::bsiStartReg, value.sigVal);
        if (m_bsi.isFilled())
        {
            m_bsi.clearBsiFill();
            initBSIEvent(Error::Msg::NoError);
            compareAndUpdate();
        }
    }
    else if (value.sigAdr >= m_bsiExt.startAddr() && value.sigAdr <= m_bsiExt.endAddr())
        m_bsiExt.updateData(value);
}

void CurrentDevice::configFileLoadFinished()
{
    if (getConfigStorage()->getDeviceSettings().HaveBSIExt())
    {
        bsiExt().updateStructure(getConfigStorage()->getDeviceSettings().getBsiExt());
    }
}

void CurrentDevice::compareAndUpdate() noexcept
{
    if (m_bsi.data(BsiIndexes::Hth) != m_previous.data(BsiIndexes::Hth))
        emit healthChanged(m_bsi.data(BsiIndexes::Hth));
    if (m_bsi.data(BsiIndexes::MTypeB) != m_previous.data(BsiIndexes::MTypeB)
        || m_bsi.data(BsiIndexes::MTypeM) != m_previous.data(BsiIndexes::MTypeM))
        emit typeChanged(QString::number(getDeviceType(), 16));
    if (m_bsi.data(BsiIndexes::SerialNum) != m_previous.data(BsiIndexes::SerialNum))
        emit serialChanged(m_bsi.data(BsiIndexes::SerialNum));

    m_previous = m_bsi;
    emit bsiReceived();
}

void CurrentDevice::initBSIEvent(const Error::Msg status) noexcept
{
    if (m_isInitStage)
    {
        m_isInitStage = false;
        emit initBSIFinished(status);
    }
}

} // namespace Device
