#include "device/bsi.h"

#include <device/device_list.h>

namespace Device
{

BsiHolder::BsiHolder(QObject *parent) noexcept : QObject(parent), m_bsi {}
{
}

const BlockStartupInfo &BsiHolder::getBSI() const noexcept
{
    return m_bsi;
}

u16 BsiHolder::getDeviceType() const noexcept
{
    return u16((m_bsi.MTypeB << 8) + m_bsi.MTypeM);
}

QString BsiHolder::getDeviceName() const noexcept
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

QString BsiHolder::getUID() const noexcept
{
    return QString::number(m_bsi.UIDHigh, 16) + QString::number(m_bsi.UIDMid, 16) + QString::number(m_bsi.UIDLow, 16);
}

bool BsiHolder::isOutdatedFirmware(const u32 configVersion) const noexcept
{
    return m_bsi.Fwver < configVersion;
}

void BsiHolder::update(const DataTypes::BitStringStruct &value)
{
    Q_UNUSED(value);
}

} // namespace Device
