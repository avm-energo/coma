#ifndef VALUEMAP_H
#define VALUEMAP_H

#include "../ctti/type_id.hpp"

#include <gen/datatypes.h>
#include <gen/std_ext.h>
#include <map>

namespace DataTypes
{

class ValueMap
{
public:
    using value_type = std::map<int, ctti::unnamed_type_id_t>;

    template <typename T> struct true_type
    {
        static constexpr bool value = std_ext::is_variant_alternative<T, valueType>();
    };

    template <typename T, std::enable_if_t<true_type<T>::value, bool> = true> void insert(int key)
    {
        m_map.insert({ key, ctti::unnamed_type_id<T>() });
    }

    void insert(int key, ctti::unnamed_type_id_t value)
    {
        m_map.insert({ key, value });
    }

    const ValueMap::value_type &map() const
    {
        return m_map;
    }

private:
    value_type m_map;
};

}

#endif // VALUEMAP_H
