#include <engines/engines.h>

namespace Engines
{
Engines &Engines::GetInstance()
{
    static Engines instance;
    return instance;
}

ThreadPool *Engines::getPool()
{
    return m_threadPool;
}
}
