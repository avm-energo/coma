#pragma once

#include <interfaces/device_query_executor.h>

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
    static DeviceQueryExecutor *makeProtocomExecutor(RequestQueue &queue, quint32 timeout = 1000);
    /// \brief Создание исполнителя запросов к устройству по протоколу Modbus.
    static DeviceQueryExecutor *makeModbusExecutor(RequestQueue &queue, quint8 deviceAddress, quint32 timeout = 3000);
    /// \brief Создание исполнителя запросов к устройству по протоколу МЭК 60870-5-104.
    static DeviceQueryExecutor *makeIec104Executor(RequestQueue &queue, quint16 bsAddress, quint32 timeout = 1000);
};

} // namespace Interface
