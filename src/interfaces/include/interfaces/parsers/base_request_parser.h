#pragma once

#include <deque>
#include <interfaces/types/common_types.h>

namespace Interface
{

class BaseRequestParser : public QObject
{
    Q_OBJECT
protected:
    static const std::map<Commands, CommandRegisters> s_wsCmdMap;

    QByteArray m_request;
    std::deque<QByteArray> m_writeLongData;
    bool m_isExceptionalSituation;

    /// \brief Устанавливает флаг исключительной ситуации.
    void setExceptionalSituationStatus(bool status) noexcept;

public:
    explicit BaseRequestParser(QObject *parent = nullptr);

    /// \brief Функция парсинга полученной команды в запрос к устройству.
    virtual QByteArray parse(const CommandStruct &command) = 0;
    /// \brief Возвращает команду для получения следующей
    /// секции файла или большого массива данных.
    virtual QByteArray getNextContinueCommand() noexcept = 0;
    /// \brief Возвращает следующую команду из очереди для записи
    /// файла или большого массива данных в устройство.
    QByteArray getNextChunk();

    /// \brief Возвращает флаг исключительной ситуации.
    /// \details Данный флаг устанавливается в true, если
    /// требуется дополнительный вызов функции exceptionalAction.
    /// \see exceptionalAction.
    bool isExceptionalSituation() const noexcept;
    /// \brief Выполнение действий, связанных с вызванной исключительной ситуацией.
    /// \details Должен переопредляться в классах-наследниках, если
    /// они могут требовать исключительных ситуаций.
    virtual void exceptionalAction(const CommandStruct &command) noexcept;

signals:
    void writingFile();
    void readingFile();
    void totalBytes(const int total);
    void progressBytes(const int progress);
    void parsedCommand(const Interface::CommandStruct &command);
};

} // namespace Interface
