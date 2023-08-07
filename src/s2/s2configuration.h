#pragma once

#include "s2datafactory.h"
#include "s2util.h"

namespace S2
{

/// \brief Класс для хранения конфигурации устройства/модуля/платы в формате S2.
/// \details Конфигурация представляет собой контейнер, в котором по идентификаторам
/// хранятся отдельные элементы данных, представляющие собой записи в формате S2.
/// \see S2::DataItem.
class Configuration
{
private:
    const S2ConfigStorage &m_storage;
    S2DataFactory m_factory;
    S2Util m_util;
    std::map<quint32, DataItem> m_data;

    /// \brief Возвращает ID S2 записи по её имени.
    quint32 getIdByName(const QString &name) const noexcept;

public:
    /// \brief Итератор на хранимые данные.
    using Iter = decltype(m_data)::iterator;
    /// \brief Константный итератор на хранимые данные.
    using ConstIter = decltype(m_data)::const_iterator;

    /// \brief Конструктор.
    explicit Configuration(const S2ConfigStorage &storage);
    /// \brief Копирующий конструктор.
    explicit Configuration(const Configuration &rhs);

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

    /// \brief Функция для добавления элемента в конфигурацию.
    /// \returns Возвращает статус добавления нового элемента в контейнер.
    bool append(const S2::DataRec &record);
    /// \brief Функция для добавления элемента в конфигурацию.
    /// \returns Возвращает статус добавления нового элемента в контейнер.
    bool append(const quint32 id, const QByteArray &bytes);
    /// \brief Функция для добавления элемента в конфигурацию.
    /// \returns Возвращает статус добавления нового элемента в контейнер.
    bool append(const quint32 id, const QString &str);
    /// \brief Функция для добавления элемента в конфигурацию.
    /// \returns Возвращает статус добавления нового элемента в контейнер.
    template <typename T> //
    bool append(const QString &name, const T &value)
    {
        auto id = getIdByName(name);
        const auto [_, success] = m_data.insert({ id, m_factory.create(id, value) });
        Q_UNUSED(_);
        return success;
    }

    /// \brief Функция для добавления элемента в конфигурацию.
    /// \details Если элемент с указанным id уже существует, то его данные заменяются переданными.
    void setRecord(const quint32 id, const DataItem &record);
    /// \brief Функция для добавления элемента в конфигурацию.
    /// \details Если элемент с указанным id уже существует, то его данные заменяются переданными.
    void setRecord(const quint32 id, const valueType &value);
    /// \brief Функция для добавления элемента в конфигурацию.
    /// \details Если элемент с указанным id уже существует, то его данные заменяются переданными.
    void setRecord(const QString &name, const valueType &value);

    bool contains(const quint32 id) const noexcept;
    bool contains(const QString &name) const noexcept;

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
