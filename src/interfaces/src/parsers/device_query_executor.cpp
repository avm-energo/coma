#include "interfaces/parsers/device_query_executor.h"

#include <interfaces/parsers/protocom_request_parser.h>
#include <interfaces/parsers/protocom_response_parser.h>

namespace Interface
{

DeviceQueryExecutor::DeviceQueryExecutor(RequestQueue &queue, quint32 timeout, QObject *parent)
    : QObject(parent), m_state(ExecutorState::Starting), m_queue(queue), m_timeoutTimer(new QTimer(this))
{
    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(timeout);
    connect(m_timeoutTimer, &QTimer::timeout, this, [this] {
        qCritical() << "Timeout";
        m_log.error("Timeout");
        cancelQuery();
        emit this->timeout();
    });
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
        connect(m_responseParser, &BaseResponseParser::responseParsed,    //
            this, &DeviceQueryExecutor::responseSend);                    //
        connect(m_requestParser, &BaseRequestParser::totalWritingBytes,   //
            m_responseParser, &BaseResponseParser::processProgressRange); //
        connect(m_requestParser, &BaseRequestParser::progressBytes,       //
            m_responseParser, &BaseResponseParser::processProgressCount); //
        connect(m_requestParser, &BaseRequestParser::writingLastSection,  //
            m_responseParser, &BaseResponseParser::lastSectionSended);    //
        connect(m_requestParser, &BaseRequestParser::writingLongData, this, [this] {
            setState(ExecutorState::WritingLongData);
            m_queue.get().deactivate();
        });
        connect(m_responseParser, &BaseResponseParser::readingLongData, this, [this] {
            setState(ExecutorState::ReadingLongData);
            m_queue.get().deactivate();
        });
    }
}

ExecutorState DeviceQueryExecutor::getState() const noexcept
{
    return m_state.load();
}

void DeviceQueryExecutor::setState(const ExecutorState newState) noexcept
{
    m_state.store(newState);
    emit stateChanged(newState);
}

void DeviceQueryExecutor::parseFromQueue() noexcept
{
    auto opt = m_queue.get().deQueue();
    if (opt.has_value())
    {
        const auto command(opt.value());
        auto request = m_requestParser->parse(command);
        if (request.isEmpty() && m_requestParser->isExceptionalSituation())
            m_requestParser->exceptionalAction(command);
        else
        {
            if (getState() == ExecutorState::RequestParsing)
                setState(ExecutorState::Pending);
            m_lastRequestedCommand = command.command;
            m_responseParser->setRequest(command);
            writeToInterface(request);
        }
    }
}

void DeviceQueryExecutor::writeToInterface(const QByteArray &request) noexcept
{
    emit sendDataToInterface(request);
    m_timeoutTimer->start();
    writeToLog(request, Direction::ToDevice);
}

void DeviceQueryExecutor::writeToLog(const QByteArray &ba, const Direction dir) noexcept
{
    QString msg = "DeviceQueryExecutor";
    switch (dir)
    {
    case Interface::FromDevice:
        msg += ": <- ";
        break;
    case Interface::ToDevice:
        msg += ": -> ";
        break;
    default:
        msg += ": ";
        break;
    }
    msg += ba.toHex();
    m_log.info(msg);
}

void DeviceQueryExecutor::exec()
{
    auto currentState = getState();
    while (currentState != ExecutorState::Stopping)
    {
        switch (currentState)
        {
        case ExecutorState::RequestParsing:
            parseFromQueue();
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

void DeviceQueryExecutor::run() noexcept
{
    if (getState() != ExecutorState::Stopping)
    {
        setState(ExecutorState::RequestParsing);
        m_queue.get().activate();
    }
}

void DeviceQueryExecutor::pause() noexcept
{
    setState(ExecutorState::Pending);
    m_queue.get().deactivate();
}

void DeviceQueryExecutor::stop() noexcept
{
    setState(ExecutorState::Stopping);
}

const Commands DeviceQueryExecutor::getLastRequestedCommand() const noexcept
{
    return m_lastRequestedCommand;
}

void DeviceQueryExecutor::receiveDataFromInterface(QByteArray response)
{
    m_timeoutTimer->stop();
    if (m_responseParser->isValid(response))
    {
        m_responseParser->parse(response);
        writeToLog(response, Direction::FromDevice);

        switch (getState())
        {
        case ExecutorState::ReadingLongData:
        {
            if (!m_responseParser->isLastSectionReceived())
            {
                auto request = m_requestParser->getNextContinueCommand();
                writeToInterface(request);
            }
            else
                run();
            break;
        }
        case ExecutorState::WritingLongData:
        {
            auto nextRequest = m_requestParser->getNextDataSection();
            // Если чанк не пустой, то ещё не отправили файл полностью
            if (!nextRequest.isEmpty())
                writeToInterface(nextRequest);
            // Если чанк пустой, то отправили файл полностью
            else
                run();
            break;
        }
        case ExecutorState::Stopping:
            // Просто выходим из слота, если исполнителя остановили
            break;
        default:
            run();
            break;
        }
    }
}

void DeviceQueryExecutor::cancelQuery()
{
    setState(ExecutorState::RequestParsing);
}

DeviceQueryExecutor *DeviceQueryExecutor::makeProtocomExecutor(RequestQueue &queue, quint32 timeout)
{
    auto executor = new DeviceQueryExecutor(queue, timeout);
    executor->initLogger("Protocom");
    auto requestParser = new ProtocomRequestParser(executor);
    auto responseParser = new ProtocomResponseParser(executor);
    connect(requestParser, &ProtocomRequestParser::sendJournalData, //
        responseParser, &ProtocomResponseParser::receiveJournalData);
    executor->setParsers(requestParser, responseParser);
    return executor;
}

DeviceQueryExecutor *DeviceQueryExecutor::makeModbusExecutor(RequestQueue &queue, quint32 timeout)
{
    /// TODO
    Q_UNUSED(queue);
    Q_UNUSED(timeout);
    return nullptr;
}

DeviceQueryExecutor *DeviceQueryExecutor::makeIec104Executor(RequestQueue &queue, quint32 timeout)
{
    /// TODO
    Q_UNUSED(queue);
    Q_UNUSED(timeout);
    return nullptr;
}

} // namespace Interface
