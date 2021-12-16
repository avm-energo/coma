#pragma once
#include "../ctti/type_id.hpp"
#include "s2helper.h"
#include "std_ext.h"

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

struct DataRecHeader
{
    // id
    quint32 id;
    // количество байт в TypeTheData
    quint32 numByte;
};

struct DataRec
{
    DataRecHeader header;
    void *thedata;
};
inline bool is_same(const S2DataTypes::DataRec &lhs, const S2DataTypes::DataRec &rhs)
{
    bool is_same_value = false;
    if ((lhs.header.id == rhs.header.id) && (lhs.header.numByte == rhs.header.numByte))
        is_same_value = !memcmp(lhs.thedata, rhs.thedata, lhs.header.numByte);

    Q_ASSERT(is_same_value);
    return is_same_value;
}

}

class S2;
class Module;
namespace DataTypes
{

class valueMap
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

    const auto &map() const
    {
        return m_map;
    }

private:
    value_type m_map;
};

class DataRecV
{
    friend class ::S2;
    friend class ::Module;

public:
    friend bool operator==(const DataRecV &lhs, const DataRecV &rhs);
    friend bool operator!=(const DataRecV &lhs, const DataRecV &rhs);
    DataRecV(const S2DataTypes::DataRec &record) : DataRecV(record, static_cast<const char *>(record.thedata))
    {
    }
    DataRecV(const valueMap &_map, const S2DataTypes::DataRec &record, const char *rawdata);

    DataRecV(const S2DataTypes::DataRec &record, const char *rawdata) : DataRecV(map, record, rawdata)
    {
    }
    DataRecV(const valueMap &_map, const unsigned _id, const QString &str);
    DataRecV(const unsigned _id, const QString &str) : DataRecV(map, _id, str)
    {
    }
    DataRecV(const unsigned _id);
    template <typename T, std::enable_if_t<valueMap::true_type<T>::value, bool> = true>
    DataRecV(unsigned _id, T _data) : id(_id), data(_data)
    {
    }
    DataRecV() = default;
    void printer() const;
    S2DataTypes::DataRec serialize() const;

    template <typename T, std::enable_if_t<valueMap::true_type<T>::value, bool> = true> T value() const
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
    static valueMap map;

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

bool operator==(const DataTypes::DataRecV &lhs, const DataTypes::DataRecV &rhs);
bool operator!=(const DataTypes::DataRecV &lhs, const DataTypes::DataRecV &rhs);
}
