#pragma once

#include <atomic>
#include <gen/error.h>
#include <interfaces/types/common_types.h>
#include <mutex>
#include <optional>
#include <queue>

namespace Interface
{

/// \brief Структура для представления очереди запросов к устройству.
class RequestQueue
{
private:
    std::queue<CommandStruct> m_requests;
    std::mutex m_queueAccess;
    std::atomic_bool m_isActive = true;

public:
    /// \brief Конструктор по умолчанию.
    explicit RequestQueue() noexcept;

    /// \brief Функция для добавления команды в очередь запросов.
    void addToQueue(CommandStruct &&request);
    /// \brief Функция для взятия команды из очереди запросов.
    std::optional<CommandStruct> deQueue();

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
