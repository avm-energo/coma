#pragma once

#include <QTimer>
#include <gen/logclass.h>
#include <interfaces/types/base_settings.h>
#include <interfaces/types/protocol_settings.h>
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
    ReadingLongData,       ///< Чтение большого массива данных или файла.
    WritingLongData,       ///< Запись большого массива данных или файла.
    Stopping,              ///< Исполнитель заканчивает работу.
    Undefined = UINT32_MAX ///< ABI compatibility.
};

/// \brief Класс исполнителя запросов к устройству.
class DefaultQueryExecutor : public QObject
{
    Q_OBJECT
protected:
    friend class QueryExecutorFabric;

    std::atomic<ExecutorState> m_state;
    std::atomic<Commands> m_lastRequestedCommand;
    std::reference_wrapper<RequestQueue> m_queue;
    LogClass m_log;
    QTimer *m_timeoutTimer;
    BaseRequestParser *m_requestParser;
    BaseResponseParser *m_responseParser;

    /// \brief Приватный конструктор.
    /// \details Создание экземпляров класса доступно только через QueryExecutorFabric.
    /// \param timeout[in] - допустимые интервал времени в мс между записью запроса в
    /// устройства, и ожиданием от него ответа. При превышении данного интервала происходит
    /// таймаут, текущая отправленная команда сбрасывается, о таймауте уведомляется
    /// ConnectionManager с помощью сигнала timeout, из очереди запросов берётся следующий запрос.
    /// \see QueryExecutorFabric.
    explicit DefaultQueryExecutor(RequestQueue &queue, const BaseSettings &settings, QObject *parent = nullptr);

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

    /// \brief Функция, которая использует BaseRequestParser для парсинга
    /// запроса из очереди запросов в бинарное представление конкретного протокола.
    void parseFromQueue() noexcept;

    /// \brief Функция для отправки запроса активному интерфейсу.
    virtual void writeToInterface(const QByteArray &request, bool isCounted = true) noexcept;

    /// \brief Функция для записи данных в лог протокола.
    void writeToLog(const QByteArray &ba, const Direction dir = Direction::NoDirection) noexcept;

private slots:
    /// \brief Приватный слот для записи информации в лог от парсера запросов и ответов.
    void logFromParser(const QString &message, const LogLevel level);

public:
    /// \brief Удалённый конструктор по умолчанию.
    DefaultQueryExecutor() = delete;
    /// \brief Удалённый конструктор копирования.
    DefaultQueryExecutor(const DefaultQueryExecutor &rhs) = delete;

    /// \brief Функция, содержащая главный цикл исполнителя запросов.
    virtual void exec();

    /// \brief Функция для продолжения работы исполнителя запросов.
    /// \details Переводит состояние исполнителя в ExecutorState::RequestParsing.
    void run() noexcept;
    /// \brief Функция для приостановки работы исполнителя запросов.
    /// \details Переводит состояние исполнителя в ExecutorState::Pending.
    void pause() noexcept;
    /// \brief Функция для окончания работы исполнителя запросов.
    /// \details Переводит состояние исполнителя в ExecutorState::Stopping.
    void stop() noexcept;

    /// \brief Функция, возвраващающая последнюю запрошенную команду.
    Commands getLastRequestedCommand() const noexcept;

public slots:
    /// \brief Слот для принятия от устройства ответа на посланный ему ранее запрос.
    virtual void receiveDataFromInterface(const QByteArray &response);
    /// \brief Слот для принятия протокола обмена данными с устройством.
    void receiveProtocolDescription(const ProtocolDescription &desc) noexcept;
    /// \brief Слот для отмены текущего запроса.
    void cancelQuery();
    /// \brief Слот, вызываемый при переподключении текущего интерфейса.
    void reconnectEvent();

signals:
    /// \brief Сигнал для уведомления об изменении состояния исполнителя запросов.
    void stateChanged(const Interface::ExecutorState state);
    /// \brief Сигнал для уведомления об окончании работы исполнителя запросов.
    void finished();
    /// \brief Сигнал для уведомления о таймауте устройства на посланный ему запрос.
    /// \details Данный сигнал отправляется в том случае, если устройство не успело
    /// ответить на отправленный ему запрос в течение заданного интервала времени.
    void timeout();

    /// \brief Сигнал для отправки распарсенного ответа от устройства на высшестоящий урвоень.
    void responseSend(const Interface::DeviceResponse &resp);
    /// \brief Сигнал для отправки запроса устройству в бинарном представлении.
    void sendDataToInterface(const QByteArray &data);
};

} // namespace Interface
