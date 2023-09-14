#pragma once

#include <s2/s2datatypes.h>

namespace S2
{

class DataItem
{
private:
    valueType data;

public:
    DataItem();
    DataItem(const valueType &value);

    friend bool operator==(const DataItem &lhs, const DataItem &rhs);
    friend bool operator!=(const DataItem &lhs, const DataItem &rhs);

    QByteArray toByteArray() const;
    valueType getData() const;
    void setData(const valueType &value);

    template <typename T, std::enable_if_t<isValueType<T>::value, bool> = true> //
    T value() const
    {
        assert(std::holds_alternative<T>(data) && "Requested wrong type");
        if (std::holds_alternative<T>(data))
            return std::get<T>(data);
        else
            return T();
    }

    template <typename T> //
    void changeValue(const T value)
    {
        if (!data.valueless_by_exception())
        {
            assert(std::holds_alternative<T>(data) && "Target wrong type");
            if (auto pval = std::get_if<T>(&data))
                *pval = value;
        }
        else
            data = value;
    }
};

bool operator==(const DataItem &lhs, const DataItem &rhs);
bool operator!=(const DataItem &lhs, const DataItem &rhs);

}

Q_DECLARE_METATYPE(S2::DataItem)
