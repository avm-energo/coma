#include <interfaces/utils/request_queue.h>

namespace Interface
{

RequestQueue::RequestQueue() noexcept = default;

void RequestQueue::addToQueue(CommandStruct &&request)
{
    std::lock_guard<std::mutex> locker { m_queueAccess };
    m_requests.push(std::move(request));
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

} // namespace Interface
