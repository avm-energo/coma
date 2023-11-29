#include "interfaces/exec/query_executor_fabric.h"

#include <interfaces/exec/iec104_query_executor.h>
#include <interfaces/parsers/iec104_request_parser.h>
#include <interfaces/parsers/iec104_response_parser.h>
#include <interfaces/parsers/modbus_request_parser.h>
#include <interfaces/parsers/modbus_response_parser.h>
#include <interfaces/parsers/protocom_request_parser.h>
#include <interfaces/parsers/protocom_response_parser.h>

namespace Interface
{

DefaultQueryExecutor *QueryExecutorFabric::makeProtocomExecutor(RequestQueue &queue, quint32 timeout)
{
    auto executor = new DefaultQueryExecutor(queue, timeout);
    executor->initLogger("Protocom");
    // NOTE: query executor must be parent for all parsers
    auto requestParser = new ProtocomRequestParser(executor);
    auto responseParser = new ProtocomResponseParser(executor);
    // Чтение журналов через парсер запросов
    QObject::connect(requestParser, &ProtocomRequestParser::sendJournalData, //
        responseParser, &ProtocomResponseParser::receiveJournalData);
    // Эмуляция ответа "Ок"
    QObject::connect(requestParser, &ProtocomRequestParser::emulateOkAnswer, //
        responseParser, &ProtocomResponseParser::processOk);
    executor->setParsers(requestParser, responseParser);
    return executor;
}

DefaultQueryExecutor *QueryExecutorFabric::makeModbusExecutor(
    RequestQueue &queue, quint8 deviceAddress, quint32 timeout)
{
    auto executor = new DefaultQueryExecutor(queue, timeout);
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

DefaultQueryExecutor *QueryExecutorFabric::makeIec104Executor(RequestQueue &queue, quint16 bsAddress, quint32 timeout)
{
    Q_UNUSED(bsAddress);
    auto executor = new Iec104QueryExecutor(queue, timeout);
    executor->initLogger("IEC104");
    // NOTE: query executor must be parent for all parsers
    auto requestParser = new Iec104RequestParser(executor);
    auto responseParser = new Iec104ResponseParser(executor);
    executor->setParsers(requestParser, responseParser);
    return executor;
}

} // namespace Interface
