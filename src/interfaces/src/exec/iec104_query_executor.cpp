#include "interfaces/exec/iec104_query_executor.h"

namespace Interface
{

Iec104QueryExecutor::Iec104QueryExecutor(RequestQueue &queue, quint32 timeout, QObject *parent)
    : DefaultQueryExecutor(queue, timeout, parent)
{
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
