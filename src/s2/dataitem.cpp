#include "dataitem.h"

#include "../module/configstorage.h"

#include <type_traits>

namespace S2DataTypes
{

DataItem::DataItem(quint16 _id) : DataItem(_id, 0)
{
}

DataItem::DataItem(quint16 _id, const valueType &value) : id(_id), data(value)
{
}

DataItem::DataItem(const DataRec &record, const char *rawdata) : id(record.header.id)
{
    //    auto &s2map = ConfigStorage::GetInstance().getS2Map();
    //    auto search = s2map.find(id);
    //    assert(search != s2map.end());

    //    // Exception inside ctor https://www.stroustrup.com/bs_faq2.html#ctor-exceptions
    //    auto value = s2map.at(record.header.id);
    //    switch (value.hash())
    //    {
    //    case ctti::unnamed_type_id<BYTE>().hash():
    //        helper<BYTE>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<WORD>().hash():
    //        helper<WORD>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<DWORD>().hash():
    //        helper<DWORD>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<INT32>().hash():
    //        helper<INT32>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<BYTE_4t>().hash():
    //        helper<BYTE_4t>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<WORD_4t>().hash():
    //        helper<WORD_4t>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<DWORD_4t>().hash():
    //        helper<DWORD_4t>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<BYTE_8t>().hash():
    //        helper<BYTE_8t>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<WORD_8t>().hash():
    //        helper<WORD_8t>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<DWORD_8t>().hash():
    //        helper<DWORD_8t>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<BYTE_16t>().hash():
    //        helper<BYTE_16t>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<WORD_16t>().hash():
    //        helper<WORD_16t>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<DWORD_16t>().hash():
    //        helper<DWORD_16t>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<BYTE_32t>().hash():
    //        helper<BYTE_32t>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<WORD_32t>().hash():
    //        helper<WORD_32t>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<DWORD_32t>().hash():
    //        helper<DWORD_32t>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<FLOAT>().hash():
    //        helper<FLOAT>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<FLOAT_2t>().hash():
    //        helper<FLOAT_2t>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<FLOAT_3t>().hash():
    //        helper<FLOAT_3t>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<FLOAT_4t>().hash():
    //        helper<FLOAT_4t>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<FLOAT_6t>().hash():
    //        helper<FLOAT_6t>(record.header.numByte, rawdata);
    //        break;
    //    case ctti::unnamed_type_id<FLOAT_8t>().hash():
    //        helper<FLOAT_8t>(record.header.numByte, rawdata);
    //        break;
    //    default:
    //        assert(false && "Unknown type id");
    //    }
}

DataItem::DataItem(quint16 _id, const QString &str) : id(_id)
{
    //    auto &s2map = ConfigStorage::GetInstance().getS2Map();
    //    auto search = s2map.find(_id);
    //    assert(search != s2map.end());

    //    // Exception inside ctor https://www.stroustrup.com/bs_faq2.html#ctor-exceptions
    //    auto value = s2map.at(_id);
    //    switch (value.hash())
    //    {
    //    case ctti::unnamed_type_id<BYTE>().hash():
    //        helper<BYTE>(str);
    //        break;
    //    case ctti::unnamed_type_id<WORD>().hash():
    //        helper<WORD>(str);
    //        break;
    //    case ctti::unnamed_type_id<DWORD>().hash():
    //        helper<DWORD>(str);
    //        break;
    //    case ctti::unnamed_type_id<INT32>().hash():
    //        helper<INT32>(str);
    //        break;
    //    case ctti::unnamed_type_id<BYTE_4t>().hash():
    //        helper<BYTE_4t>(str);
    //        break;
    //    case ctti::unnamed_type_id<WORD_4t>().hash():
    //        helper<WORD_4t>(str);
    //        break;
    //    case ctti::unnamed_type_id<DWORD_4t>().hash():
    //        helper<DWORD_4t>(str);
    //        break;
    //    case ctti::unnamed_type_id<BYTE_6t>().hash():
    //        helper<BYTE_6t>(str);
    //        break;
    //    case ctti::unnamed_type_id<WORD_6t>().hash():
    //        helper<WORD_6t>(str);
    //        break;
    //    case ctti::unnamed_type_id<DWORD_6t>().hash():
    //        helper<DWORD_6t>(str);
    //        break;
    //    case ctti::unnamed_type_id<BYTE_8t>().hash():
    //        helper<BYTE_8t>(str);
    //        break;
    //    case ctti::unnamed_type_id<WORD_8t>().hash():
    //        helper<WORD_8t>(str);
    //        break;
    //    case ctti::unnamed_type_id<DWORD_8t>().hash():
    //        helper<DWORD_8t>(str);
    //        break;
    //    case ctti::unnamed_type_id<BYTE_16t>().hash():
    //        helper<BYTE_16t>(str);
    //        break;
    //    case ctti::unnamed_type_id<WORD_16t>().hash():
    //        helper<WORD_16t>(str);
    //        break;
    //    case ctti::unnamed_type_id<DWORD_16t>().hash():
    //        helper<DWORD_16t>(str);
    //        break;
    //    case ctti::unnamed_type_id<BYTE_32t>().hash():
    //        helper<BYTE_32t>(str);
    //        break;
    //    case ctti::unnamed_type_id<WORD_32t>().hash():
    //        helper<WORD_32t>(str);
    //        break;
    //    case ctti::unnamed_type_id<DWORD_32t>().hash():
    //        helper<DWORD_32t>(str);
    //        break;
    //    case ctti::unnamed_type_id<float>().hash():
    //        helper<FLOAT>(str);
    //        break;
    //    case ctti::unnamed_type_id<FLOAT_2t>().hash():
    //        helper<FLOAT_2t>(str);
    //        break;
    //    case ctti::unnamed_type_id<FLOAT_3t>().hash():
    //        helper<FLOAT_3t>(str);
    //        break;
    //    case ctti::unnamed_type_id<FLOAT_4t>().hash():
    //        helper<FLOAT_4t>(str);
    //        break;
    //    case ctti::unnamed_type_id<FLOAT_6t>().hash():
    //        helper<FLOAT_6t>(str);
    //        break;
    //    case ctti::unnamed_type_id<FLOAT_8t>().hash():
    //        helper<FLOAT_8t>(str);
    //        break;
    //    default:
    //        assert(false && "Unknown type id");
    //    }
}

DataItem::DataItem(const DataRec &record) : DataItem(record, static_cast<const char *>(record.thedata))
{
}

bool operator==(const DataItem &lhs, const DataItem &rhs)
{
    return (lhs.id == rhs.id) && (lhs.data == rhs.data);
}

bool operator!=(const DataItem &lhs, const DataItem &rhs)
{
    return !(lhs == rhs);
}

DataRec DataItem::serialize() const
{
    return std::visit(
        [=](auto &arg) -> DataRec {
            using internalType = std::remove_reference_t<decltype(arg)>;
            DataRec record { { id, sizeof(internalType) }, (void *)(&arg) };
            return record;
        },
        data);
}

quint16 DataItem::getId() const
{
    return id;
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

size_t DataItem::typeIndex() const
{
    return data.index();
}
}
