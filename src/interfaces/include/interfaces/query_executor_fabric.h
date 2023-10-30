#pragma once

#include <interfaces/device_query_executor.h>

namespace Interface
{

class QueryExecutorFabric
{
public:
    /// \brief Удалённый конструктор по умолчанию.
    /// \details Not constructible type.
    QueryExecutorFabric() = delete;
    /// \brief Удалённый деструктор.
    /// \details Not constructible type.
    ~QueryExecutorFabric() = delete;

    static DeviceQueryExecutor *makeProtocomExecutor(RequestQueue &queue, quint32 timeout = 1000);
    static DeviceQueryExecutor *makeModbusExecutor(RequestQueue &queue, quint8 deviceAddress, quint32 timeout = 3000);
    static DeviceQueryExecutor *makeIec104Executor(RequestQueue &queue, quint32 timeout = 1000);
};

} // namespace Interface
