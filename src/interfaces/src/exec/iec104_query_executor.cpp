#include "interfaces/exec/iec104_query_executor.h"

#include <interfaces/parsers/iec104_request_parser.h>
#include <interfaces/parsers/iec104_response_parser.h>
#include <interfaces/types/settingstypes.h>

namespace Interface
{

using namespace Iec104;

Iec104QueryExecutor::Iec104QueryExecutor(RequestQueue &queue, const IEC104ConnectionParams params, QObject *parent)
    : DefaultQueryExecutor(queue, params.t1, parent)
    , m_ctrlBlock(std::make_shared<ControlBlock>())
    , m_params(params)
    , m_t2Timer(new QTimer(this))
    , m_t3Timer(new QTimer(this))
{
    m_t2Timer->setSingleShot(true);
    m_t2Timer->setInterval(m_params.t2);
    m_t3Timer->setSingleShot(true);
    m_t3Timer->setInterval(m_params.t3);
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
    DefaultQueryExecutor::writeToInterface(request);
    if (isCounted)
    {
        ++(m_ctrlBlock->m_sent);
        checkControlBlock();
    }
}

void Iec104QueryExecutor::exec()
{
    initConnection();
    DefaultQueryExecutor::exec();
    closeConnection();
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

void Iec104QueryExecutor::checkControlBlock() noexcept
{
    if (m_ctrlBlock->m_received >= m_params.w)
    {
        /// TODO: подумать о логике работы
    }
}

} // namespace Interface
