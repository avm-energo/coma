#include "interfaces/types/protocol_settings.h"

#include <QString>

namespace Protocol
{

BaseGroup::BaseGroup() noexcept = default;
BaseGroup::BaseGroup(const ui32 addr, const ui32 count) noexcept : m_startAddr(addr), m_count(count)
{
}

ProtocomGroup::ProtocomGroup() noexcept = default;
ProtocomGroup::ProtocomGroup(const ui32 addr, const ui32 count, const ui16 block) noexcept
    : BaseGroup(addr, count), m_block(block)
{
}

ModbusGroup::ModbusGroup() noexcept = default;
ModbusGroup::ModbusGroup(const ui32 addr, const ui32 count, const quint16 function) noexcept
    : BaseGroup(addr, count), m_function(QString("%1").arg(function).toUInt(nullptr, 16))
{
}

Iec104Group::Iec104Group() noexcept = default;
Iec104Group::Iec104Group(const ui32 addr, const ui32 count, const ui16 transType, const ui16 group) noexcept
    : BaseGroup(addr, count), m_transType(transType), m_group(group)
{
}

void ProtocolDescription::addGroup(const ProtocolGroup &group) noexcept
{
    std::visit([&group, this](const BaseGroup &base) { m_groups.insert({ base.m_startAddr, group }); }, group);
}

const std::map<ui32, ProtocolGroup> &ProtocolDescription::groups() const noexcept
{
    return m_groups;
}

void ProtocolDescription::clear() noexcept
{
    m_groups.clear();
}

void ProtocolDescription::merge(const ProtocolDescription &rhs) noexcept
{
    for (auto &node : rhs.m_groups)
    {
        auto [_, status] = m_groups.insert(node);
        if (!status)
        {
            auto search = m_groups.find(node.first);
            if (search != m_groups.end())
                search->second = node.second;
        }
    }
}

} // namespace Protocol
