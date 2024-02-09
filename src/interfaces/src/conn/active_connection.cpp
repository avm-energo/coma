#include "interfaces/conn/active_connection.h"

namespace Interface
{

ActiveConnection::ActiveConnection(token) noexcept : m_asyncConnection(nullptr), m_syncConnection(nullptr)
{
}

AsyncConnection *ActiveConnection::getAsync() noexcept
{
    Q_ASSERT(m_asyncConnection.get() != nullptr);
    return m_asyncConnection.get();
}

SyncConnection *ActiveConnection::getSync() noexcept
{
    Q_ASSERT(m_syncConnection.get() != nullptr);
    return m_syncConnection.get();
}

void ActiveConnection::updateConnection(AsyncConnectionPtr &&conn) noexcept
{
    m_asyncConnection = std::move(conn);
    m_syncConnection.reset(new SyncConnection(m_asyncConnection.get()));
}

void ActiveConnection::resetConnection() noexcept
{
    m_syncConnection.reset();
    m_asyncConnection.reset();
}

AsyncConnection *ActiveConnection::async() noexcept
{
    return ActiveConnection::GetInstance().getAsync();
}

SyncConnection *ActiveConnection::sync() noexcept
{
    return ActiveConnection::GetInstance().getSync();
}

void ActiveConnection::update(AsyncConnectionPtr &&conn) noexcept
{
    ActiveConnection::GetInstance().updateConnection(std::move(conn));
}

void ActiveConnection::reset() noexcept
{
    ActiveConnection::GetInstance().resetConnection();
}

} // namespace Interface
