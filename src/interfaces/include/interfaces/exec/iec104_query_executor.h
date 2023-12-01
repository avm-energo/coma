#pragma once

#include <interfaces/exec/default_query_executor.h>

namespace Interface
{

class Iec104ResponseParser;
class Iec104RequestParser;

class Iec104QueryExecutor final : public DefaultQueryExecutor
{
    Q_OBJECT
private:
    friend class QueryExecutorFabric;

    /// \brief Приватный конструктор.
    /// \details Создание экземпляров класса доступно только через QueryExecutorFabric.
    /// \see QueryExecutorFabric.
    explicit Iec104QueryExecutor(RequestQueue &queue, quint32 timeout, QObject *parent = nullptr);

    inline Iec104ResponseParser *getResponseParser() noexcept;
    inline Iec104RequestParser *getRequestParser() noexcept;

    void setParsers(BaseRequestParser *reqParser, BaseResponseParser *respParser) noexcept override;

public:
    /// \brief Удалённый конструктор по умолчанию.
    Iec104QueryExecutor() = delete;
    /// \brief Удалённый конструктор копирования.
    Iec104QueryExecutor(const DefaultQueryExecutor &rhs) = delete;

    void exec() override;
};

} // namespace Interface
