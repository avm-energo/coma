#pragma once

#include <interfaces/exec/default_query_executor.h>
#include <interfaces/types/iec104_settings.h>
#include <interfaces/types/iec104_types.h>

namespace Interface
{

class Iec104ResponseParser;
class Iec104RequestParser;

class Iec104QueryExecutor final : public DefaultQueryExecutor
{
    Q_OBJECT
private:
    friend class QueryExecutorFabric;

    /// \brief Контрольный блок пакета.
    SharedControlBlock m_ctrlBlock;

    /// \brief Таймауты
    /// \param t0 - Тайм-аут при установлении соединения. Здесь не используется.
    /// \param t1 - Тайм-аут при посылке или тестировании APDU.
    /// \param t2 - Тайм-аут для подтверждения в случае отсутствия сообщения с данными t2 < t1.
    /// \param t3 - Тайм-аут для посылки блоков тестирования в случае долгого простоя.
    /// \see ГОСТ Р МЭК 60870-5-104 пункт 9.6 Основные прикладные функции - стр. 43.
    QTimer *m_t1Timer, *m_t2Timer, *m_t3Timer;

    /// \brief Признак того, что исполнитель ждёт STOPDT con перед реальной остановкой.
    /// \details Читается/пишется с разных потоков (stop() дёргается из потока, вызвавшего
    /// ConnectionContext::reset(), а подтверждение приходит в потоке исполнителя) - atomic.
    std::atomic_bool m_isStopping = false;

    /// \brief Приватный конструктор.
    /// \details Создание экземпляров класса доступно только через QueryExecutorFabric.
    /// \see QueryExecutorFabric.
    explicit Iec104QueryExecutor(RequestQueue &queue, IEC104Settings *settings, QObject *parent = nullptr);

    /// \brief Преобразование хранимого парсера запросов к primary типу.
    inline Iec104ResponseParser *getResponseParser() noexcept;
    /// \brief Преобразование хранимого парсера ответов к primary типу.
    inline Iec104RequestParser *getRequestParser() noexcept;

    /// \brief Отправка команды Stop data transfer activation.
    void closeConnection() noexcept;
    /// \brief Функция для отправки запроса устройству с подсчётом отправляемых сообщений.
    void writeToInterface(const QByteArray &request, bool isCounted = true) noexcept override;

public:
    /// \brief Удалённый конструктор по умолчанию.
    Iec104QueryExecutor() = delete;
    /// \brief Удалённый конструктор копирования.
    Iec104QueryExecutor(const DefaultQueryExecutor &rhs) = delete;

    void start() override;
    /// \brief Отправка STOPDT act и ожидание STOPDT con перед реальной остановкой исполнителя.
    /// \details ГОСТ 104, рис. 16: контролирующая станция посылает STOPDT act и ждёт
    /// подтверждения (t1, как и на STARTDT - рис. 15) перед закрытием соединения. Реальный
    /// переход в Stopping (DefaultQueryExecutor::stop()) откладывается до прихода STOPDT con
    /// (см. checkUnnumberedFormat) либо до истечения t1, если con так и не пришёл.
    void stop() noexcept override;

public slots:
    /// \brief Слот для принятия от устройства данных асинхронно.
    void receiveDataFromInterface(const QByteArray &response) override;

private slots:
    /// \brief Слот для активации или подтверждения U-посылки для тестирования данных.
    void testConnection(Iec104::ControlArg arg) noexcept;
    /// \brief Слот для отправки S-посылки.
    void sendSupervisoryMessage() noexcept;
    /// \brief Слот для обработки полученного фрейма U-формата.
    void checkUnnumberedFormat(const Iec104::ControlFunc func, const Iec104::ControlArg arg) noexcept;
    /// \brief Слот для обработки сигнала от парсера ответов, когда запрошенные данные получены.
    void requestedDataReceived() noexcept;
    /// \brief Слот для отправки следующего шага реактивного протокола передачи файла устройству.
    void sendFileReply(const Iec104::FileReplyAction action, const quint8 fileNum, const quint8 section) noexcept;
    /// \brief Слот для отправки следующего шага реактивного протокола записи файла в устройство.
    void sendFileWriteReply(const Iec104::FileWriteReplyAction action, const QByteArray &payload) noexcept;
    /// \brief Слот для проверки и подтверждения счетчиков с I-пакета.
    void countersFromIPacket(const RSCounter &counters) noexcept;
    /// \brief Слот для проверки и подтверждения счетчиков с S-пакета.
    void countersFromSPacket(const RSCounter &counters) noexcept;
};

} // namespace Interface
