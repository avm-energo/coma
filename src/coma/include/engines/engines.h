#pragma once

#include <gen/singleton.h>
#include <gen/threadpool.h>

namespace Engines
{
class Engines : public Singleton<Engines>
{
public:
    Engines()
    {
        m_threadPool = new ThreadPool;
    }
    ThreadPool *getPool();

private:
    ThreadPool *m_threadPool;
};

inline ThreadPool *Engines::getPool()
{
    return GetInstance().m_threadPool;
}
}
