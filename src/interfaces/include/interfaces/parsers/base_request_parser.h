#pragma once

#include <deque>
#include <gen/logclass.h>
#include <interfaces/types/protocol_settings.h>

namespace Interface
{

/// \brief Базовый класс парсера запросов к устройству.
class BaseRequestParser : public QObject
{
    Q_OBJECT
protected:
    static const std::map<Commands, CommandRegisters> s_wsCmdMap;

    ProtocolDescription m_protocol;
    QByteArray m_request;
    std::deque<QByteArray> m_longDataSections;
    bool m_isExceptionalSituation;
    std::uint32_t m_progressCount;

    template <typename GroupType> //
    GroupType getGroupsByAddress(const quint32 addr) const noexcept;

    /// \brief Устанавливает флаг исключительной ситуации.
    void setExceptionalSituationStatus(bool status) noexcept;

public:
    /// \brief Конструктор по-умолчанию.
    explicit BaseRequestParser(QObject *parent = nullptr);

    /// \brief Начальная инициализация настроек протокола обмена данными.
    virtual void basicProtocolSetup() noexcept = 0;
    /// \brief Обновление описания протокола парсера.
    void updateProtocolSettings(const ProtocolDescription &desc) noexcept;

    /// \brief Функция парсинга полученной команды в запрос к устройству.
    virtual QByteArray parse(const CommandStruct &command) = 0;
    /// \brief Возвращает команду для получения следующей
    /// секции файла или большого массива данных.
    virtual QByteArray getNextContinueCommand() noexcept = 0;
    /// \brief Возвращает следующую команду из очереди для записи
    /// файла или большого массива данных в устройство.
    QByteArray getNextDataSection();

    /// \brief Возвращает флаг исключительной ситуации.
    /// \details Данный флаг устанавливается в true, если
    /// требуется дополнительный вызов функции exceptionalAction.
    /// \see exceptionalAction.
    bool isExceptionalSituation() const noexcept;
    /// \brief Выполнение действий, связанных с вызванной исключительной ситуацией.
    /// \details Должен переопредляться в классах-наследниках, если
    /// они могут требовать исключительных ситуаций.
    /// \see isExceptionalSituation, setExceptionalSituationStatus.
    virtual void exceptionalAction(const CommandStruct &command) noexcept;

signals:
    /// \brief Сигнал, информирующий исполнителя на верхнем уровне
    /// о записи файла или большого массива данных в устройство.
    void writingLongData();
    /// \brief Сигнал, информирующий парсер ответов от устройства о
    /// том, что устройству была послана последняя секция данных.
    void writingLastSection();

    /// \brief Сигнал, который используется для передачи размера всего
    /// отправляемого файла или большого массива данных (в байтах) в устройство.
    void totalWritingBytes(const quint64 total);
    /// \brief Сигнал, который используется для передачи прогресса
    /// записи файла или большого массива данных (в байтах) в устройство.
    void progressBytes(const quint64 progress);

    /// \brief Сигнал, который используется для отправки сообщения в лог исполнителя запросов.
    void needToLog(const QString &message, const LogLevel level);
};

template <typename GroupType> //
GroupType BaseRequestParser::getGroupsByAddress(const quint32 addr) const noexcept
{
    const auto &dictionary { m_protocol.groups() };
    auto search = dictionary.find(addr);
    if (search != dictionary.cend())
    {
        if (std::holds_alternative<GroupType>(search->second))
            return std::get<GroupType>(search->second);
    }
    return GroupType {};
}

} // namespace Interface
