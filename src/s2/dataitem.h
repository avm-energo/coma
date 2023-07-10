#ifndef DATAITEM_H
#define DATAITEM_H

#include "s2datatypes.h"

// template <typename T, std::size_t N1, std::size_t N2>
// auto inline operator<<(std::array<std::array<T, N2>, N1> &array, const QStringList &list) -> decltype(array)
//{
//    Q_ASSERT(std::size_t(list.size()) <= (N1 * N2));
//    T *ptr = reinterpret_cast<T *>(array.data());
//    for (auto i = 0; i != (N1 * N2); ++i)
//    {
//        *ptr = QVariant(list.at(i)).value<T>();
//    }
//    return array;
//}

// template <typename T, size_t N> std::array<T, N> inline operator<<(std::array<T, N> &array, const QStringList &list)
//{
//    Q_ASSERT(std::size_t(list.size()) <= array.size());
//    std::transform(
//        list.cbegin(), list.cend(), array.begin(), [](const QString &str) { return QVariant(str).value<T>(); });
//    return array;
//}

// template <typename T, std::size_t N> std::array<T, N> inline operator<<(std::array<T, N> &array, const QString str)
//{
//    const auto list = str.split(',');
//    return (array << list);
//}

namespace S2DataTypes
{

class DataItem
{
public:
    DataItem() = default;
    DataItem(quint16 _id);
    DataItem(quint16 _id, const valueType &value);
    [[deprecated]] DataItem(const DataRec &record);
    [[deprecated]] DataItem(const DataRec &record, const char *rawdata);
    [[deprecated]] DataItem(quint16 _id, const QString &str);

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

    template <typename T, std::enable_if_t<isValueType<T>::value, bool> = true> T value() const
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

    //    template <typename T> //
    //    void helper(unsigned int numByte, const char *rawdata)
    //    {
    //        assert(sizeof(T) == numByte);
    //        data = *reinterpret_cast<const T *>(rawdata);
    //    }

    //    template <typename T, std::enable_if_t<std_ext::is_container<T>::value, bool> = true> //
    //    void helper(const QString &str)
    //    {
    //        T arr {};
    //        arr << str;
    //        data = arr;
    //    }

    //    template <typename T, std::enable_if_t<!std_ext::is_container<T>::value, bool> = true> //
    //    void helper(const QString &str)
    //    {
    //        data = QVariant(str).value<T>();
    //    }
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
