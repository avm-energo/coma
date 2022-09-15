#pragma once

#include "../../module/configstorage.h"

#include <QDebug>
#include <QMetaEnum>
#include <QtXml/QDomElement>
#include <utility>

struct InterfaceSettings
{
    QVariant settings;
};

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

    bool operator==(const BaseGroup &rhs) const
    {
        return ((function == rhs.function) && (dataType == rhs.dataType) && (startAddr == rhs.startAddr));
    }

    bool operator!=(const BaseGroup &rhs) const
    {
        return !(*this == rhs);
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
