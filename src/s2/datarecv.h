#ifndef DATARECV_H
#define DATARECV_H

#include "datarec.h"
#include "valuemap.h"

#include <QVariant>

template <typename T, size_t N1, size_t N2>
auto inline operator<<(std::array<std::array<T, N2>, N1> &array, const QStringList &list) -> decltype(array)
{
    Q_ASSERT(size_t(list.size()) <= (N1 * N2));
    T *ptr = reinterpret_cast<T *>(array.data());
    for (auto i = 0; i != (N1 * N2); ++i)
    {
        *ptr = QVariant(list.at(i)).value<T>();
    }
    return array;
}

template <typename T, size_t N> std::array<T, N> inline operator<<(std::array<T, N> &array, const QStringList &list)
{
    Q_ASSERT(size_t(list.size()) <= array.size());
    std::transform(
        list.cbegin(), list.cend(), array.begin(), [](const QString &str) { return QVariant(str).value<T>(); });
    return array;
}

template <typename T, size_t N> std::array<T, N> inline operator<<(std::array<T, N> &array, const QString str)
{
    const auto list = str.split(',');
    return (array << list);
}

namespace DataTypes
{

class DataRecV
{
public:
    DataRecV() = default;
    DataRecV(const S2DataTypes::DataRec &record);
    DataRecV(const S2DataTypes::DataRec &record, const char *rawdata);
    DataRecV(const unsigned _id, const QString &str);
    DataRecV(const unsigned _id);

    template <typename T, std::enable_if_t<ValueMap::true_type<T>::value, bool> = true>
    DataRecV(quint16 _id, T _data) : id(_id), data(_data)
    {
    }

    friend bool operator==(const DataRecV &lhs, const DataRecV &rhs);
    friend bool operator!=(const DataRecV &lhs, const DataRecV &rhs);

    void printer() const;
    S2DataTypes::DataRec serialize() const;
    quint16 getId() const;
    valueType getData() const;
    void setData(const valueType &value);
    size_t typeIndex() const;

    template <typename T, std::enable_if_t<ValueMap::true_type<T>::value, bool> = true> T value() const
    {
        assert(std::holds_alternative<T>(data) && "Requested wrong type");
        if (std::holds_alternative<T>(data))
            return std::get<T>(data);
        else
            return T();
    }

    template <typename T> void changeValue(const T value)
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

    template <typename T> void helper(unsigned int numByte, const char *rawdata, valueType &data)
    {
        constexpr auto hash = ctti::unnamed_type_id<T>().hash();
        assert(sizeof(T) == numByte);
        data = *reinterpret_cast<const T *>(rawdata);
    }

    template <typename T, std::enable_if_t<std_ext::is_container<T>::value, bool> = true>
    valueType helper(const QString &str)
    {
        T arr {};
        arr << str;
        data = arr;
        return valueType(arr);
    }

    template <typename T, std::enable_if_t<!std_ext::is_container<T>::value, bool> = true>
    valueType helper(const QString &str)
    {
        valueType d = QVariant(str).value<T>();
        return d;
    }
};

bool operator==(const DataRecV &lhs, const DataRecV &rhs);
bool operator!=(const DataRecV &lhs, const DataRecV &rhs);

struct RecordPair
{
    DataRecV record;
    bool visibility = true;
};

}

Q_DECLARE_METATYPE(DataTypes::DataRecV)

#endif // DATARECV_H
