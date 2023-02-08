#include "valuemap.h"

namespace DataTypes
{

void ValueMap::insert(int key, ctti::unnamed_type_id_t value)
{
    m_map.insert({ key, value });
}

const ValueMap::value_type &ValueMap::map() const
{
    return m_map;
}

}
