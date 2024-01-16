#pragma once

#include <s2/s2configuration.h>
#include <s2/s2util.h>

namespace S2
{

/// \brief Перечисление для типа хранимой конфигурации.
enum class BoardType : std::uint8_t
{
    Base = 0, ///< Хранится конфигурация базовой платы.
    Mezz      ///< Хранится конфигурация мезонинной платы.
};

/// \brief Структура для хранения конфигурации платы.
struct BoardConfiguration
{
    QString m_tabName;
    S2Configuration m_defaultConfig; ///< Конфигурация по умолчанию (из XML файла).
    S2Configuration m_workingConfig; ///< Рабочая конфигурация платы.

    void setDefaultConfig() noexcept
    {
        m_workingConfig = m_defaultConfig;
    }
};

/// \brief Класс, который используется в качестве контейнера
/// для хранения конфигураций плат подключённого устройства.
class DataManager final : public QObject
{
    Q_OBJECT
private:
    S2ConfigStorage &m_storage;
    std::map<BoardType, BoardConfiguration> m_data;
    S2Util m_util;
    BoardType m_currentParseTarget;

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
    /// \brief Возвращает ссылку на конфигурацию платы, тип которой указан.
    [[nodiscard]] BoardConfiguration &getConfiguration(const BoardType boardType) noexcept;
    /// \brief Возвращает константную ссылку на конфигурацию платы, тип которой указан.
    [[nodiscard]] const BoardConfiguration &getConfiguration(const BoardType boardType) const noexcept;

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

    void parseS2File(const QByteArray &rawFile);
    [[nodiscard]] QByteArray getBinaryConfiguration() const;

public slots:
    /// \brief Слот, при вызове которого создаётся новая конфигурация для другого типа платы.
    void startNewConfig();
    /// \brief Слот, который сохраняет данные от XML парсера о конфигурации по умолчанию для текущей платы.
    void configDataReceive(const quint16 id, const QString &defVal, const bool visib, const quint16 count);
    /// \brief aboba
    void configNameReceive(const QString &tabName);

signals:
    void parseStatus(const Error::Msg status);
};

}

using S2BoardType = S2::BoardType;
using S2BoardConfig = S2::BoardConfiguration;
using S2DataManager = S2::DataManager;
