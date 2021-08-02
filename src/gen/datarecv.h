#pragma once
#include "../ctti/type_id.hpp"
#include "s2helper.h"

#include <cassert>
#include <map>

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
};

bool operator==(const DataTypes::DataRecV &lhs, const DataTypes::DataRecV &rhs);
bool operator!=(const DataTypes::DataRecV &lhs, const DataTypes::DataRecV &rhs);
}
