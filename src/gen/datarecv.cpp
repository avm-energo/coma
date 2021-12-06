#include "datarecv.h"

#include "datatypes.h"
#include "s2helper.h"

#include <type_traits>
DataTypes::valueMap DataTypes::DataRecV::map;

namespace DataTypes
{

void DataRecV::printer() const
{
    std::cout << id << " : ";
    valueType w = data;
    std::visit([](auto arg) { detail::print(arg); }, w);
    std::cout << std::endl;
}

S2DataTypes::DataRec DataRecV::serialize() const
{
    return std::visit(
        [=](auto &arg) -> S2DataTypes::DataRec {
            typedef std::remove_reference_t<decltype(arg)> internalType;
            S2DataTypes::DataRec record { id, sizeof(internalType), (void *)(&arg) };
            return record;
        },
        data);
}

template <typename T, typename F> static constexpr bool is_variant_alternative()
{
    constexpr auto size = std::variant_size_v<F>;
    bool state = false;
    std_ext::for_constexpr<size>([&](auto index) {
        if constexpr (std::is_same_v<T, std::variant_alternative_t<index, F>>)
        {
            state = true;
        }
    });
    return state;
}

DataRecV::DataRecV(const valueMap &_map, const S2DataTypes::DataRec &record, const char *rawdata) : id(record.header.id)
{
    using namespace detail;

    auto search = _map.map().find(id);
    assert(search != _map.map().end());

    // Exception inside ctor https://www.stroustrup.com/bs_faq2.html#ctor-exceptions
    auto value = _map.map().at(record.header.id);
    switch (value.hash())
    {
    case ctti::unnamed_type_id<BYTE>().hash():
    {
        helper<BYTE>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<WORD>().hash():
    {
        helper<WORD>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<DWORD>().hash():
    {
        helper<DWORD>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<INT32>().hash():
    {
        helper<INT32>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<BYTE_4t>().hash():
    {
        helper<BYTE_4t>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<WORD_4t>().hash():
    {
        helper<WORD_4t>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<DWORD_4t>().hash():
    {
        helper<DWORD_4t>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<BYTE_8t>().hash():
    {
        helper<BYTE_8t>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<WORD_8t>().hash():
    {
        helper<WORD_8t>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<DWORD_8t>().hash():
    {
        helper<DWORD_8t>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<BYTE_16t>().hash():
    {
        helper<BYTE_16t>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<WORD_16t>().hash():
    {
        helper<WORD_16t>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<DWORD_16t>().hash():
    {
        helper<DWORD_16t>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<BYTE_32t>().hash():
    {
        helper<BYTE_32t>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<WORD_32t>().hash():
    {
        helper<WORD_32t>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<DWORD_32t>().hash():
    {
        helper<DWORD_32t>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<float>().hash():
    {
        helper<float>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_2t>().hash():
    {
        helper<FLOAT_2t>(record.header.numByte, rawdata, data);
        break;
    }
        //    case ctti::unnamed_type_id<FLOAT_2t_2t>().hash():
        //    {
        //        helper<FLOAT_2t_2t>(record.numByte, rawdata, data);
        //        break;
        //    }
    case ctti::unnamed_type_id<FLOAT_3t>().hash():
    {
        helper<FLOAT_3t>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_4t>().hash():
    {
        helper<FLOAT_4t>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_6t>().hash():
    {
        helper<FLOAT_6t>(record.header.numByte, rawdata, data);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_8t>().hash():
    {
        helper<FLOAT_8t>(record.header.numByte, rawdata, data);
        break;
    }
    default:
        assert(false && "Unknown type id");
    }
}

DataRecV::DataRecV(const valueMap &_map, const unsigned _id, const QString &str) : id(_id)
{
    using namespace detail;

    auto search = _map.map().find(_id);
    assert(search != _map.map().end());
    // return;
    // Exception inside ctor https://www.stroustrup.com/bs_faq2.html#ctor-exceptions

    auto value = _map.map().at(_id);
    switch (value.hash())
    {
    case ctti::unnamed_type_id<BYTE>().hash():
    {
        data = helper<BYTE>(str);
        break;
    }
    case ctti::unnamed_type_id<WORD>().hash():
    {
        data = helper<WORD>(str);
        break;
    }
    case ctti::unnamed_type_id<DWORD>().hash():
    {
        data = helper<DWORD>(str);
        break;
    }
    case ctti::unnamed_type_id<INT32>().hash():
    {
        data = helper<INT32>(str);
        break;
    }
    case ctti::unnamed_type_id<BYTE_4t>().hash():
    {
        data = helper<BYTE_4t>(str);
        break;
    }
    case ctti::unnamed_type_id<WORD_4t>().hash():
    {
        data = helper<WORD_4t>(str);
        break;
    }
    case ctti::unnamed_type_id<DWORD_4t>().hash():
    {
        data = helper<DWORD_4t>(str);
        break;
    }
    case ctti::unnamed_type_id<BYTE_8t>().hash():
    {
        data = helper<BYTE_8t>(str);
        break;
    }
    case ctti::unnamed_type_id<WORD_8t>().hash():
    {
        data = helper<WORD_8t>(str);
        break;
    }
    case ctti::unnamed_type_id<DWORD_8t>().hash():
    {
        data = helper<DWORD_8t>(str);
        break;
    }
    case ctti::unnamed_type_id<BYTE_16t>().hash():
    {
        data = helper<BYTE_16t>(str);
        break;
    }
    case ctti::unnamed_type_id<WORD_16t>().hash():
    {
        data = helper<WORD_16t>(str);
        break;
    }
    case ctti::unnamed_type_id<DWORD_16t>().hash():
    {
        data = helper<DWORD_16t>(str);
        break;
    }
    case ctti::unnamed_type_id<BYTE_32t>().hash():
    {
        data = helper<BYTE_32t>(str);
        break;
    }
    case ctti::unnamed_type_id<WORD_32t>().hash():
    {
        data = helper<WORD_32t>(str);
        break;
    }
    case ctti::unnamed_type_id<DWORD_32t>().hash():
    {
        data = helper<DWORD_32t>(str);
        break;
    }
    case ctti::unnamed_type_id<float>().hash():
    {
        data = helper<float>(str);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_2t>().hash():
    {
        data = helper<FLOAT_2t>(str);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_3t>().hash():
    {
        data = helper<FLOAT_3t>(str);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_4t>().hash():
    {
        data = helper<FLOAT_4t>(str);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_6t>().hash():
    {
        data = helper<FLOAT_6t>(str);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_8t>().hash():
    {
        data = helper<FLOAT_8t>(str);
        break;
    }
    default:
        assert(false && "Unknown type id");
    }
}

DataRecV::DataRecV(const unsigned _id) : DataRecV(_id, QString::number(0))
{
}

bool operator==(const DataTypes::DataRecV &lhs, const DataTypes::DataRecV &rhs)
{
    using namespace S2DataTypes;
    return (lhs.id == rhs.id) && (lhs.data == rhs.data);
}

bool operator!=(const DataTypes::DataRecV &lhs, const DataTypes::DataRecV &rhs)
{
    using namespace S2DataTypes;
    return !(lhs == rhs);
}

unsigned int DataRecV::getId() const
{
    return id;
}

valueType DataRecV::getData() const
{
    return data;
}

void DataRecV::setData(const valueType &value)
{
    // not true setter, only swapper for same internal types
    assert(data.index() == value.index());
    data = value;
}
}
