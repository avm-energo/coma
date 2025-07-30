#include "device/bsi_ext.h"

#include <gen/stdfunc.h>
namespace Device
{

BlockStartupInfoExtended::BlockStartupInfoExtended(QObject *parent) : QObject(parent), m_startReg(0), m_endReg(0) { }

void BlockStartupInfoExtended::updateStructure(const XmlDataTypes::BsiExtItemList &items)
{
    if (!items.empty())
    {
        m_data.clear();
        for (const auto &item : items)
            m_data.insert({ item.address, 0 });
        m_startReg = m_data.begin()->first;
        m_endReg = m_startReg + static_cast<u32>(m_data.size() - 1);
    }
}

void BlockStartupInfoExtended::updateData(const DataTypes::BitStringStruct &value)
{
    if (value.sigAdr >= m_startReg && value.sigAdr <= m_endReg)
    {
        auto search = m_data.find(value.sigAdr);
        if (search != m_data.end())
        {
            search->second = value.sigVal;
            if (value.sigAdr == m_endReg)
                emit wasUpdated();
        }
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

u32 BlockStartupInfoExtended::operator[](const u32 address) const
{
    auto search = m_data.find(address);
    if (search != m_data.end())
        return search->second;
    else
        return 0;
}

u32 BlockStartupInfoExtended::size() const
{
    return m_data.size();
}

QByteArray BlockStartupInfoExtended::toByteArray() const
{
    QByteArray ba;
    ba.append(StdFunc::toByteArray(m_data.begin()->first)); // start addr into the first u32 of bytearray
    for (auto iter = m_data.begin(); iter != m_data.end(); ++iter)
    {
        ba.append(StdFunc::toByteArray(iter->second));
    }
    return ba;
}

void BlockStartupInfoExtended::fromByteArray(QByteArray &ba)
{
    u32 endOffset = ba.size() - sizeof(u32);
    u32 offset = 0;
    u32 startAddr = StdFunc::getFromByteArray<u32>(ba, offset);
    offset += sizeof(u32);
    m_data.clear();
    while (offset < endOffset)
    {
        m_data.insert({ startAddr++, StdFunc::getFromByteArray<u32>(ba, offset) });
        offset += 4;
    }
}

} // namespace Device
