#pragma once

#include <libavm-gen/threadpool.h>

namespace Engines
{
class Engines
{
public:
    Engines(const Engines &) = delete;
    Engines &operator=(const Engines &) = delete;

    static Engines &GetInstance();
    ThreadPool *getPool();

private:
    Engines() : m_threadPool(new ThreadPool) { }

    ThreadPool *m_threadPool;
};
}
