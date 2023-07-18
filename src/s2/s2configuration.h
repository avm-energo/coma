#pragma once

#include "s2datafactory.h"
#include "s2util.h"

namespace S2
{

class Configuration
{
private:
    const S2ConfigStorage &s2confStorage;
    S2DataFactory factory;
    S2Util util;
    std::map<quint32, DataItem> data;

    quint32 getIdByName(const QString &name) const noexcept;

public:
    /// \brief Итератор на хранимые данные.
    using Iter = decltype(data)::iterator;
    /// \brief Константный итератор на хранимые данные.
    using ConstIter = decltype(data)::const_iterator;

    /// \brief Конструктор класса.
    explicit Configuration(const S2ConfigStorage &storage);
    /// \brief Перегрузка оператора присваивания.
    const Configuration &operator=(const Configuration &rhs);

    /// \brief Перегрузка оператора индексирования, индекс - ID записи S2.
    /// \returns Возвращает неконстантную ссылку на запись S2.
    [[nodiscard]] DataItem &operator[](const quint32 id);
    /// \brief Перегрузка оператора индексирования, индекс - ID записи S2.
    /// \returns Возвращает константную ссылку на запись S2.
    [[nodiscard]] const DataItem &operator[](const quint32 id) const;
    /// \brief Перегрузка оператора индексирования, индекс - имя записи S2.
    /// \returns Возвращает неконстантную ссылку на запись S2.
    [[nodiscard]] DataItem &operator[](const QString &name);
    /// \brief Перегрузка оператора индексирования, индекс - имя записи S2.
    /// \returns Возвращает константную ссылку на запись S2.
    [[nodiscard]] const DataItem &operator[](const QString &name) const;

    /// \brief Возвращает итератор на начало хранимых данных.
    /// \details Нужно для range-based циклов for.
    [[nodiscard]] Iter begin() noexcept;
    /// \brief Возвращает итератор на конец хранимых данных.
    /// \details Нужно для range-based циклов for.
    [[nodiscard]] Iter end() noexcept;
    /// \brief Возвращает константный итератор на начало хранимых данных.
    /// \details Нужно для range-based циклов for.
    [[nodiscard]] ConstIter begin() const noexcept;
    /// \brief Возвращает константный итератор на конец хранимых данных.
    /// \details Нужно для range-based циклов for.
    [[nodiscard]] ConstIter end() const noexcept;

    /// \brief Возвращает ссылку на хранилище с конфигурацией S2.
    const S2ConfigStorage &getConfigStorage() const noexcept;

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

    void setRecord(const quint32 id, const DataItem &record);
    void setRecord(const quint32 id, const valueType &value);

    /// \brief Конвертация хранимых данных согласно формату S2 в массив байт.
    [[nodiscard]] QByteArray toByteArray() const;
    /// \brief Парсинг бинарного файла согласно формату S2 в хранилище данных.
    /// \param rawData[in] - полученный бинарный файл.
    bool updateByRawData(const QByteArray &rawData);

    /// \brief Сравнение двух конфигураций
    /// \returns Возвращает вектор с ID элементов, которые отличаются в
    /// текущей (this) и переданной (rhs) конфигурациях.
    std::vector<quint32> checkDiff(const Configuration &rhs) const;
};

} // namespace S2

using S2Configuration = S2::Configuration;
