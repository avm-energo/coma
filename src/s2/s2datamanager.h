#pragma once

#include "s2configuration.h"

namespace S2
{

/// \brief Перечисление для типа хранимой конфигурации.
enum class BoardConfig : bool
{
    Base = false, ///< Хранится конфигурация базовой платы.
    Mezz = true   ///< Хранится конфигурация мезонинной платы.
};

/// \brief Структура для хранения конфигурации платы.
struct BoardConfiguration
{
    S2Configuration m_defaultConfig; ///< Конфигурация по умолчанию (из XML файла).
    S2Configuration m_workingConfig; ///< Рабочая конфигурация платы.
};

/// \brief Класс, который используется в качестве контейнера
/// для хранения конфигураций плат подключённого устройства.
class DataManager : public QObject
{
    Q_OBJECT
private:
    S2ConfigStorage &m_storage;
    std::map<BoardConfig, BoardConfiguration> m_data;
    BoardConfig m_currentParseTarget;

public:
    /// \brief Итератор на хранимые данные.
    using Iter = decltype(m_data)::iterator;
    /// \brief Константный итератор на хранимые данные.
    using ConstIter = decltype(m_data)::const_iterator;

    explicit DataManager(QObject *parent = nullptr);

    /// \brief Возвращает ссылку на хранилище с XML конфигурацией для работы с S2.
    [[nodiscard]] S2ConfigStorage &getStorage() noexcept;
    /// \brief Возвращает константную ссылку на хранилище с XML конфигурацией для работы с S2.
    [[nodiscard]] const S2ConfigStorage &getStorage() const noexcept;
    /// \brief Возвращает ссылку на текущую конфигурацию платы.
    [[nodiscard]] BoardConfiguration &getCurrentConfiguration() noexcept;
    /// \brief Возвращает константную ссылку на текущую конфигурацию платы.
    [[nodiscard]] const BoardConfiguration &getCurrentConfiguration() const noexcept;

    /// \brief Очищает mutable-данные контейнера.
    void clear() noexcept;
    /// \brief Возвращает true, если модуль состоит из одной платы, или является коробочным.
    bool isOneBoard() const noexcept;

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

public slots:
    /// \brief Слот, при вызове которого создаётся новая конфигурация для другого типа платы.
    void startNewConfig();
    /// \brief Слот, который сохраняет данные от XML парсера о конфигурации по умолчанию для текущей платы.
    void configDataReceive(const quint16 id, const QString &defVal, const bool visib, const quint16 count);
};

}

using S2BoardConfig = S2::BoardConfiguration;
using S2DataManager = S2::DataManager;