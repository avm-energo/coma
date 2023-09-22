#pragma once

#include <gen/error.h>
#include <interfaces/types/common_types.h>
#include <mutex>
#include <optional>
#include <queue>

namespace Interface
{

class RequestQueue
{
private:
    std::queue<CommandStruct> m_requests;
    std::mutex m_queueAccess;

public:
    explicit RequestQueue() noexcept;

    void addToQueue(CommandStruct &&request);
    std::optional<CommandStruct> deQueue();
};

}
