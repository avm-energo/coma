#include "s2datafactory.h"

#include "s2configstorage.h"

namespace detail
{

template <typename T, size_t N> //
std::array<T, N> operator<<(std::array<T, N> &array, const QStringList &list)
{
    Q_ASSERT(std::size_t(list.size()) <= array.size());
    std::transform(list.cbegin(), list.cend(), array.begin(), //
        [](const QString &str) { return QVariant(str).value<T>(); });
    return array;
}

template <typename T> //
S2DataTypes::valueType helper(unsigned int numByte, const char *rawdata)
{
    assert(sizeof(T) == numByte);
    return *reinterpret_cast<const T *>(rawdata);
}

template <typename T, std::enable_if_t<std_ext::is_container<T>::value, bool> = true> //
S2DataTypes::valueType helper(const QString &str)
{
    T arr {};
    const auto list = str.split(',');
    arr << list;
    return arr;
}

template <typename T, std::enable_if_t<!std_ext::is_container<T>::value, bool> = true> //
S2DataTypes::valueType helper(const QString &str)
{
    return QVariant(str).value<T>();
}

}

namespace S2
{

DataFactory::DataFactory() : confStorage(S2::ConfigStorage::GetInstance())
{
}

ctti::unnamed_type_id_t DataFactory::getType(quint16 id) const
{
    auto &typeMap = confStorage.getTypeByIdMap();
    auto search = typeMap.find(id);
    assert(search != typeMap.cend());
    return search->second;
}

quint16 DataFactory::getId(const QString &name) const
{
    auto &nameMap = confStorage.getIdByNameMap();
    auto search = nameMap.find(name);
    assert(search != nameMap.cend());
    return search->second;
}

S2DataTypes::DataItem DataFactory::create(const S2DataTypes::DataRec &record) const
{
    using namespace detail;
    using namespace S2DataTypes;
    auto rawdata = static_cast<const char *>(record.thedata);
    auto id = static_cast<quint16>(record.header.id);
    auto type = getType(id);
    switch (type.hash())
    {
    case ctti::unnamed_type_id<BYTE>().hash():
        return { id, helper<BYTE>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<WORD>().hash():
        return { id, helper<WORD>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<DWORD>().hash():
        return { id, helper<DWORD>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<INT32>().hash():
        return { id, helper<INT32>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<BYTE_4t>().hash():
        return { id, helper<BYTE_4t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<WORD_4t>().hash():
        return { id, helper<WORD_4t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<DWORD_4t>().hash():
        return { id, helper<DWORD_4t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<BYTE_8t>().hash():
        return { id, helper<BYTE_8t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<WORD_8t>().hash():
        return { id, helper<WORD_8t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<DWORD_8t>().hash():
        return { id, helper<DWORD_8t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<BYTE_16t>().hash():
        return { id, helper<BYTE_16t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<WORD_16t>().hash():
        return { id, helper<WORD_16t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<DWORD_16t>().hash():
        return { id, helper<DWORD_16t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<BYTE_32t>().hash():
        return { id, helper<BYTE_32t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<WORD_32t>().hash():
        return { id, helper<WORD_32t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<DWORD_32t>().hash():
        return { id, helper<DWORD_32t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<FLOAT>().hash():
        return { id, helper<FLOAT>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<FLOAT_2t>().hash():
        return { id, helper<FLOAT_2t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<FLOAT_3t>().hash():
        return { id, helper<FLOAT_3t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<FLOAT_4t>().hash():
        return { id, helper<FLOAT_4t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<FLOAT_6t>().hash():
        return { id, helper<FLOAT_6t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<FLOAT_8t>().hash():
        return { id, helper<FLOAT_8t>(record.header.numByte, rawdata) };
    default:
        assert(false && "Unknown type id");
        return { id };
    }
}

S2DataTypes::DataItem DataFactory::create(const quint16 id, const QByteArray &data) const
{
    return create({ { id, quint32(data.size()) }, (void *)(data.data()) });
}

S2DataTypes::DataItem DataFactory::create(quint16 id, const QString &str) const
{
    using namespace detail;
    using namespace S2DataTypes;
    auto type = getType(id);
    switch (type.hash())
    {
    case ctti::unnamed_type_id<BYTE>().hash():
        return { id, helper<BYTE>(str) };
    case ctti::unnamed_type_id<WORD>().hash():
        return { id, helper<WORD>(str) };
    case ctti::unnamed_type_id<DWORD>().hash():
        return { id, helper<DWORD>(str) };
    case ctti::unnamed_type_id<INT32>().hash():
        return { id, helper<INT32>(str) };
    case ctti::unnamed_type_id<BYTE_4t>().hash():
        return { id, helper<BYTE_4t>(str) };
    case ctti::unnamed_type_id<WORD_4t>().hash():
        return { id, helper<WORD_4t>(str) };
    case ctti::unnamed_type_id<DWORD_4t>().hash():
        return { id, helper<DWORD_4t>(str) };
    case ctti::unnamed_type_id<BYTE_6t>().hash():
        return { id, helper<BYTE_6t>(str) };
    case ctti::unnamed_type_id<WORD_6t>().hash():
        return { id, helper<WORD_6t>(str) };
    case ctti::unnamed_type_id<DWORD_6t>().hash():
        return { id, helper<DWORD_6t>(str) };
    case ctti::unnamed_type_id<BYTE_8t>().hash():
        return { id, helper<BYTE_8t>(str) };
    case ctti::unnamed_type_id<WORD_8t>().hash():
        return { id, helper<WORD_8t>(str) };
    case ctti::unnamed_type_id<DWORD_8t>().hash():
        return { id, helper<DWORD_8t>(str) };
    case ctti::unnamed_type_id<BYTE_16t>().hash():
        return { id, helper<BYTE_16t>(str) };
    case ctti::unnamed_type_id<WORD_16t>().hash():
        return { id, helper<WORD_16t>(str) };
    case ctti::unnamed_type_id<DWORD_16t>().hash():
        return { id, helper<DWORD_16t>(str) };
    case ctti::unnamed_type_id<BYTE_32t>().hash():
        return { id, helper<BYTE_32t>(str) };
    case ctti::unnamed_type_id<WORD_32t>().hash():
        return { id, helper<WORD_32t>(str) };
    case ctti::unnamed_type_id<DWORD_32t>().hash():
        return { id, helper<DWORD_32t>(str) };
    case ctti::unnamed_type_id<FLOAT>().hash():
        return { id, helper<FLOAT>(str) };
    case ctti::unnamed_type_id<FLOAT_2t>().hash():
        return { id, helper<FLOAT_2t>(str) };
    case ctti::unnamed_type_id<FLOAT_3t>().hash():
        return { id, helper<FLOAT_3t>(str) };
    case ctti::unnamed_type_id<FLOAT_4t>().hash():
        return { id, helper<FLOAT_4t>(str) };
    case ctti::unnamed_type_id<FLOAT_6t>().hash():
        return { id, helper<FLOAT_6t>(str) };
    case ctti::unnamed_type_id<FLOAT_8t>().hash():
        return { id, helper<FLOAT_8t>(str) };
    default:
        assert(false && "Unknown type id");
        return { id };
    }
}

}
