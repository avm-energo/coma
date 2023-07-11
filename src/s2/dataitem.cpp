#include "dataitem.h"

#include "../module/configstorage.h"

#include <type_traits>

namespace S2DataTypes
{

DataItem::DataItem(quint16 _id) : DataItem(_id, 0)
{
}

DataItem::DataItem(quint16 _id, const valueType &value) : id(_id), data(value)
{
}

bool operator==(const DataItem &lhs, const DataItem &rhs)
{
    return (lhs.id == rhs.id) && (lhs.data == rhs.data);
}

bool operator!=(const DataItem &lhs, const DataItem &rhs)
{
    return !(lhs == rhs);
}

DataRec DataItem::serialize() const
{
    return std::visit(
        [=](auto &arg) -> DataRec {
            using internalType = std::remove_reference_t<decltype(arg)>;
            DataRec record { { id, sizeof(internalType) }, (void *)(&arg) };
            return record;
        },
        data);
}

quint16 DataItem::getId() const
{
    return id;
}

valueType DataItem::getData() const
{
    return data;
}

void DataItem::setData(const valueType &value)
{
    // not true setter, only swapper for same internal types
    assert(data.index() == value.index());
    data = value;
}

size_t DataItem::typeIndex() const
{
    return data.index();
}

}
