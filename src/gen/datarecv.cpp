#include "datarecv.h"

#include "datatypes.h"

std::map<int, ctti::unnamed_type_id_t> DataTypes::DataRecV::map;

namespace DataTypes
{

DataRecV DataTypes::DataRecV::deserialize(const S2DataTypes::DataRec &record)
{
    using namespace detail;
    DataRecV newRec;
    newRec.id = record.id;
    auto value = map.at(record.id);
    switch (value.hash())
    {
    case ctti::unnamed_type_id<BYTE>().hash():
    {
        newRec.data = *reinterpret_cast<BYTE *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<WORD>().hash():
    {
        newRec.data = *reinterpret_cast<WORD *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<DWORD>().hash():
    {
        newRec.data = *reinterpret_cast<DWORD *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<BYTE_4t>().hash():
    {
        newRec.data = *reinterpret_cast<BYTE_4t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<WORD_4t>().hash():
    {
        newRec.data = *reinterpret_cast<WORD_4t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<BYTE_8t>().hash():
    {
        newRec.data = *reinterpret_cast<BYTE_8t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<WORD_8t>().hash():
    {
        newRec.data = *reinterpret_cast<WORD_8t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<BYTE_16t>().hash():
    {
        newRec.data = *reinterpret_cast<BYTE_16t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<WORD_16t>().hash():
    {
        newRec.data = *reinterpret_cast<WORD_16t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<float>().hash():
    {
        newRec.data = *reinterpret_cast<float *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_2t>().hash():
    {
        newRec.data = *reinterpret_cast<FLOAT_2t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_2t_2t>().hash():
    {
        newRec.data = *reinterpret_cast<FLOAT_2t_2t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_3t>().hash():
    {
        newRec.data = *reinterpret_cast<FLOAT_3t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_6t>().hash():
    {
        newRec.data = *reinterpret_cast<FLOAT_6t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_8t>().hash():
    {
        newRec.data = *reinterpret_cast<FLOAT_8t *>(record.thedata);
        break;
    }
    default:
        assert(false && "Unknown type id");
    }
    return newRec;
}

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
            //  std::cout << data.index() << std::endl;
            S2DataTypes::DataRec record { id, sizeof(arg), (void *)(&arg) };
            return record;
        },
        data);
}

DataRecV::DataRecV(const S2DataTypes::DataRec &record) : id(record.id)
{
    using namespace detail;

    auto search = map.find(id);
    if (search == map.end())
        return;

    auto value = map.at(record.id);
    switch (value.hash())
    {
    case ctti::unnamed_type_id<BYTE>().hash():
    {
        assert(sizeof(BYTE) == record.numByte);
        data = *reinterpret_cast<BYTE *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<WORD>().hash():
    {
        assert(sizeof(WORD) == record.numByte);
        data = *reinterpret_cast<WORD *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<DWORD>().hash():
    {
        assert(sizeof(DWORD) == record.numByte);
        data = *reinterpret_cast<DWORD *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<BYTE_4t>().hash():
    {
        assert(sizeof(BYTE_4t) == record.numByte);
        data = *reinterpret_cast<BYTE_4t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<WORD_4t>().hash():
    {
        assert(sizeof(WORD_4t) == record.numByte);
        data = *reinterpret_cast<WORD_4t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<BYTE_8t>().hash():
    {
        assert(sizeof(BYTE_8t) == record.numByte);
        data = *reinterpret_cast<BYTE_8t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<WORD_8t>().hash():
    {
        assert(sizeof(WORD_8t) == record.numByte);
        data = *reinterpret_cast<WORD_8t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<BYTE_16t>().hash():
    {
        assert(sizeof(BYTE_16t) == record.numByte);
        data = *reinterpret_cast<BYTE_16t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<WORD_16t>().hash():
    {
        assert(sizeof(WORD_16t) == record.numByte);
        data = *reinterpret_cast<WORD_16t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<float>().hash():
    {
        assert(sizeof(float) == record.numByte);
        data = *reinterpret_cast<float *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_2t>().hash():
    {
        assert(sizeof(FLOAT_2t) == record.numByte);
        data = *reinterpret_cast<FLOAT_2t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_2t_2t>().hash():
    {
        assert(sizeof(FLOAT_2t_2t) == record.numByte);
        data = *reinterpret_cast<FLOAT_2t_2t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_3t>().hash():
    {
        assert(sizeof(FLOAT_3t) == record.numByte);
        data = *reinterpret_cast<FLOAT_3t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_6t>().hash():
    {
        assert(sizeof(FLOAT_6t) == record.numByte);
        data = *reinterpret_cast<FLOAT_6t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_8t>().hash():
    {
        assert(sizeof(FLOAT_8t) == record.numByte);
        data = *reinterpret_cast<FLOAT_8t *>(record.thedata);
        break;
    }
    default:
        assert(false && "Unknown type id");
    }
}

DataRecV::DataRecV(const S2DataTypes::DataRec &record, const char *rawdata) : id(record.id)
{
    using namespace detail;

    auto search = map.find(id);
    assert(search != map.end());
    // return;
    // Exception inside ctor https://www.stroustrup.com/bs_faq2.html#ctor-exceptions
    auto value = map.at(record.id);
    switch (value.hash())
    {
    case ctti::unnamed_type_id<BYTE>().hash():
    {
        assert(sizeof(BYTE) == record.numByte);
        data = *reinterpret_cast<const BYTE *>(rawdata);
        break;
    }
    case ctti::unnamed_type_id<WORD>().hash():
    {
        assert(sizeof(WORD) == record.numByte);
        data = *reinterpret_cast<const WORD *>(rawdata);
        break;
    }
    case ctti::unnamed_type_id<DWORD>().hash():
    {
        assert(sizeof(DWORD) == record.numByte);
        data = *reinterpret_cast<const DWORD *>(rawdata);
        break;
    }
    case ctti::unnamed_type_id<BYTE_4t>().hash():
    {
        assert(sizeof(BYTE_4t) == record.numByte);
        data = *reinterpret_cast<const BYTE_4t *>(rawdata);
        break;
    }
    case ctti::unnamed_type_id<WORD_4t>().hash():
    {
        assert(sizeof(WORD_4t) == record.numByte);
        data = *reinterpret_cast<const WORD_4t *>(rawdata);
        break;
    }
    case ctti::unnamed_type_id<BYTE_8t>().hash():
    {
        assert(sizeof(BYTE_8t) == record.numByte);
        data = *reinterpret_cast<const BYTE_8t *>(rawdata);
        break;
    }
    case ctti::unnamed_type_id<WORD_8t>().hash():
    {
        assert(sizeof(WORD_8t) == record.numByte);
        data = *reinterpret_cast<const WORD_8t *>(rawdata);
        break;
    }
    case ctti::unnamed_type_id<BYTE_16t>().hash():
    {
        assert(sizeof(BYTE_16t) == record.numByte);
        data = *reinterpret_cast<const BYTE_16t *>(rawdata);
        break;
    }
    case ctti::unnamed_type_id<WORD_16t>().hash():
    {
        assert(sizeof(WORD_16t) == record.numByte);
        data = *reinterpret_cast<const WORD_16t *>(rawdata);
        break;
    }
    case ctti::unnamed_type_id<float>().hash():
    {
        assert(sizeof(float) == record.numByte);
        data = *reinterpret_cast<const float *>(rawdata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_2t>().hash():
    {
        assert(sizeof(FLOAT_2t) == record.numByte);
        data = *reinterpret_cast<const FLOAT_2t *>(rawdata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_2t_2t>().hash():
    {
        assert(sizeof(FLOAT_2t_2t) == record.numByte);
        data = *reinterpret_cast<const FLOAT_2t_2t *>(rawdata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_3t>().hash():
    {
        assert(sizeof(FLOAT_3t) == record.numByte);
        data = *reinterpret_cast<const FLOAT_3t *>(rawdata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_6t>().hash():
    {
        assert(sizeof(FLOAT_6t) == record.numByte);
        data = *reinterpret_cast<const FLOAT_6t *>(rawdata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_8t>().hash():
    {
        assert(sizeof(FLOAT_8t) == record.numByte);
        data = *reinterpret_cast<const FLOAT_8t *>(rawdata);
        break;
    }
    default:
        assert(false && "Unknown type id");
    }
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
}
bool S2DataTypes::is_same(const S2DataTypes::DataRec &lhs, const S2DataTypes::DataRec &rhs)
{
    if ((lhs.id == rhs.id) && (lhs.numByte == rhs.numByte))
        return !memcmp(lhs.thedata, rhs.thedata, lhs.numByte);

    return false;
}
