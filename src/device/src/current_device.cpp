#include "device/current_device.h"

namespace Device
{

CurrentDevice::CurrentDevice(AsyncConnection *conn)
    : QObject(conn), m_async(conn), m_sync(m_async), m_holder(this), m_cfgStorage(this), m_s2manager(this)
{
    m_async->connection(&m_holder, &BsiHolder::update);
}

CurrentDevice *DeviceFabric::create(AsyncConnection *connection)
{
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

const BlockStartupInfo &CurrentDevice::bsi() noexcept
{
    return m_holder.getBSI();
}

BsiHolder *CurrentDevice::getBsiHolder() noexcept
{
    return &m_holder;
}

ConfigStorage *CurrentDevice::getConfigStorage() noexcept
{
    return &m_cfgStorage;
}

S2DataManager *CurrentDevice::getS2Datamanager() noexcept
{
    return &m_s2manager;
}

} // namespace Device
