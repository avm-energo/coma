#include "errorqueue.h"
#define LOG_BUFFER_SIZE 1000

ErrorQueue::ErrorQueue(Singleton::token)
{
    m_lastErrorIndex = 0;
    m_errMsgPool = {};
}

const std::queue<ErrorMsg> *ErrorQueue::errMsgPool()
{
    return &m_errMsgPool;
}

ErrorMsg ErrorQueue::popError()
{
    ErrorMsg error = m_errMsgPool.front();
    m_errMsgPool.pop();
    return error;
}

void ErrorQueue::pushError(const ErrorMsg &msg)
{
    if (m_errMsgPool.size() > LOG_BUFFER_SIZE)
        popError();
    m_errMsgPool.push(msg);
    emit errCounts(m_errMsgPool.size());
}

size_t ErrorQueue::lastErrorIndex()
{
    return m_lastErrorIndex;
}

void ErrorQueue::setLastErrorIndex(size_t lastErrorIndex)
{
    m_lastErrorIndex = lastErrorIndex;
}
