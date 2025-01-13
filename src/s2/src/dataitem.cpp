#include "s2/dataitem.h"

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
    if (std::holds_alternative<S2::GasDensity_3t>(data))
    {
        S2::GasDensity_3t density = value<S2::GasDensity_3t>();
        std::array<S2::GasDensity, 3> densArray;
        if (density.density.size() > 2)
        {
            for (int i = 0; i < 3; ++i)
                densArray[i] = density.density.at(i);
        }
        else
            return QByteArray();
        char *dataPtr = reinterpret_cast<char *>(&densArray);
        QByteArray ba(dataPtr, sizeof(densArray));
        return ba;
    }
    else
    {
        int dataSizeof = std::visit(
            [](auto &arg) -> int {
                using internalType = std::remove_reference_t<decltype(arg)>;
                return sizeof(internalType);
            },
            data);
        const char *dataPtr = reinterpret_cast<const char *>(&data);
        QByteArray ba(dataPtr, dataSizeof);
        return ba;
    }
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
