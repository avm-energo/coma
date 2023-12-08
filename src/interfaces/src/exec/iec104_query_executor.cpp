#include "interfaces/exec/iec104_query_executor.h"

#include <interfaces/parsers/iec104_request_parser.h>
#include <interfaces/parsers/iec104_response_parser.h>

namespace Interface
{

using namespace Iec104;

Iec104QueryExecutor::Iec104QueryExecutor(RequestQueue &queue, quint32 timeout, QObject *parent)
    : DefaultQueryExecutor(queue, timeout, parent), m_ctrlBlock(std::make_shared<ControlBlock>())
{
}

Iec104ResponseParser *Iec104QueryExecutor::getResponseParser() noexcept
{
    return reinterpret_cast<Iec104ResponseParser *>(m_responseParser);
}

Iec104RequestParser *Iec104QueryExecutor::getRequestParser() noexcept
{
    return reinterpret_cast<Iec104RequestParser *>(m_requestParser);
}

void Iec104QueryExecutor::setParsers(BaseRequestParser *reqParser, BaseResponseParser *respParser) noexcept
{
    DefaultQueryExecutor::setParsers(reqParser, respParser);
    getRequestParser()->updateControlBlock(m_ctrlBlock);
    getResponseParser()->updateControlBlock(m_ctrlBlock);
}

void Iec104QueryExecutor::initConnection() noexcept
{
    auto startMsg { getRequestParser()->createStartMessage() };
    writeToInterface(startMsg, false);
    setState(ExecutorState::Pending);
}

void Iec104QueryExecutor::closeConnection() noexcept
{
    auto stopMsg { getRequestParser()->createStopMessage() };
    writeToInterface(stopMsg, false);
}

void Iec104QueryExecutor::writeToInterface(const QByteArray &message, bool isCounted) noexcept
{
    DefaultQueryExecutor::writeToInterface(message);
    if (isCounted)
        ++(m_ctrlBlock->m_sent);
}

void Iec104QueryExecutor::exec()
{
    initConnection();
    DefaultQueryExecutor::exec();
    closeConnection();
}

} // namespace Interface
