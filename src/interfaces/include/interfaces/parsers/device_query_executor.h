#pragma once

#include <gen/logclass.h>
#include <interfaces/types/common_types.h>
#include <interfaces/utils/request_queue.h>

namespace Interface
{

class QTimer;
class BaseRequestParser;
class BaseResponseParser;

enum class ExecutorState : std::uint32_t
{
    Starting = 0,          ///< Стартовое состояние
    RequestParsing,        ///< Выполняется парсинг запроса
    Pending,               ///<
    FileReading,           ///<
    FileWriting,           ///<
    Stopping,              ///<
    Undefined = UINT32_MAX ///< ABI compatibility
};

/// \brief Класс исполнителя запросов к устройству.
class DeviceQueryExecutor : public QObject
{
    Q_OBJECT
private:
    std::atomic<ExecutorState> m_state;
    RequestQueue &m_queue;
    LogClass m_log;
    QTimer *m_timeoutTimer;
    BaseRequestParser *m_requestParser;
    BaseResponseParser *m_responseParser;

    /// \brief Приватный конструктор.
    /// \details Создание экземпляров класса доступно только через функции
    /// makeProtocomExecutor, makeModbusExecutor и makeIec104Executor.
    /// \see makeProtocomExecutor, makeModbusExecutor и makeIec104Executor.
    explicit DeviceQueryExecutor(RequestQueue &queue, QObject *parent = nullptr);

    /// \brief Инициализация логгера исполнителя запросов.
    /// \details Вызывается при создании исполнителя запросов.
    /// \param protocolName - имя протокола, согласно которому
    /// создастся файл логгирования работы исполнителя запросов.
    void initLogger(const QString &protocolName) noexcept;
    /// \brief Устанавливает переданные парсеры для работы исполнителя запросов.
    /// \details Вызывается при создании исполнителя запросов.
    void setParsers(BaseRequestParser *reqParser, BaseResponseParser *respParser) noexcept;

    /// \brief Возвращает текущее состояние исполнителя запросов.
    ExecutorState getState() const noexcept;
    /// \brief Изменяет текущее состояние исполнителя запросов указанным.
    void setState(const ExecutorState newState) noexcept;

    void writeFromQueue() noexcept;

public:
    DeviceQueryExecutor() = delete;
    DeviceQueryExecutor(const DeviceQueryExecutor &rhs) = delete;

    void run();

    static DeviceQueryExecutor *makeProtocomExecutor(RequestQueue &queue);
    static DeviceQueryExecutor *makeModbusExecutor(RequestQueue &queue);
    static DeviceQueryExecutor *makeIec104Executor(RequestQueue &queue);

public slots:
    void receiveDataFromInterface(QByteArray data);

signals:
    void stateChanged(const Interface::ExecutorState state);
    void finished();
    void responseSend(const Interface::DeviceResponse &resp);
    void sendDataToInterface(const QByteArray &data);
};

} // namespace Interface
