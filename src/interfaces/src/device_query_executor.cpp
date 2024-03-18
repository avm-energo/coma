#include "interfaces/device_query_executor.h"

#include <interfaces/parsers/base_request_parser.h>
#include <interfaces/parsers/base_response_parser.h>

namespace Interface
{

DeviceQueryExecutor::DeviceQueryExecutor(RequestQueue &queue, quint32 timeout, QObject *parent)
    : QObject(parent), m_state(ExecutorState::Starting), m_queue(std::ref(queue)), m_timeoutTimer(new QTimer(this))
{
    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(timeout);
    connect(m_timeoutTimer, &QTimer::timeout, this, [this] {
        qCritical() << "Timeout, command: " << m_lastRequestedCommand.load();
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
            m_responseParser, &BaseResponseParser::lastSectionSent);    //
        connect(m_requestParser, &BaseRequestParser::writingLongData, this, [this] {
            setState(ExecutorState::WritingLongData);
            m_timeoutTimer->setInterval(m_timeoutTimer->interval() * 5);
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
    if (getState() != ExecutorState::Stopping)
    {
        m_state.store(newState);
        emit stateChanged(newState);
    }
}

void DeviceQueryExecutor::parseFromQueue() noexcept
{
    auto opt = m_queue.get().getFromQueue();
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
            m_lastRequestedCommand.store(command.command);
            m_responseParser->setRequest(command);
            writeToInterface(request);
        }
    }
    else
    {
        // Если нет запросов в очереди, то ждём, пока они появятся
        m_queue.get().waitFillingQueue();
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
    emit finished();
}

void DeviceQueryExecutor::run() noexcept
{
    setState(ExecutorState::RequestParsing);
    m_queue.get().activate();
}

void DeviceQueryExecutor::pause() noexcept
{
    setState(ExecutorState::Pending);
    m_queue.get().deactivate();
}

void DeviceQueryExecutor::stop() noexcept
{
    setState(ExecutorState::Stopping);
    m_queue.get().activate();
    m_queue.get().addToQueue({});
}

const Commands DeviceQueryExecutor::getLastRequestedCommand() const noexcept
{
    return m_lastRequestedCommand.load();
}

void DeviceQueryExecutor::receiveDataFromInterface(const QByteArray &response)
{
    // Валидация при фрагментировании ответа от устройства
    m_responseParser->accumulateToResponseBuffer(response);
    if (m_responseParser->isCompleteResponse())
    {
        m_timeoutTimer->stop();
        writeToLog(m_responseParser->getResponseBuffer(), Direction::FromDevice);
    }
    else
        return;

    auto validationResult = m_responseParser->validate();
    if (validationResult == Error::NoError)
    {
        m_responseParser->parse();

        switch (getState())
        {
        case ExecutorState::ReadingLongData:
        {
            // Если не получили последнюю секцию, то посылаем команды на продолжение
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
            {
                m_timeoutTimer->setInterval(m_timeoutTimer->interval() / 5);
                run();
            }
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
    else
    {
        DataTypes::GeneralResponseStruct resp {
            DataTypes::GeneralResponseTypes::Error, //
            static_cast<quint64>(validationResult)  //
        };
        emit responseSend(resp);
        cancelQuery();
    }
}

void DeviceQueryExecutor::cancelQuery()
{
    m_log.warning("Command canceled");
    m_responseParser->clearResponseBuffer();
    m_queue.get().activate();
    if (m_timeoutTimer->isActive())
        m_timeoutTimer->stop();
    if (getState() == ExecutorState::WritingLongData)
        m_timeoutTimer->setInterval(m_timeoutTimer->interval() / 5);
    setState(ExecutorState::RequestParsing);
}

void DeviceQueryExecutor::reconnectEvent()
{
    m_log.warning("Reconnect");
    cancelQuery();
    pause();
    m_queue.get().clear();
}

} // namespace Interface
