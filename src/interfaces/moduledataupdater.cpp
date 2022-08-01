#include "moduledataupdater.h"

ModuleDataUpdater::ModuleDataUpdater(QObject *parent) : QObject(parent)
{
    proxyFS = UniquePointer<DataTypesProxy>(new DataTypesProxy(&DataManager::GetInstance()));
    proxySP = UniquePointer<DataTypesProxy>(new DataTypesProxy(&DataManager::GetInstance()));
    proxyBS = UniquePointer<DataTypesProxy>(new DataTypesProxy(&DataManager::GetInstance()));
    proxyFS->RegisterType<DataTypes::FloatStruct>();
    proxySP->RegisterType<DataTypes::SinglePointWithTimeStruct>();
    proxyBS->RegisterType<DataTypes::BitStringStruct>();
    connect(proxyFS.get(), &DataTypesProxy::DataStorable, this, &ModuleDataUpdater::updateFloatData);
    connect(proxySP.get(), &DataTypesProxy::DataStorable, this, &ModuleDataUpdater::updateSinglePointData);
    connect(proxyBS.get(), &DataTypesProxy::DataStorable, this, &ModuleDataUpdater::updateBitStringData);
}

void ModuleDataUpdater::requestUpdates()
{
    // NOTE: POS (Piece Of Shit)
    if (!m_updatesEnabled)
        return;
    for (const auto &query : qAsConst(m_floatQueryList))
        BaseInterface::iface()->reqFloats(query.sigAdr, query.sigQuantity);
    for (const auto &query : qAsConst(m_spQueryList))
        BaseInterface::iface()->reqAlarms(query.sigAdr, query.sigQuantity);
    for (const auto &query : qAsConst(m_bsQueryList))
        BaseInterface::iface()->reqBitStrings(query.sigAdr, query.sigQuantity);
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

void ModuleDataUpdater::updateFloatData(const QVariant &msg)
{
    if (msg.canConvert<DataTypes::FloatStruct>())
    {
        if (m_updatesEnabled)
        {
            auto fl = msg.value<DataTypes::FloatStruct>();
            emit itsTimeToUpdateFloatSignal(fl);
        }
    }
}

void ModuleDataUpdater::updateSinglePointData(const QVariant &msg)
{
    if (msg.canConvert<DataTypes::SinglePointWithTimeStruct>())
    {
        if (m_updatesEnabled)
        {
            auto sp = msg.value<DataTypes::SinglePointWithTimeStruct>();
            emit itsTimeToUpdateSinglePointSignal(sp);
        }
    }
}

void ModuleDataUpdater::updateBitStringData(const QVariant &msg)
{
    if (msg.canConvert<DataTypes::BitStringStruct>())
    {
        if (m_updatesEnabled)
        {
            auto bs = msg.value<DataTypes::BitStringStruct>();
            emit itsTimeToUpdateBitStringSignal(bs);
        }
    }
}
