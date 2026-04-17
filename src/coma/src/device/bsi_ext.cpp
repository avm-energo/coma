#include "device/bsi_ext.h"

#include <avm-gen/stdfunc.h>

#include <algorithm>

namespace Device
{

BlockStartupInfoExtended::BlockStartupInfoExtended(QObject *parent) : QObject(parent), m_startReg(0), m_endReg(0) { }

void BlockStartupInfoExtended::updateStructure(const XmlDataTypes::BsiExtSettings &records)
{
    if (records.empty())
        return;
    u32 maxOffset = 0;
    for (const auto &rec : records)
        maxOffset = std::max(maxOffset, rec.offset);
    m_data.assign(maxOffset + 1, 0);
    m_startReg = bsiExtStartReg;
    m_endReg = bsiExtStartReg + maxOffset;
}

void BlockStartupInfoExtended::updateData(const DataTypes::BitStringStruct &value)
{
    if (m_data.empty())
        return;
    if (value.sigAdr >= m_startReg && value.sigAdr <= m_endReg)
    {
        const auto idx = value.sigAdr - m_startReg;
        m_data[idx] = value.sigVal;
        emit wasUpdated(value.sigAdr);
    }
}

BlockStartupInfoExtended::Iter BlockStartupInfoExtended::begin() const noexcept
{
    return m_data.cbegin();
}

BlockStartupInfoExtended::Iter BlockStartupInfoExtended::end() const noexcept
{
    return m_data.cend();
}

u32 BlockStartupInfoExtended::operator[](const u32 offset) const
{
    if (offset < static_cast<u32>(m_data.size()))
        return m_data[offset];
    return 0;
}

u32 BlockStartupInfoExtended::size() const
{
    return static_cast<u32>(m_data.size());
}

QByteArray BlockStartupInfoExtended::toByteArray()
{
    QByteArray ba;
    for (u32 i = startAddr(); i < endAddr(); ++i)
    {
        ba.append(StdFunc::toByteArray(m_data[i]));
    }
    return ba;
}

void BlockStartupInfoExtended::fromByteArray(const QByteArray &ba)
{
    u32 endOffset = ba.size();
    u32 offset = 0;
    m_data.clear();
    while (offset < endOffset)
    {
        m_data.push_back(StdFunc::getFromByteArray<u32>(ba, offset));
        offset += 4;
    }
}

u32 BlockStartupInfoExtended::startAddr()
{
    return m_startReg;
}

u32 BlockStartupInfoExtended::endAddr()
{
    return m_endReg;
}

} // namespace Device
