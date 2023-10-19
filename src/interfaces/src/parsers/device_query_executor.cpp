#include "interfaces/parsers/device_query_executor.h"

#include <QTimer>
#include <interfaces/parsers/protocom_request_parser.h>
#include <interfaces/parsers/protocom_response_parser.h>

namespace Interface
{

DeviceQueryExecutor::DeviceQueryExecutor(RequestQueue &queue, QObject *parent)
    : QObject(parent), m_state(ExecutorState::Starting), m_queue(queue)
{
}

void DeviceQueryExecutor::initLogger(const QString &protocolName) noexcept
{
    m_log.init(protocolName + "Executor." + logExt);
    m_log.info(logStart);
}

void DeviceQueryExecutor::setParsers(BaseRequestParser *reqParser, BaseResponseParser *respParser) noexcept
{
    if (reqParser != nullptr && respParser != nullptr)
    {
        m_requestParser = reqParser;
        m_responseParser = respParser;
        connect(m_responseParser, &BaseResponseParser::responseParsed, //
            this, &DeviceQueryExecutor::responseSend);                 //
        connect(m_requestParser, &BaseRequestParser::totalBytes,       //
            m_responseParser, &BaseResponseParser::totalBytes);        //
        connect(m_requestParser, &BaseRequestParser::progressBytes,    //
            m_responseParser, &BaseResponseParser::progressBytes);     //
        connect(m_requestParser, &BaseRequestParser::writingFile,      //
            this, [this] { setState(ExecutorState::FileWriting); });   //
        connect(m_requestParser, &BaseRequestParser::readingFile,      //
            this, [this] { setState(ExecutorState::FileReading); });   //
    }
}

ExecutorState DeviceQueryExecutor::getState() const noexcept
{
    return m_state.load();
}

void DeviceQueryExecutor::setState(const ExecutorState newState) noexcept
{
    if (newState != m_state.load())
    {
        m_state.store(newState);
        emit stateChanged(newState);
    }
}

void DeviceQueryExecutor::writeFromQueue() noexcept
{
    auto opt = m_queue.deQueue();
    if (opt.has_value())
    {
        auto request = m_requestParser->parse(opt.value());
        if (request.isEmpty() || m_requestParser->isExceptionalSituation())
        {
            /// TODO: вызвать метод exceptionalAction
        }
        else
        {
            setState(ExecutorState::Pending);
            emit sendDataToInterface(request);
        }
    }
}

void DeviceQueryExecutor::run()
{
    auto currentState = getState();
    while (currentState != ExecutorState::Stopping)
    {
        switch (currentState)
        {
        case ExecutorState::RequestParsing:
            writeFromQueue();
            break;
        case ExecutorState::Pending:
            // Ничего не делаем, ждём изменения состояния
            break;
        default:
            break;
        }
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        currentState = getState();
    }
}

DeviceQueryExecutor *DeviceQueryExecutor::makeProtocomExecutor(RequestQueue &queue)
{
    auto executor = new DeviceQueryExecutor(queue);
    executor->initLogger("Protocom");
    auto requestParser = new ProtocomRequestParser(executor);
    auto responseParser = new ProtocomResponseParser(executor);
    executor->setParsers(requestParser, responseParser);
    return executor;
}

void DeviceQueryExecutor::receiveDataFromInterface(QByteArray data)
{
    // setState(ExecutorState::ResponseParsing);
    if (m_responseParser->isValid(data))
    {
        m_responseParser->parse(data);
    }
}

DeviceQueryExecutor *DeviceQueryExecutor::makeModbusExecutor(RequestQueue &queue)
{
    /// TODO
    Q_UNUSED(queue);
    return nullptr;
}

DeviceQueryExecutor *DeviceQueryExecutor::makeIec104Executor(RequestQueue &queue)
{
    /// TODO
    Q_UNUSED(queue);
    return nullptr;
}

} // namespace Interface
