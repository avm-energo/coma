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
    , m_cfgStorage(this)
    , m_s2manager(this)
    , m_fileProvider(this)
    , m_timeoutTimer(this)
    , m_bsiCounter(0)
    , m_isInitStage(true)
{
    m_async->connection(this, &CurrentDevice::updateBSI);
    m_timeoutTimer.setInterval(m_async->getTimeout());
    connect(&m_timeoutTimer, &QTimer::timeout, this, [this] { initBSIEvent(Error::Msg::Timeout); });
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

const BlockStartupInfo &CurrentDevice::bsi() const noexcept
{
    return m_bsi;
}

Health CurrentDevice::health() const noexcept
{
    return Health(m_bsi.Hth);
}

ConfigStorage *CurrentDevice::getConfigStorage() noexcept
{
    return &m_cfgStorage;
}

S2DataManager *CurrentDevice::getS2Datamanager() noexcept
{
    return &m_s2manager;
}

u16 CurrentDevice::getBaseType() const noexcept
{
    return m_bsi.MTypeB;
}

u16 CurrentDevice::getMezzType() const noexcept
{
    return m_bsi.MTypeM;
}

u16 CurrentDevice::getDeviceType() const noexcept
{
    return u16((m_bsi.MTypeB << 8) + m_bsi.MTypeM);
}

QString CurrentDevice::getDeviceName() const noexcept
{
    auto search = BoxModules.find(static_cast<Model>(getDeviceType()));
    if (search == BoxModules.cend())
    {
        auto bSearch = BaseBoards.find(static_cast<BaseBoard>(m_bsi.MTypeB));
        auto mSearch = MezzanineBoards.find(static_cast<MezzanineBoard>(m_bsi.MTypeM));
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
    return QString::number(m_bsi.UIDHigh, 16) + QString::number(m_bsi.UIDMid, 16) + QString::number(m_bsi.UIDLow, 16);
}

u32 CurrentDevice::getSerialNumber() const noexcept
{
    return m_bsi.SerialNum;
}

bool CurrentDevice::isOutdatedFirmware(const u32 configVersion) const noexcept
{
    return m_bsi.Fwver < configVersion;
}

void CurrentDevice::initBSI() noexcept
{
    m_isInitStage = true;
    m_async->reqBSI();
    m_timeoutTimer.stop();
}

void CurrentDevice::internalProtocolUpdate() noexcept
{
    Q_ASSERT(m_cfgStorage.getProtocolDescription().groups().size() > 0);
    m_async->updateProtocol(m_cfgStorage.getProtocolDescription());
}

void CurrentDevice::updateBSI(const DataTypes::BitStringStruct &value)
{
    using namespace Interface;
    constexpr auto bsiMembers = sizeof(BlockStartupInfo) / sizeof(u32);
    if (value.sigAdr >= addr::bsiStartReg && value.sigAdr <= addr::bsiCountRegs)
    {
        u32 &item = *(reinterpret_cast<u32 *>(&m_bsi) + (value.sigAdr - addr::bsiStartReg));
        item = value.sigVal;
        ++m_bsiCounter;
        if (m_bsiCounter == bsiMembers)
        {
            initBSIEvent(Error::Msg::NoError);
            m_bsiCounter = 0;
            compareAndUpdate();
        }
    }
}

void CurrentDevice::compareAndUpdate() noexcept
{
    if (m_bsi.Hth != m_previous.Hth)
        emit healthChanged(m_bsi.Hth);
    if ((m_bsi.MTypeB != m_previous.MTypeB) || (m_bsi.MTypeM != m_previous.MTypeM))
        emit typeChanged(getDeviceType());
    if (m_bsi.SerialNum != m_previous.SerialNum)
        emit serialChanged(m_bsi.SerialNum);

    m_previous = m_bsi;
    emit bsiReceived();
}

void CurrentDevice::initBSIEvent(const Error::Msg status) noexcept
{
    if (m_isInitStage)
    {
        m_timeoutTimer.stop();
        m_isInitStage = false;
        emit initBSIFinished(status);
    }
}

} // namespace Device
