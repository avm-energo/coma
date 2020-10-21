#pragma once

#include "error.h"
#include "singleton.h"

#include <queue>
class ErrorQueue : public QObject, public Singleton<ErrorQueue>
{
    Q_OBJECT
public:
    ErrorQueue(token);
    const std::queue<Error::ErMsg> *errMsgPool();
    Error::ErMsg popError();
    void pushError(const Error::ErMsg &msg);

    int lastErrorIndex();
    void setLastErrorIndex(int lastErrorIndex);

signals:
    void errCounts(size_t size);

private:
    std::queue<Error::ErMsg> m_errMsgPool;
    int m_lastErrorIndex;
};
