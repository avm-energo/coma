#pragma once

#include "error.h"
#include "singleton.h"

#include <vector>
class ErrorQueue : public QObject, public Singleton<ErrorQueue>
{
    Q_OBJECT
public:
    explicit ErrorQueue(token);
    const std::vector<ErrorMsg> *errMsgPool();
    ErrorMsg popError();
    void pushError(const ErrorMsg &msg);

    void clearCounter();
    auto counter() const
    {
        return m_newErrors;
    }
signals:
    void errCounts(size_t size);

private:
    std::vector<ErrorMsg> m_errMsgPool;
    size_t m_newErrors;
};
