#include "interfaces/moduledataupdater.h"

ModuleDataUpdater::ModuleDataUpdater(AsyncConnection *connection, QObject *parent) : QObject(parent), m_conn(nullptr)
{
    updateConnection(connection);
}

void ModuleDataUpdater::updateConnection(AsyncConnection *connection) noexcept
{
    m_conn = connection;
    if (m_conn != nullptr)
    {
        enableFloatDataUpdates();
        enableSinglePointDataUpdates();
        enableBitStringDataUpdates();
    }
}

AsyncConnection *ModuleDataUpdater::currentConnection() noexcept
{
    return m_conn;
}

void ModuleDataUpdater::requestUpdates()
{
    // NOTE: POS (Piece Of Shit)
    if (!m_updatesEnabled)
        return;
    for (const auto &query : qAsConst(m_floatQueryList))
        m_conn->reqFloats(query.sigAdr, query.sigQuantity);
    for (const auto &query : qAsConst(m_spQueryList))
        m_conn->reqAlarms(query.sigAdr, query.sigQuantity);
    for (const auto &query : qAsConst(m_bsQueryList))
        m_conn->reqBitStrings(query.sigAdr, query.sigQuantity);
}

bool ModuleDataUpdater::updatesEnabled()
{
    return m_updatesEnabled;
}

void ModuleDataUpdater::setUpdatesEnabled(bool enabled)
{
    m_updatesEnabled = enabled;
}

void ModuleDataUpdater::enableFloatDataUpdates() noexcept
{
    if (m_floatDataConn)
        disconnect(m_floatDataConn);
    m_floatDataConn = m_conn->connection(this, &ModuleDataUpdater::updateFloatData);
}

void ModuleDataUpdater::enableSinglePointDataUpdates() noexcept
{
    if (m_singlePointDataConn)
        disconnect(m_singlePointDataConn);
    m_singlePointDataConn = m_conn->connection(this, &ModuleDataUpdater::updateSinglePointData);
}

void ModuleDataUpdater::enableBitStringDataUpdates() noexcept
{
    if (m_bitStringDataConn)
        disconnect(m_bitStringDataConn);
    m_bitStringDataConn = m_conn->connection(this, &ModuleDataUpdater::updateBitStringData);
}

void ModuleDataUpdater::disableUpdates() noexcept
{
    if (m_floatDataConn)
        disconnect(m_floatDataConn);
    if (m_singlePointDataConn)
        disconnect(m_singlePointDataConn);
    if (m_bitStringDataConn)
        disconnect(m_bitStringDataConn);
}

void ModuleDataUpdater::addFloat(const BdQuery &query)
{
    m_floatQueryList.push_back(query);
}

void ModuleDataUpdater::addSp(const BdQuery &query)
{
    m_spQueryList.push_back(query);
}

void ModuleDataUpdater::addBs(const BdQuery &query)
{
    m_bsQueryList.push_back(query);
}

void ModuleDataUpdater::updateFloatData(const DataTypes::FloatStruct &fl)
{
    if (m_updatesEnabled)
        emit itsTimeToUpdateFloatSignal(fl);
}

void ModuleDataUpdater::updateSinglePointData(const DataTypes::SinglePointWithTimeStruct &sp)
{
    if (m_updatesEnabled)
        emit itsTimeToUpdateSinglePointSignal(sp);
}

void ModuleDataUpdater::updateBitStringData(const DataTypes::BitStringStruct &bs)
{
    if (m_updatesEnabled)
        emit itsTimeToUpdateBitStringSignal(bs);
}
