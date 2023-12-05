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
}

void Iec104QueryExecutor::exec()
{
    ;
}

} // namespace Interface
