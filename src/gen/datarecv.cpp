#include "datarecv.h"

#include "datatypes.h"
#include "s2helper.h"

#include <type_traits>
std::map<int, ctti::unnamed_type_id_t> DataTypes::DataRecV::map;

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
            //  std::cout << data.index() << std::endl;
            typedef std::remove_reference_t<decltype(arg)> internalType;
            S2DataTypes::DataRec record { id, sizeof(internalType), (void *)(&arg) };
            return record;
        },
        data);
}

DataRecV::DataRecV(const S2DataTypes::DataRec &record) : DataRecV(record, static_cast<const char *>(record.thedata))
{
}

DataRecV::DataRecV(const S2DataTypes::DataRec &record, const char *rawdata) : id(record.id)
{
    using namespace detail;

    auto search = map.find(id);
    assert(search != map.end());

    // Exception inside ctor https://www.stroustrup.com/bs_faq2.html#ctor-exceptions
    auto value = map.at(record.id);
    switch (value.hash())
    {
    case ctti::unnamed_type_id<BYTE>().hash():
    {
        Q_ASSERT(sizeof(BYTE) == record.numByte);
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
    case ctti::unnamed_type_id<INT32>().hash():
    {
        assert(sizeof(INT32) == record.numByte);
        data = *reinterpret_cast<const INT32 *>(rawdata);
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
    case ctti::unnamed_type_id<DWORD_4t>().hash():
    {
        assert(sizeof(DWORD_4t) == record.numByte);
        data = *reinterpret_cast<const DWORD_4t *>(rawdata);
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
    case ctti::unnamed_type_id<DWORD_8t>().hash():
    {
        assert(sizeof(DWORD_8t) == record.numByte);
        data = *reinterpret_cast<const DWORD_8t *>(rawdata);
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
    case ctti::unnamed_type_id<DWORD_16t>().hash():
    {
        assert(sizeof(DWORD_16t) == record.numByte);
        data = *reinterpret_cast<const DWORD_16t *>(rawdata);
        break;
    }
    case ctti::unnamed_type_id<BYTE_32t>().hash():
    {
        assert(sizeof(BYTE_32t) == record.numByte);
        data = *reinterpret_cast<const BYTE_32t *>(rawdata);
        break;
    }
    case ctti::unnamed_type_id<WORD_32t>().hash():
    {
        assert(sizeof(WORD_32t) == record.numByte);
        data = *reinterpret_cast<const WORD_32t *>(rawdata);
        break;
    }
    case ctti::unnamed_type_id<DWORD_32t>().hash():
    {
        assert(sizeof(DWORD_32t) == record.numByte);
        data = *reinterpret_cast<const DWORD_32t *>(rawdata);
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

template <typename T, size_t N> std::array<T, N> operator<<(std::array<T, N> &array, const QStringList list)
{
    Q_ASSERT(size_t(list.size()) <= array.size());
    std::transform(
        list.cbegin(), list.cend(), array.begin(), [](const QString str) { return QVariant(str).value<T>(); });
    return array;
}

template <typename T, size_t N1, size_t N2>
auto operator<<(std::array<std::array<T, N2>, N1> &array, const QStringList list) -> decltype(array)
{
    Q_ASSERT(size_t(list.size()) <= (N1 * N2));
    T *ptr = reinterpret_cast<T *>(array.data());
    for (auto i = 0; i != (N1 * N2); ++i)
    {
        *ptr = QVariant(list.at(i)).value<T>();
    }
    return array;
}

template <typename T, size_t N> std::array<T, N> operator<<(std::array<T, N> &array, const QString str)
{
    const auto list = str.split(',');
    return (array << list);
}

DataRecV::DataRecV(const unsigned _id, const QString &str) : id(_id)
{
    using namespace detail;

    auto search = map.find(_id);
    assert(search != map.end());
    // return;
    // Exception inside ctor https://www.stroustrup.com/bs_faq2.html#ctor-exceptions

    auto value = map.at(_id);
    switch (value.hash())
    {
    case ctti::unnamed_type_id<BYTE>().hash():
    {
        data = QVariant(str).value<BYTE>();
        break;
    }
    case ctti::unnamed_type_id<WORD>().hash():
    {
        data = QVariant(str).value<WORD>();
        break;
    }
    case ctti::unnamed_type_id<DWORD>().hash():
    {
        data = QVariant(str).value<DWORD>();
        break;
    }
    case ctti::unnamed_type_id<INT32>().hash():
    {
        data = QVariant(str).value<INT32>();
        break;
    }
    case ctti::unnamed_type_id<BYTE_4t>().hash():
    {
        BYTE_4t arr {};
        arr << str;
        data = arr;
        break;
    }
    case ctti::unnamed_type_id<WORD_4t>().hash():
    {
        WORD_4t arr {};
        arr << str;
        data = arr;
        break;
    }
    case ctti::unnamed_type_id<DWORD_4t>().hash():
    {
        DWORD_4t arr {};
        arr << str;
        data = arr;
        break;
    }
    case ctti::unnamed_type_id<BYTE_8t>().hash():
    {
        BYTE_8t arr {};
        arr << str;
        data = arr;
        break;
    }
    case ctti::unnamed_type_id<WORD_8t>().hash():
    {
        WORD_8t arr {};
        arr << str;
        data = arr;
        break;
    }
    case ctti::unnamed_type_id<DWORD_8t>().hash():
    {
        DWORD_8t arr {};
        arr << str;
        data = arr;
        break;
    }
    case ctti::unnamed_type_id<BYTE_16t>().hash():
    {
        BYTE_16t arr {};
        arr << str;
        data = arr;
        break;
    }
    case ctti::unnamed_type_id<WORD_16t>().hash():
    {
        WORD_16t arr {};
        arr << str;
        data = arr;
        break;
    }
    case ctti::unnamed_type_id<DWORD_16t>().hash():
    {
        DWORD_16t arr {};
        arr << str;
        data = arr;
        break;
    }
    case ctti::unnamed_type_id<BYTE_32t>().hash():
    {
        BYTE_32t arr {};
        arr << str;
        data = arr;
        break;
    }
    case ctti::unnamed_type_id<WORD_32t>().hash():
    {
        WORD_32t arr {};
        arr << str;
        data = arr;
        break;
    }
    case ctti::unnamed_type_id<DWORD_32t>().hash():
    {
        DWORD_32t arr {};
        arr << str;
        data = arr;
        break;
    }
    case ctti::unnamed_type_id<float>().hash():
    {
        data = QVariant(str).value<float>();
        break;
    }
    case ctti::unnamed_type_id<FLOAT_2t>().hash():
    {
        FLOAT_2t arr {};
        arr << str;
        data = arr;
        break;
    }
    case ctti::unnamed_type_id<FLOAT_2t_2t>().hash():
    {
        FLOAT_2t_2t arr {};
        arr << str;
        data = arr;
        break;
    }
    case ctti::unnamed_type_id<FLOAT_3t>().hash():
    {
        FLOAT_3t arr {};
        arr << str;
        data = arr;
        break;
    }
    case ctti::unnamed_type_id<FLOAT_6t>().hash():
    {
        FLOAT_6t arr {};
        arr << str;
        data = arr;
        break;
    }
    case ctti::unnamed_type_id<FLOAT_8t>().hash():
    {
        FLOAT_8t arr {};
        arr << str;
        data = arr;
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
bool S2DataTypes::is_same(const S2DataTypes::DataRec &lhs, const S2DataTypes::DataRec &rhs)
{
    bool is_same_value = false;
    if ((lhs.id == rhs.id) && (lhs.numByte == rhs.numByte))
        is_same_value = !memcmp(lhs.thedata, rhs.thedata, lhs.numByte);

    Q_ASSERT(is_same_value);
    return is_same_value;
}
