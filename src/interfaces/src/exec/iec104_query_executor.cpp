#include "interfaces/exec/iec104_query_executor.h"

#include <interfaces/parsers/iec104_request_parser.h>
#include <interfaces/parsers/iec104_response_parser.h>
#include <interfaces/types/settingstypes.h>

namespace Interface
{

using namespace Iec104;

Iec104QueryExecutor::Iec104QueryExecutor(RequestQueue &queue, const IEC104ConnectionParams params, QObject *parent)
    : DefaultQueryExecutor(queue, 1000, parent)
    , m_ctrlBlock(std::make_shared<ControlBlock>())
    , m_params(params)
    , m_t2Timer(new QTimer(this))
    , m_t3Timer(new QTimer(this))
    , m_acknowledgeReceived(0)
{
    m_t2Timer->setSingleShot(true);
    m_t2Timer->setInterval(m_params.t2 * 1000);
    m_t3Timer->setSingleShot(true);
    m_t3Timer->setInterval(m_params.t3 * 1000);
    connect(m_t2Timer, &QTimer::timeout, this, &Iec104QueryExecutor::sendSupervisoryMessage);
    connect(m_t3Timer, &QTimer::timeout, this, [this]() { testConnection(ControlArg::Activate); });
}

Iec104ResponseParser *Iec104QueryExecutor::getResponseParser() noexcept
{
    return reinterpret_cast<Iec104ResponseParser *>(m_responseParser);
}

Iec104RequestParser *Iec104QueryExecutor::getRequestParser() noexcept
{
    return reinterpret_cast<Iec104RequestParser *>(m_requestParser);
}

void Iec104QueryExecutor::initConnection() noexcept
{
    auto startMessage { getRequestParser()->createStartMessage() };
    writeToInterface(startMessage, false);
    setState(ExecutorState::Pending);
}

void Iec104QueryExecutor::closeConnection() noexcept
{
    auto stopMessage { getRequestParser()->createStopMessage() };
    writeToInterface(stopMessage, false);
}

void Iec104QueryExecutor::writeToInterface(const QByteArray &request, bool isCounted) noexcept
{
    emit sendDataToInterface(request);
    if (isCounted)
    {
        m_timeoutTimer->start();
        ++(m_ctrlBlock->m_sent);
        checkControlBlock();
    }
    writeToLog(request, Direction::ToDevice);
}

void Iec104QueryExecutor::exec()
{
    initConnection();
    DefaultQueryExecutor::exec();
    // closeConnection();
}

void Iec104QueryExecutor::receiveDataFromInterface(const QByteArray &response)
{
    m_responseParser->accumulateToResponseBuffer(response);
    m_responseParser->parse();
    switch (getState())
    {
    case ExecutorState::Stopping:
    case ExecutorState::Pending:
        // Просто выходим из слота, если исполнителя
        // остановили, или запрошенные данные ещё не пришли
        break;
    default:
        run();
        break;
    }
}

void Iec104QueryExecutor::testConnection(ControlArg arg) noexcept
{
    auto testMessage { getRequestParser()->createTestMessage(arg) };
    writeToInterface(testMessage, false);
}

void Iec104QueryExecutor::sendSupervisoryMessage() noexcept
{
    auto supervisoryMessage { getRequestParser()->createSupervisoryMessage() };
    m_acknowledgeReceived = m_ctrlBlock->m_received;
    writeToInterface(supervisoryMessage, false);
}

void Iec104QueryExecutor::checkControlBlock() noexcept
{
    auto triggerThresholdValue = m_acknowledgeReceived + m_params.w;
    if (m_ctrlBlock->m_received >= triggerThresholdValue || m_ctrlBlock->m_received == controlMax)
        sendSupervisoryMessage();
    if (m_ctrlBlock->m_received == controlMax)
        m_ctrlBlock->m_received = 0;
    if (m_ctrlBlock->m_sent == controlMax)
        m_ctrlBlock->m_sent = 0;
}

void Iec104QueryExecutor::checkUnnumberedFormat(const ControlFunc func, const ControlArg arg) noexcept
{
    switch (func)
    {
    case ControlFunc::StartDataTransfer:
        if (arg == ControlArg::Confirm)
            run();
        break;
    case ControlFunc::StopDataTransfer:
        if (arg == ControlArg::Confirm)
            emit finished();
        break;
    case ControlFunc::TestFrame:
        if (arg == ControlArg::Activate)
            testConnection(ControlArg::Confirm);
        break;
    }
}

void Iec104QueryExecutor::requestedDataReceived() noexcept
{
    m_timeoutTimer->stop();
    run(); // execute next query from queue
}

} // namespace Interface
