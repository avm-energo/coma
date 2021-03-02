#include "errorqueue.h"
#define LOG_BUFFER_SIZE 1000

ErrorQueue::ErrorQueue(Singleton::token)
{
    m_errMsgPool = {};
    m_newErrors = 0;
}

const std::vector<ErrorMsg> *ErrorQueue::errMsgPool()
{
    return &m_errMsgPool;
}

ErrorMsg ErrorQueue::popError()
{
    ErrorMsg error = m_errMsgPool.front();
    return error;
}

void ErrorQueue::pushError(const ErrorMsg &msg)
{
    if (m_errMsgPool.size() > LOG_BUFFER_SIZE)
        m_errMsgPool.erase(m_errMsgPool.begin());
    m_errMsgPool.push_back(msg);
    emit errCounts(++m_newErrors);
}

void ErrorQueue::clearCounter()
{
    m_newErrors = 0;
}
