#include "interfaces/moduledataupdater.h"

ModuleDataUpdater::ModuleDataUpdater(BaseConnection *connection, QObject *parent) : QObject(parent), m_conn(connection)
{
    // m_conn = connection;
    //    proxyFS = UniquePointer<DataTypesProxy>(new DataTypesProxy(&DataManager::GetInstance()));
    //    proxySP = UniquePointer<DataTypesProxy>(new DataTypesProxy(&DataManager::GetInstance()));
    //    proxyBS = UniquePointer<DataTypesProxy>(new DataTypesProxy(&DataManager::GetInstance()));
    //    proxyFS->RegisterType<DataTypes::FloatStruct>();
    //    proxySP->RegisterType<DataTypes::SinglePointWithTimeStruct>();
    //    proxyBS->RegisterType<DataTypes::BitStringStruct>();
    //    connect(proxyFS.get(), &DataTypesProxy::DataStorable, this, &ModuleDataUpdater::updateFloatData);
    //    connect(proxySP.get(), &DataTypesProxy::DataStorable, this, &ModuleDataUpdater::updateSinglePointData);
    //    connect(proxyBS.get(), &DataTypesProxy::DataStorable, this, &ModuleDataUpdater::updateBitStringData);
    m_conn->connection(this, &ModuleDataUpdater::updateFloatData);
    m_conn->connection(this, &ModuleDataUpdater::updateSinglePointData);
    m_conn->connection(this, &ModuleDataUpdater::updateBitStringData);
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

void ModuleDataUpdater::setFloatQuery(const QList<BdQuery> &list)
{
    m_floatQueryList = list;
}

void ModuleDataUpdater::setSpQuery(const QList<BdQuery> &list)
{
    m_spQueryList = list;
}

void ModuleDataUpdater::setBsQuery(const QList<BdQuery> &list)
{
    m_bsQueryList = list;
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
