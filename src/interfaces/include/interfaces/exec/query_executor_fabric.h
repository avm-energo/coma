#pragma once

#include <interfaces/exec/default_query_executor.h>

struct IEC104Settings;

namespace Interface
{

/// \brief Фабрика для создания экземпляров класса DeviceQueryExecutor.
class QueryExecutorFabric
{
public:
    /// \brief Удалённый конструктор по умолчанию.
    /// \details Not constructible type.
    QueryExecutorFabric() = delete;
    /// \brief Удалённый деструктор.
    /// \details Not constructible type.
    ~QueryExecutorFabric() = delete;

    /// \brief Создание исполнителя запросов к устройству по протоколу Protocom.
    static DefaultQueryExecutor *makeProtocomExecutor(RequestQueue &queue, quint32 timeout = 1000);
    /// \brief Создание исполнителя запросов к устройству по протоколу Modbus.
    static DefaultQueryExecutor *makeModbusExecutor(RequestQueue &queue, quint8 deviceAddress, quint32 timeout = 3000);
    /// \brief Создание исполнителя запросов к устройству по протоколу МЭК 60870-5-104.
    static DefaultQueryExecutor *makeIec104Executor(RequestQueue &queue, const IEC104Settings &settings);
};

} // namespace Interface
