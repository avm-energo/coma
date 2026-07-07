#pragma once

#include <libavm-gen/singleton.h>
#include <libavm-gen/threadpool.h>

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
