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

    SharedControlBlock m_ctrlBlock;
    IEC104ConnectionParams m_params;
    QTimer *m_t2Timer;
    QTimer *m_t3Timer;

    /// \brief Приватный конструктор.
    /// \details Создание экземпляров класса доступно только через QueryExecutorFabric.
    /// \see QueryExecutorFabric.
    explicit Iec104QueryExecutor(RequestQueue &queue, const IEC104ConnectionParams params, QObject *parent = nullptr);

    inline Iec104ResponseParser *getResponseParser() noexcept;
    inline Iec104RequestParser *getRequestParser() noexcept;

    /// \brief Отправка команды Start data transfer activation.
    void initConnection() noexcept;
    /// \brief Отправка команды Stop data transfer activation.
    void closeConnection() noexcept;
    /// \brief Функция для отправки запроса устройству с подсчётом отправляемых сообщений.
    void writeToInterface(const QByteArray &request, bool isCounted = true) noexcept;

public:
    /// \brief Удалённый конструктор по умолчанию.
    Iec104QueryExecutor() = delete;
    /// \brief Удалённый конструктор копирования.
    Iec104QueryExecutor(const DefaultQueryExecutor &rhs) = delete;

    /// \brief Перегрузка главного цикла исполнителя запросов.
    /// \details Непосредственно перед выполнением тела главного цикла, для установки
    /// соединения с устройством вызывается метод initConnection. После выполнения
    /// главного цикла вызывается метод closeConnection.
    void exec() override;

public slots:
    /// \brief Слот для проверки контрольного блока.
    void checkControlBlock() noexcept;
};

} // namespace Interface
