#ifndef DATAITEM_H
#define DATAITEM_H

#include "s2datatypes.h"

namespace S2DataTypes
{

class DataItem
{
public:
    DataItem() = default;
    DataItem(quint16 _id);
    DataItem(quint16 _id, const valueType &value);

    template <typename T, std::enable_if_t<isValueType<T>::value, bool> = true>
    DataItem(quint16 _id, T _data) : id(_id), data(_data)
    {
    }

    friend bool operator==(const DataItem &lhs, const DataItem &rhs);
    friend bool operator!=(const DataItem &lhs, const DataItem &rhs);

    void printer() const;
    DataRec serialize() const;
    quint16 getId() const;
    valueType getData() const;
    void setData(const valueType &value);
    size_t typeIndex() const;

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

private:
    quint16 id;
    valueType data;
};

bool operator==(const DataItem &lhs, const DataItem &rhs);
bool operator!=(const DataItem &lhs, const DataItem &rhs);

struct RecordPair
{
    DataItem record;
    bool visibility = true;
};

}

Q_DECLARE_METATYPE(S2DataTypes::DataItem)

#endif // DATAITEM_H
