#pragma once

#include <QTimer>
#include <gen/logclass.h>
#include <interfaces/types/common_types.h>
#include <interfaces/utils/request_queue.h>

namespace Interface
{

class BaseRequestParser;
class BaseResponseParser;

/// \brief Перечисление для описания состояния исполнителя запросов к устройству.
enum class ExecutorState : std::uint32_t
{
    Starting = 0,          ///< Стартовое состояние.
    RequestParsing,        ///< Выполняется парсинг запроса.
    Pending,               ///< Состояние паузы, исполнитель "заморожен".
    ExtendedReading,       ///< Режим работы, который активируется
                           ///< когда от интерфейса пришло недостаточно данных
                           ///< (используется для Modbus RTU).
    ReadingLongData,       ///<
    WritingLongData,       ///<
    Stopping,              ///< Исполнитель заканчивает работу.
    Undefined = UINT32_MAX ///< ABI compatibility.
};

/// \brief Класс исполнителя запросов к устройству.
class DeviceQueryExecutor : public QObject
{
    Q_OBJECT
private:
    friend class QueryExecutorFabric;
    std::atomic<ExecutorState> m_state;
    std::atomic<Commands> m_lastRequestedCommand;
    std::reference_wrapper<RequestQueue> m_queue;
    LogClass m_log;
    QTimer *m_timeoutTimer;
    BaseRequestParser *m_requestParser;
    BaseResponseParser *m_responseParser;

    /// \brief Приватный конструктор.
    /// \details Создание экземпляров класса доступно только через функции
    /// makeProtocomExecutor, makeModbusExecutor и makeIec104Executor.
    /// \see makeProtocomExecutor, makeModbusExecutor и makeIec104Executor.
    explicit DeviceQueryExecutor(RequestQueue &queue, quint32 timeout, QObject *parent = nullptr);

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

    void parseFromQueue() noexcept;
    void writeToInterface(const QByteArray &request) noexcept;

    void writeToLog(const QByteArray &ba, const Direction dir = Direction::NoDirection) noexcept;

public:
    /// \brief Удалённый конструктор по умолчанию.
    DeviceQueryExecutor() = delete;
    /// \brief Удалённый конструктор копирования.
    DeviceQueryExecutor(const DeviceQueryExecutor &rhs) = delete;

    void exec();
    void run() noexcept;
    void pause() noexcept;
    void stop() noexcept;

    const Commands getLastRequestedCommand() const noexcept;

    static DeviceQueryExecutor *makeProtocomExecutor(RequestQueue &queue, quint32 timeout = 1000);
    static DeviceQueryExecutor *makeModbusExecutor(RequestQueue &queue, quint32 timeout = 3000);
    static DeviceQueryExecutor *makeIec104Executor(RequestQueue &queue, quint32 timeout = 1000);

public slots:
    void receiveDataFromInterface(QByteArray response);
    void cancelQuery();

signals:
    void stateChanged(const Interface::ExecutorState state);
    void finished();
    void timeout();
    void responseSend(const Interface::DeviceResponse &resp);
    void sendDataToInterface(const QByteArray &data);
};

} // namespace Interface
