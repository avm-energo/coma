#pragma once

#include <ctti/type_id.hpp>
#include <s2/dataitem.h>
#include <s2/s2configstorage.h>

namespace S2
{

/// \brief Фабрика, создающая элементы конфигурации в формате S2.
/// \see S2::DataItem, S2::Configuration.
class DataFactory
{
private:
    const S2ConfigStorage &m_storage;

    /// \brief Возвращает хеш типа по его идентификатору.
    ctti::unnamed_type_id_t getType(const quint32 id) const;

public:
    explicit DataFactory(const S2ConfigStorage &confStorage);

    /// \brief Возвращает элемент конфигурации, созданный по переданным аргументам.
    [[nodiscard]] DataItem create(const DataRec &record) const;
    /// \brief Возвращает элемент конфигурации, созданный по переданным аргументам.
    [[nodiscard]] DataItem create(const quint32 id, const QByteArray &data) const;
    /// \brief Возвращает элемент конфигурации, созданный по переданным аргументам.
    [[nodiscard]] DataItem create(const quint32 id, const QString &str) const;
    /// \brief Возвращает элемент конфигурации, созданный по переданным аргументам.
    template <typename T, std::enable_if_t<isValueType<T>::value, bool> = true> //
    [[nodiscard]] DataItem create([[maybe_unused]] const quint32 id, const T &value) const
    {
        return DataItem { value };
    }
};

}

using S2DataFactory = S2::DataFactory;
