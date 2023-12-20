#pragma once

#include <gen/singleton.h>
#include <interfaces/conn/async_connection.h>
#include <interfaces/conn/sync_connection.h>

namespace Interface
{

class ActiveConnection final : public Singleton<ActiveConnection>
{
private:
    using AsyncConnectionPtr = UniquePointer<AsyncConnection>;
    using SyncConneectionPtr = UniquePointer<SyncConnection>;
    AsyncConnectionPtr m_asyncConnection;
    SyncConneectionPtr m_syncConnection;

public:
    explicit ActiveConnection(token) noexcept;

    AsyncConnection *getAsync() noexcept;
    SyncConnection *getSync() noexcept;
    void updateConnection(AsyncConnectionPtr &&conn) noexcept;
    void resetConnection() noexcept;

    static AsyncConnection *async() noexcept;
    static SyncConnection *sync() noexcept;
    static void update(AsyncConnectionPtr &&conn) noexcept;
    static void reset() noexcept;
};

} // namespace Interface
