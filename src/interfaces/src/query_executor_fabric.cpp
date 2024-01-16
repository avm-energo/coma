#include "interfaces/query_executor_fabric.h"

#include <interfaces/parsers/modbus_request_parser.h>
#include <interfaces/parsers/modbus_response_parser.h>
#include <interfaces/parsers/protocom_request_parser.h>
#include <interfaces/parsers/protocom_response_parser.h>

namespace Interface
{

DeviceQueryExecutor *QueryExecutorFabric::makeProtocomExecutor(RequestQueue &queue, quint32 timeout)
{
    auto executor = new DeviceQueryExecutor(queue, timeout);
    executor->initLogger("Protocom");
    // NOTE: query executor must be parent for all parsers
    auto requestParser = new ProtocomRequestParser(executor);
    auto responseParser = new ProtocomResponseParser(executor);
    // Чтение журналов через парсер запросов
    QObject::connect(requestParser, &ProtocomRequestParser::sendJournalData, //
        responseParser, &ProtocomResponseParser::receiveJournalData);
    executor->setParsers(requestParser, responseParser);
    return executor;
}

DeviceQueryExecutor *QueryExecutorFabric::makeModbusExecutor(RequestQueue &queue, quint8 deviceAddress, quint32 timeout)
{
    auto executor = new DeviceQueryExecutor(queue, timeout);
    executor->initLogger("Modbus");
    // NOTE: query executor must be parent for all parsers
    auto requestParser = new ModbusRequestParser(executor);
    requestParser->setDeviceAddress(deviceAddress);
    auto responseParser = new ModbusResponseParser(executor);
    responseParser->setDeviceAddress(deviceAddress);
    // Передача ожидаемого размера ответа между парсерами
    QObject::connect(requestParser, &ModbusRequestParser::expectedResponseSize, //
        responseParser, &ModbusResponseParser::expectedResponseSize);
    executor->setParsers(requestParser, responseParser);
    return executor;
}

DeviceQueryExecutor *QueryExecutorFabric::makeIec104Executor(RequestQueue &queue, quint32 timeout)
{
    /// TODO
    Q_UNUSED(queue);
    Q_UNUSED(timeout);
    return nullptr;
}

} // namespace Interface
