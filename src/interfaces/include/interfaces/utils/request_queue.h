#pragma once

#include <atomic>
#include <condition_variable>
#include <gen/std_ext.h>
#include <interfaces/types/common_types.h>
#include <interfaces/utils/unique_queue.h>
#include <mutex>
#include <optional>
#include <queue>

namespace std
{

template <> struct hash<std::reference_wrapper<Interface::CommandStruct>>
{
    std::uint64_t operator()(const std::reference_wrapper<Interface::CommandStruct> &value) const
    {
        // computes the Fowler-Noll-Vo hash function
        constexpr std::uint64_t prime { 0x100000001B3 };
        std::uint64_t result { 0xcbf29ce484222325 }, i = 0, ie = 0;
        const Interface::CommandStruct &ref = value.get();
        const auto first = ref.arg1.toByteArray();
        const auto second = ref.arg2.toByteArray();
        for (i = 0, ie = first.size(); i != ie; ++i)
            result = (result * prime) ^ first.at(i);
        for (i = 0, ie = second.size(); i != ie; ++i)
            result = (result * prime) ^ second.at(i);
        result = result ^ (std_ext::to_underlying(ref.command) << 1);
        return result;
    }
};

} // namespace std

namespace Interface
{

/// \brief Структура для представления очереди запросов к устройству.
class RequestQueue
{
private:
    Utils::UniqueQueue<CommandStruct> m_requests;
    std::mutex m_queueAccess;
    std::condition_variable m_cvQueueEmpty;
    std::atomic_bool m_isActive = true;

public:
    /// \brief Конструктор по умолчанию.
    explicit RequestQueue() noexcept;

    /// \brief Функция для добавления команды в очередь запросов.
    void addToQueue(CommandStruct &&request) noexcept;
    /// \brief Функция для взятия команды из очереди запросов.
    std::optional<CommandStruct> getFromQueue() noexcept;
    /// \brief Функция для очистки очереди запросов.
    void clear() noexcept;
    std::size_t size() noexcept;

    /// \brief Функция для ожидания момента, когда в пустую очередь попадёт запрос.
    void waitFillingQueue() noexcept;

    /// \brief Активирует очередь.
    /// \details В активном состоянии входящие запросы сохраняются в очереди.
    /// \see deactivate, isActive.
    void activate() noexcept;
    /// \brief Деактивирует очередь.
    /// \details В неактивном состоянии входящие запросы не сохраняются в очереди (игнорируются).
    /// \see activate, isActive.
    void deactivate() noexcept;
    /// \brief Возвращает текущее состояние, в котором находится очередь запросов.
    /// \see activate, deactivate.
    bool isActive() const noexcept;
};

}
