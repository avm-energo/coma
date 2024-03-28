#include <chrono>
#include <interfaces/utils/request_queue.h>

namespace Interface
{

RequestQueue::RequestQueue() noexcept = default;

void RequestQueue::addToQueue(CommandStruct &&request) noexcept
{
    if (isActive())
    {
        std::lock_guard<std::mutex> locker { m_queueAccess };
        m_requests.push(std::move(request));
        m_cvQueueEmpty.notify_all();
    }
}

std::optional<CommandStruct> RequestQueue::getFromQueue() noexcept
{
    std::lock_guard<std::mutex> locker { m_queueAccess };
    if (!m_requests.empty())
    {
        CommandStruct retVal { m_requests.front() };
        m_requests.pop();
        return std::optional<CommandStruct> { std::move(retVal) };
    }
    return std::nullopt;
}

void RequestQueue::clear() noexcept
{
    std::lock_guard<std::mutex> locker { m_queueAccess };
    Utils::UniqueQueue<CommandStruct> empty;
    m_requests.swap(empty);
}

std::size_t RequestQueue::size() noexcept
{
    return m_requests.size();
}

void RequestQueue::waitFillingQueue() noexcept
{
    std::unique_lock<std::mutex> locker { m_queueAccess };
    m_cvQueueEmpty.wait(locker, [this] { return !m_requests.empty(); });
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
