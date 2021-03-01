#pragma once
#include "../ctti/type_id.hpp"
#include "s2helper.h"

#include <map>
namespace S2DataTypes
{
using valueTypeMap = std::map<int, ctti::unnamed_type_id_t>;

struct DataRec
{
    unsigned int id;
    unsigned int num_byte;
    void *thedata;
};
bool is_same(const S2DataTypes::DataRec &lhs, const S2DataTypes::DataRec &rhs);
// struct DataRec;

class DataRecV
{
public:
    friend bool operator==(const DataRecV &lhs, const DataRecV &rhs);
    friend bool operator!=(const DataRecV &lhs, const DataRecV &rhs);
    DataRecV(const S2DataTypes::DataRec &record);
    DataRecV();
    static S2DataTypes::DataRecV deserialize(const S2DataTypes::DataRec &record);
    void printer() const;
    DataRec serialize() const;
    static std::map<int, ctti::unnamed_type_id_t> map;
    template <typename T> static void addValue(int id)
    {
        map.insert({ id, ctti::unnamed_type_id<T>() });
    }
    unsigned int id;
    template <typename T> T value() const
    {
        assert(std::holds_alternative<T>(data));
        if (std::holds_alternative<T>(data))
            return std::get<T>(data);
        else
            return T();
    }

private:
    valueType data;
};

bool operator==(const S2DataTypes::DataRecV &lhs, const S2DataTypes::DataRecV &rhs);
bool operator!=(const S2DataTypes::DataRecV &lhs, const S2DataTypes::DataRecV &rhs);
}
