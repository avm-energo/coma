#include <interfaces/utils/request_queue.h>

namespace Interface
{

RequestQueue::RequestQueue() noexcept = default;

void RequestQueue::addToQueue(CommandStruct &&request)
{
    if (isActive())
    {
        std::lock_guard<std::mutex> locker { m_queueAccess };
        m_requests.push(std::move(request));
    }
}

std::optional<CommandStruct> RequestQueue::deQueue()
{
    std::lock_guard<std::mutex> locker { m_queueAccess };
    if (!m_requests.empty())
    {
        CommandStruct retVal { m_requests.front() };
        m_requests.pop();
        return retVal;
    }
    return std::nullopt;
}

void RequestQueue::activate() noexcept
{
    m_isActive.store(true);
}

void RequestQueue::deactivate() noexcept
{
    m_isActive.store(false);
}

bool RequestQueue::isActive() const noexcept
{
    return m_isActive.load();
}

} // namespace Interface
