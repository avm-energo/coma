#include "datarecv.h"

#include "datatypes.h"

std::map<int, ctti::unnamed_type_id_t> S2DataTypes::DataRecV::map;
S2DataTypes::DataRecV::DataRecV()
{
}

S2DataTypes::DataRecV S2DataTypes::DataRecV::deserialize(const S2DataTypes::DataRec &record)
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

void S2DataTypes::DataRecV::printer() const
{
    std::cout << id << std::endl;
    valueType w = data;
    std::visit([](auto arg) { detail::print(arg); }, w);
    std::cout << std::endl;
}

// struct helper{
//    S2DataTypes::DataRec operator()(auto &arg){

//    };
//};

S2DataTypes::DataRec S2DataTypes::DataRecV::serialize() const
{
    return std::visit(
        [=](auto &arg) -> DataRec {
            std::cout << data.index() << std::endl;
            DataRec record { id, sizeof(arg), (void *)(&arg) };
            return record;
        },
        data);
}

S2DataTypes::DataRecV::DataRecV(const S2DataTypes::DataRec &record) : id(record.id)
{
    using namespace detail;

    auto search2 = map.find(id);
    if (search2 == map.end())
        return;

    auto value = map.at(record.id);
    switch (value.hash())
    {
    case ctti::unnamed_type_id<BYTE>().hash():
    {
        assert(sizeof(BYTE) == record.num_byte);
        data = *reinterpret_cast<BYTE *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<WORD>().hash():
    {
        assert(sizeof(WORD) == record.num_byte);
        data = *reinterpret_cast<WORD *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<DWORD>().hash():
    {
        assert(sizeof(DWORD) == record.num_byte);
        data = *reinterpret_cast<DWORD *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<BYTE_4t>().hash():
    {
        //   assert(sizeof(BYTE_4t) == record.num_byte);
        data = *reinterpret_cast<BYTE_4t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<WORD_4t>().hash():
    {
        assert(sizeof(WORD_4t) == record.num_byte);
        data = *reinterpret_cast<WORD_4t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<BYTE_16t>().hash():
    {
        assert(sizeof(BYTE_16t) == record.num_byte);
        data = *reinterpret_cast<BYTE_16t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<WORD_16t>().hash():
    {
        assert(sizeof(WORD_16t) == record.num_byte);
        data = *reinterpret_cast<WORD_16t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<float>().hash():
    {
        assert(sizeof(float) == record.num_byte);
        data = *reinterpret_cast<float *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_2t>().hash():
    {
        assert(sizeof(FLOAT_2t) == record.num_byte);
        data = *reinterpret_cast<FLOAT_2t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_2t_2t>().hash():
    {
        assert(sizeof(FLOAT_2t_2t) == record.num_byte);
        data = *reinterpret_cast<FLOAT_2t_2t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_3t>().hash():
    {
        assert(sizeof(FLOAT_3t) == record.num_byte);
        data = *reinterpret_cast<FLOAT_3t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_6t>().hash():
    {
        assert(sizeof(FLOAT_6t) == record.num_byte);
        data = *reinterpret_cast<FLOAT_6t *>(record.thedata);
        break;
    }
    case ctti::unnamed_type_id<FLOAT_8t>().hash():
    {
        assert(sizeof(FLOAT_8t) == record.num_byte);
        data = *reinterpret_cast<FLOAT_8t *>(record.thedata);
        break;
    }
    default:
        assert(false && "Unknown type id");
    }
}

bool S2DataTypes::operator==(const S2DataTypes::DataRecV &lhs, const S2DataTypes::DataRecV &rhs)
{
    using namespace S2DataTypes;
    return (lhs.id == rhs.id) && (lhs.data == rhs.data);
}

bool S2DataTypes::operator!=(const S2DataTypes::DataRecV &lhs, const S2DataTypes::DataRecV &rhs)
{
    using namespace S2DataTypes;
    return !(lhs == rhs);
}

bool S2DataTypes::is_same(const S2DataTypes::DataRec &lhs, const S2DataTypes::DataRec &rhs)
{
    if ((lhs.id == rhs.id) && (lhs.num_byte == rhs.num_byte))
        return !memcmp(lhs.thedata, rhs.thedata, lhs.num_byte);
    else
        return false;
}
