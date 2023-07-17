#pragma once

#include "s2datafactory.h"
#include "s2util.h"

namespace S2
{

class Configuration
{
private:
    S2DataFactory factory;
    S2Util util;
    std::map<quint32, DataItem> data;

public:
    /// \brief Итератор на хранимые данные.
    using Iter = decltype(data)::iterator;
    /// \brief Константный итератор на хранимые данные.
    using ConstIter = decltype(data)::const_iterator;

    explicit Configuration(const S2ConfigStorage &storage);
    const Configuration &operator=(const Configuration &rhs);

    bool append(const S2::DataRec &record);
    bool append(const quint32 id, const QByteArray &bytes);
    bool append(const quint32 id, const QString &str);

    template <typename T> //
    bool append(const QString &name, const T &value)
    {
        auto id = util.getIdByName(name);
        const auto [it, success] = data.insert({ id, factory.create(id, value) });
        Q_UNUSED(it);
        return success;
    }

    /// \brief Возвращает итератор на начало хранимых данных.
    [[nodiscard]] Iter begin() noexcept;
    /// \brief Возвращает итератор на конец хранимых данных.
    [[nodiscard]] Iter end() noexcept;
    /// \brief Возвращает константный итератор на начало хранимых данных.
    [[nodiscard]] ConstIter begin() const noexcept;
    /// \brief Возвращает константный итератор на конец хранимых данных.
    [[nodiscard]] ConstIter end() const noexcept;

    void setRecord(const quint32 id, const DataItem &record);
    void setRecord(const quint32 id, const valueType &value);
    [[nodiscard]] const DataItem &getRecord(const quint32 id) const;
    [[nodiscard]] const DataItem &getRecord(const QString &name) const;

    /// \brief Сравнение двух конфигураций
    /// \returns Возвращает вектор с ID элементов, которые отличаются в
    /// текущей (this) и переданной (rhs) конфигурациях.
    std::vector<quint32> checkDiff(const Configuration &rhs) const;
};

} // namespace S2

using S2Configuration = S2::Configuration;
