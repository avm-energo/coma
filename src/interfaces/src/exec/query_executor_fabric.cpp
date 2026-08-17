#include <common/names.h>
#include <interfaces/exec/iec104_query_executor.h>
#include <interfaces/exec/query_executor_fabric.h>
#include <interfaces/parsers/iec104/iec104_request_parser.h>
#include <interfaces/parsers/iec104/iec104_response_parser.h>
#include <interfaces/parsers/modbus/modbus_request_parser.h>
#include <interfaces/parsers/modbus/modbus_response_parser.h>
#include <interfaces/parsers/protocom/protocom_request_parser.h>
#include <interfaces/parsers/protocom/protocom_response_parser.h>

namespace Interface
{

DefaultQueryExecutor *QueryExecutorFabric::makeProtocomExecutor(RequestQueue &queue, UsbHidSettings *settings)
{
    auto executor = new DefaultQueryExecutor(queue, settings);
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

DefaultQueryExecutor *QueryExecutorFabric::makeModbusExecutor(RequestQueue &queue, BaseSettings *settings)
{
    auto executor = new DefaultQueryExecutor(queue, settings);
    executor->initLogger("Modbus");
    // NOTE: query executor must be parent for all parsers
    auto requestParser = new ModbusRequestParser(executor);
    requestParser->setDeviceAddress(settings->get(MemKeys::Serial::address));
    auto responseParser = new ModbusResponseParser(executor);
    responseParser->setDeviceAddress(settings->get(MemKeys::Serial::address));
    // Передача ожидаемого размера ответа между парсерами
    QObject::connect(requestParser, &ModbusRequestParser::expectedResponseSize, //
        responseParser, &ModbusResponseParser::expectedResponseSize);
    executor->setParsers(requestParser, responseParser);
    return executor;
}

DefaultQueryExecutor *QueryExecutorFabric::makeIec104Executor(RequestQueue &queue, IEC104Settings *settings)
{
    auto executor = new Iec104QueryExecutor(queue, settings);
    executor->initLogger("IEC104");
    // NOTE: query executor must be parent for all parsers
    auto requestParser = new Iec104RequestParser(executor);
    auto responseParser = new Iec104ResponseParser(executor);
    requestParser->updateControlBlock(executor->m_ctrlBlock);
    responseParser->updateControlBlock(executor->m_ctrlBlock);
    requestParser->setBaseStationAddress(settings->get(MemKeys::Iec104::bsAddress));
    // Парсер запросов отправляет данные в парсер ответов для подтверждения получения
    QObject::connect(requestParser, &Iec104RequestParser::currentCommand, //
        responseParser, &Iec104ResponseParser::receiveCurrentCommand);    //
    // Обработка счетчиков пришедшего I-пакета
    QObject::connect(responseParser, &Iec104ResponseParser::infoTransferFormatReceived, //
        executor, &Iec104QueryExecutor::countersFromIPacket);                           //
    // Обработка счетчиков пришедшего S-пакета
    QObject::connect(responseParser, &Iec104ResponseParser::SupervisoryFormatReceived, //
        executor, &Iec104QueryExecutor::countersFromSPacket);
    // Проверка посылки U-формата исполнителем запросов
    QObject::connect(responseParser, &Iec104ResponseParser::unnumberedFormatReceived, //
        executor, &Iec104QueryExecutor::checkUnnumberedFormat);                       //
    // Обработка состояния, когда запрошенные данные получены
    QObject::connect(responseParser, &Iec104ResponseParser::requestedDataReceived, //
        executor, &Iec104QueryExecutor::requestedDataReceived);                    //
    // Отправка следующего шага реактивного протокола передачи файла (журналов)
    QObject::connect(responseParser, &Iec104ResponseParser::fileReplyNeeded, //
        executor, &Iec104QueryExecutor::sendFileReply);                     //
    // Отправка следующего шага реактивного протокола записи файла (конфигурация, прошивка)
    QObject::connect(responseParser, &Iec104ResponseParser::fileWriteReplyNeeded, //
        executor, &Iec104QueryExecutor::sendFileWriteReply);                      //
    executor->setParsers(requestParser, responseParser);
    return executor;
}

} // namespace Interface
