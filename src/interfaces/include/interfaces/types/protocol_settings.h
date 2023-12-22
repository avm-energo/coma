#pragma once

#include <interfaces/types/common_types.h>
#include <map>

namespace Protocol
{

using ui16 = std::uint16_t;
using ui32 = std::uint32_t;

struct AbstractGroup
{
    Interface::IfaceType ifaceType;
    ui32 signalId;
    ui16 arg1;
    ui16 arg2;
};

struct BaseGroup
{
    ui32 m_startAddr;
    ui32 m_count;

    explicit BaseGroup() noexcept;
    explicit BaseGroup(const ui32 addr, const ui32 count) noexcept;
};

struct ProtocomGroup final : public BaseGroup
{
    ui16 m_block;

    explicit ProtocomGroup() noexcept;
    explicit ProtocomGroup(const ui32 addr, const ui32 count, const ui16 block) noexcept;
};

struct ModbusGroup final : public BaseGroup
{
    ui16 m_function;

    explicit ModbusGroup() noexcept;
    explicit ModbusGroup(const ui32 addr, const ui32 count, const ui16 function) noexcept;
};

struct Iec104Group final : public BaseGroup
{
    ui16 m_transType;
    ui16 m_group;

    explicit Iec104Group() noexcept;
    explicit Iec104Group(const ui32 addr, const ui32 count, const ui16 transType, const ui16 group) noexcept;
};

using ProtocolGroup = std::variant<ProtocomGroup, ModbusGroup, Iec104Group>;

/// \brief Контейнер для хранения описания протокола связи.
class ProtocolDescription final
{
private:
    std::map<ui32, ProtocolGroup> m_groups;

public:
    /// \brief Добавление группы в описание протокола.
    void addGroup(const ProtocolGroup &group) noexcept;
    /// \brief Доступ к внутреннему контейнеру данных.
    const std::map<ui32, ProtocolGroup> &groups() const noexcept;
    /// \brief Очистка описания протокола связи.
    void clear() noexcept;
    /// \brief Слияние двух объектов описания протокола связи.
    void merge(const ProtocolDescription &rhs) noexcept;
};

} // namespace Protocol

using ProtocolDescription = Protocol::ProtocolDescription;
Q_DECLARE_METATYPE(ProtocolDescription)
