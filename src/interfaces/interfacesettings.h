#pragma once

#include "../module/configstorage.h"
#include "iec104private.h"
#include "modbusprivate.h"
#include "protocomprivate.h"

#include <QMetaEnum>
#include <utility>

// Thanx to https://stackoverflow.com/questions/34672441
template <template <typename...> class base, typename derived> struct is_base_of_template_impl
{
    template <typename... Ts> static constexpr std::true_type test(const base<Ts...> *);
    static constexpr std::false_type test(...);
    using type = decltype(test(std::declval<derived *>()));
};

template <template <typename...> class base, typename derived>
using is_base_of_template = typename is_base_of_template_impl<base, derived>::type;

template <typename FuncCode, typename TypeId> struct BaseGroup
{
    FuncCode function;
    TypeId dataType;
    quint32 startAddr;
    quint32 count;

    BaseGroup() = default;
    BaseGroup(const quint32 &sigId) : startAddr(0), count(0)
    {
        auto &sigMap = ConfigStorage::GetInstance().getModuleSettings().getSignals();
        if (sigMap.contains(sigId))
        {
            auto &signal = sigMap.value(sigId);
            startAddr = signal.startAddr;
            count = signal.count;
        }
    }
};

// Class have to derived from BaseGroup due to dependent types
template <typename Group, typename = typename std::enable_if<is_base_of_template<BaseGroup, Group>::value>::type>
class InterfaceInfo
{
public:
    void addGroup(const Group &gr)
    {
        m_dictionary.insert(gr.startAddr, gr);
    }

    void clear()
    {
        m_dictionary.clear();
    }

    const auto &dictionary() const
    {
        return m_dictionary;
    }

    void merge(const InterfaceInfo<Group> &rhs)
    {
        for (auto it = rhs.m_dictionary.cbegin(); it != rhs.m_dictionary.cend(); it++)
            m_dictionary.insert(it.key(), it.value());
    }

private:
    QMultiMap<quint32, Group> m_dictionary;
};

struct ProtocomGroup : BaseGroup<Proto::Commands, Proto::TypeId>
{
    quint8 block;

    ProtocomGroup() = default;
    ProtocomGroup(const quint32 &sigId, const quint16 &blk)
        : BaseGroup<Proto::Commands, Proto::TypeId>(sigId), block(blk)
    {
    }
};

struct ModbusGroup : BaseGroup<CommandsMBS::Commands, CommandsMBS::TypeId>
{
    ModbusGroup() = default;
    ModbusGroup(const quint32 &sigId, const quint16 &regType, const QString &type)
        : BaseGroup<CommandsMBS::Commands, CommandsMBS::TypeId>(sigId)
    {
        // Decimal to hex
        auto hexRegType = QString("%1").arg(regType).toUInt(nullptr, 16);
        function = static_cast<CommandsMBS::Commands>(hexRegType);
        // Getting TypeId from QString
        auto buffer(type);
        auto types = QMetaEnum::fromType<CommandsMBS::TypeId>;
        if (!buffer.isEmpty() && types().isValid())
        {
            buffer[0] = buffer[0].toUpper();
            auto typeId = types().keyToValue(buffer.toStdString().c_str());
            if (typeId != -1)
                dataType = static_cast<CommandsMBS::TypeId>(typeId);
        }
    }

    bool operator==(const ModbusGroup &rhs) const
    {
        return ((function == rhs.function) && (dataType == rhs.dataType) && (startAddr == rhs.startAddr));
    }

    bool operator!=(const ModbusGroup &rhs) const
    {
        return !(*this == rhs);
    }
};

struct Iec104Group : BaseGroup<Commands104::Commands, Commands104::TypeId>
{
    Iec104Group() = default;
    Iec104Group(const quint32 &sigId, const quint16 &sigType, const quint16 &transType, const quint16 &sigGroup)
        : BaseGroup<Commands104::Commands, Commands104::TypeId>(sigId)
    {
        if (sigType > 0 && sigType < 129)
            dataType = static_cast<Commands104::TypeId>(sigType);
        // TODO: Any fields?
        Q_UNUSED(transType);
        Q_UNUSED(sigGroup);
    }
};

Q_DECLARE_METATYPE(InterfaceInfo<ProtocomGroup>)
Q_DECLARE_METATYPE(InterfaceInfo<ModbusGroup>)
Q_DECLARE_METATYPE(InterfaceInfo<Iec104Group>)
