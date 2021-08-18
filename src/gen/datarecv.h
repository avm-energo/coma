#pragma once
#include "../ctti/type_id.hpp"
#include "s2helper.h"

#include <QVariant>
#include <cassert>
#include <map>

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

class QString;
namespace S2DataTypes
{
using valueTypeMap = std::map<int, ctti::unnamed_type_id_t>;

struct DataRec
{
    unsigned int id;
    unsigned int numByte;
    void *thedata;
};
bool is_same(const S2DataTypes::DataRec &lhs, const S2DataTypes::DataRec &rhs);

}
class S2;
namespace DataTypes
{
class DataRecV
{
    friend class ::S2;
    template <typename T> struct true_type
    {
        static constexpr bool value = std::is_variant_alternative<T, valueType>();
    };

public:
    friend bool operator==(const DataRecV &lhs, const DataRecV &rhs);
    friend bool operator!=(const DataRecV &lhs, const DataRecV &rhs);
    DataRecV(const S2DataTypes::DataRec &record);
    DataRecV(const S2DataTypes::DataRec &record, const char *rawdata);
    DataRecV(const unsigned _id, const QString &str);
    DataRecV(const unsigned _id);
    template <typename T
#if (_MSC_VER > 1924)
        ,
        std::enable_if_t<true_type<T>::value, bool> = true
#endif
        >
    DataRecV(unsigned _id, T _data) : id(_id), data(_data)
    {
    }
    DataRecV() = default;
    void printer() const;
    S2DataTypes::DataRec serialize() const;
    static std::map<int, ctti::unnamed_type_id_t> map;

    template <typename T
#if (_MSC_VER > 1924)
        ,
        std::enable_if_t<true_type<T>::value, bool> = true
#endif
        >
    static void addValue(int id)
    {
        map.insert({ id, ctti::unnamed_type_id<T>() });
    }

    template <typename T
#if (_MSC_VER > 1924)
        ,
        std::enable_if_t<true_type<T>::value, bool> = true
#endif
        >
    T value() const
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

    unsigned int getId() const;
    size_t typeIndex() const
    {
        return data.index();
    }

    valueType getData() const;

    void setData(const valueType &value);

private:
    unsigned int id;
    valueType data;

    template <typename T> void helper(unsigned int numByte, const char *rawdata, valueType &data)
    {
        constexpr auto hash = ctti::unnamed_type_id<T>().hash();
        assert(sizeof(T) == numByte);
        data = *reinterpret_cast<const T *>(rawdata);
    }
    template <typename T, std::enable_if_t<std::is_container<T>::value, bool> = true>
    valueType helper(const QString &str)
    {
        T arr {};
        arr << str;
        data = arr;
        return valueType(arr);
    }
    template <typename T, std::enable_if_t<!std::is_container<T>::value, bool> = true>
    valueType helper(const QString &str)
    {
        valueType data = QVariant(str).value<T>();
        return data;
    }
};

bool operator==(const DataTypes::DataRecV &lhs, const DataTypes::DataRecV &rhs);
bool operator!=(const DataTypes::DataRecV &lhs, const DataTypes::DataRecV &rhs);
}
