#include "dataitem.h"

#include <type_traits>

namespace S2
{

DataItem::DataItem() : data(DWORD(0))
{
}

DataItem::DataItem(const valueType &value) : data(value)
{
}

bool operator==(const DataItem &lhs, const DataItem &rhs)
{
    return lhs.data == rhs.data;
}

bool operator!=(const DataItem &lhs, const DataItem &rhs)
{
    return !(lhs == rhs);
}

QByteArray DataItem::toByteArray() const
{
    return std::visit(
        [](auto &arg) -> QByteArray {
            using internalType = std::remove_reference_t<decltype(arg)>;
            auto dataPtr = reinterpret_cast<const char *>(&arg);
            return QByteArray::fromRawData(dataPtr, sizeof(internalType));
        },
        data);
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

}
