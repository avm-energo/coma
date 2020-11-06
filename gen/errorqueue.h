#pragma once

#include "error.h"
#include "singleton.h"

#include <queue>
class ErrorQueue : public QObject, public Singleton<ErrorQueue>
{
    Q_OBJECT
public:
    explicit ErrorQueue(token);
    const std::queue<ErrorMsg> *errMsgPool();
    ErrorMsg popError();
    void pushError(const ErrorMsg &msg);

    size_t lastErrorIndex();
    void setLastErrorIndex(size_t lastErrorIndex);

signals:
    void errCounts(size_t size);

private:
    std::queue<ErrorMsg> m_errMsgPool;
    size_t m_lastErrorIndex;
};
