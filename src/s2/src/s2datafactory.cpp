#include "s2/s2datafactory.h"

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
S2::valueType helper(unsigned int numByte, const char *rawdata)
{
    assert(sizeof(T) == numByte);
    return *reinterpret_cast<const T *>(rawdata);
}

template <typename T, std::enable_if_t<std_ext::is_container<T>::value, bool> = true> //
S2::valueType helper(const QString &str)
{
    T arr {};
    const auto list = str.split(',');
    arr << list;
    return arr;
}

template <typename T, std::enable_if_t<!std_ext::is_container<T>::value && std::is_arithmetic_v<T>, bool> = true> //
S2::valueType helper(const QString &str)
{
    return QVariant(str).value<T>();
}

template <typename T, std::enable_if_t<std::is_same_v<T, S2::CONFMAST>, bool> = true> //
S2::valueType helper(const QString &str)
{
    using namespace ModbusItem;
    Q_UNUSED(str);
    return S2::CONFMAST {
        SensorType::SEN_None,                       //
        { 0, Parity::NoParity, StopBits::OneStop }, //
        0, 0, TypeId::None, 0, 0, 0                 //
    };
}

} // namespace detail

namespace S2
{

DataFactory::DataFactory(const S2ConfigStorage &confStorage) : m_storage(confStorage)
{
}

ctti::unnamed_type_id_t DataFactory::getType(const quint32 id) const
{
    auto &typeMap = m_storage.getTypeByIdMap();
    auto search = typeMap.find(id);
    assert(search != typeMap.cend());
    return search->second;
}

DataItem DataFactory::create(const DataRec &record) const
{
    using namespace detail;
    auto rawdata = static_cast<const char *>(record.thedata);
    auto id = record.header.id;
    auto type = getType(id);
    switch (type.hash())
    {
    case ctti::unnamed_type_id<BYTE>().hash():
        return DataItem { helper<BYTE>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<WORD>().hash():
        return DataItem { helper<WORD>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<DWORD>().hash():
        return DataItem { helper<DWORD>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<INT32>().hash():
        return DataItem { helper<INT32>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<BYTE_4t>().hash():
        return DataItem { helper<BYTE_4t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<WORD_4t>().hash():
        return DataItem { helper<WORD_4t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<DWORD_4t>().hash():
        return DataItem { helper<DWORD_4t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<BYTE_6t>().hash():
        return DataItem { helper<BYTE_6t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<WORD_6t>().hash():
        return DataItem { helper<WORD_6t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<DWORD_6t>().hash():
        return DataItem { helper<DWORD_6t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<BYTE_8t>().hash():
        return DataItem { helper<BYTE_8t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<WORD_8t>().hash():
        return DataItem { helper<WORD_8t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<DWORD_8t>().hash():
        return DataItem { helper<DWORD_8t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<BYTE_16t>().hash():
        return DataItem { helper<BYTE_16t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<WORD_16t>().hash():
        return DataItem { helper<WORD_16t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<DWORD_16t>().hash():
        return DataItem { helper<DWORD_16t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<BYTE_32t>().hash():
        return DataItem { helper<BYTE_32t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<WORD_32t>().hash():
        return DataItem { helper<WORD_32t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<DWORD_32t>().hash():
        return DataItem { helper<DWORD_32t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<FLOAT>().hash():
        return DataItem { helper<FLOAT>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<FLOAT_2t>().hash():
        return DataItem { helper<FLOAT_2t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<FLOAT_3t>().hash():
        return DataItem { helper<FLOAT_3t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<FLOAT_4t>().hash():
        return DataItem { helper<FLOAT_4t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<FLOAT_6t>().hash():
        return DataItem { helper<FLOAT_6t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<FLOAT_8t>().hash():
        return DataItem { helper<FLOAT_8t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<CONF_DENS_3t>().hash():
        return DataItem { helper<CONF_DENS_3t>(record.header.numByte, rawdata) };
    case ctti::unnamed_type_id<CONFMAST>().hash():
        return DataItem { helper<CONFMAST>(record.header.numByte, rawdata) };
    default:
        assert(false && "Unknown type id");
        return DataItem {};
    }
}

DataItem DataFactory::create(const quint32 id, const QByteArray &data) const
{
    return create({ { id, quint32(data.size()) }, (void *)(data.data()) });
}

DataItem DataFactory::create(const quint32 id, const QString &str) const
{
    using namespace detail;
    auto type = getType(id);
    switch (type.hash())
    {
    case ctti::unnamed_type_id<BYTE>().hash():
        return DataItem { helper<BYTE>(str) };
    case ctti::unnamed_type_id<WORD>().hash():
        return DataItem { helper<WORD>(str) };
    case ctti::unnamed_type_id<DWORD>().hash():
        return DataItem { helper<DWORD>(str) };
    case ctti::unnamed_type_id<INT32>().hash():
        return DataItem { helper<INT32>(str) };
    case ctti::unnamed_type_id<BYTE_4t>().hash():
        return DataItem { helper<BYTE_4t>(str) };
    case ctti::unnamed_type_id<WORD_4t>().hash():
        return DataItem { helper<WORD_4t>(str) };
    case ctti::unnamed_type_id<DWORD_4t>().hash():
        return DataItem { helper<DWORD_4t>(str) };
    case ctti::unnamed_type_id<BYTE_6t>().hash():
        return DataItem { helper<BYTE_6t>(str) };
    case ctti::unnamed_type_id<WORD_6t>().hash():
        return DataItem { helper<WORD_6t>(str) };
    case ctti::unnamed_type_id<DWORD_6t>().hash():
        return DataItem { helper<DWORD_6t>(str) };
    case ctti::unnamed_type_id<BYTE_8t>().hash():
        return DataItem { helper<BYTE_8t>(str) };
    case ctti::unnamed_type_id<WORD_8t>().hash():
        return DataItem { helper<WORD_8t>(str) };
    case ctti::unnamed_type_id<DWORD_8t>().hash():
        return DataItem { helper<DWORD_8t>(str) };
    case ctti::unnamed_type_id<BYTE_16t>().hash():
        return DataItem { helper<BYTE_16t>(str) };
    case ctti::unnamed_type_id<WORD_16t>().hash():
        return DataItem { helper<WORD_16t>(str) };
    case ctti::unnamed_type_id<DWORD_16t>().hash():
        return DataItem { helper<DWORD_16t>(str) };
    case ctti::unnamed_type_id<BYTE_32t>().hash():
        return DataItem { helper<BYTE_32t>(str) };
    case ctti::unnamed_type_id<WORD_32t>().hash():
        return DataItem { helper<WORD_32t>(str) };
    case ctti::unnamed_type_id<DWORD_32t>().hash():
        return DataItem { helper<DWORD_32t>(str) };
    case ctti::unnamed_type_id<FLOAT>().hash():
        return DataItem { helper<FLOAT>(str) };
    case ctti::unnamed_type_id<FLOAT_2t>().hash():
        return DataItem { helper<FLOAT_2t>(str) };
    case ctti::unnamed_type_id<FLOAT_3t>().hash():
        return DataItem { helper<FLOAT_3t>(str) };
    case ctti::unnamed_type_id<FLOAT_4t>().hash():
        return DataItem { helper<FLOAT_4t>(str) };
    case ctti::unnamed_type_id<FLOAT_6t>().hash():
        return DataItem { helper<FLOAT_6t>(str) };
    case ctti::unnamed_type_id<FLOAT_8t>().hash():
        return DataItem { helper<FLOAT_8t>(str) };
    case ctti::unnamed_type_id<CONF_DENS_3t>().hash():
        return DataItem { helper<CONF_DENS_3t>(str) };
    case ctti::unnamed_type_id<CONFMAST>().hash():
        return DataItem { helper<CONFMAST>(str) };
    default:
        assert(false && "Unknown type id");
        return DataItem {};
    }
}

} // namespace S2
