#include <device/bsi.h>
#include <device/configstorage.h>
#include <interfaces/conn/async_connection.h>
#include <interfaces/conn/sync_connection.h>
#include <s2/s2datamanager.h>

namespace Device
{

using AsyncConnection = Interface::AsyncConnection;
using SyncConnection = Interface::SyncConnection;

/// \brief Класс для представления подключенного устройства.
class CurrentDevice final : public QObject
{
    Q_OBJECT
private:
    friend class DeviceFabric;

    AsyncConnection *m_async;
    SyncConnection m_sync;
    BsiHolder m_holder;
    ConfigStorage m_cfgStorage;
    S2DataManager m_s2manager;

    /// \brief Приватный конструктор.
    explicit CurrentDevice(AsyncConnection *conn);

public:
    explicit CurrentDevice() = delete;
    explicit CurrentDevice(const CurrentDevice &other) = delete;
    CurrentDevice &operator=(const CurrentDevice &other) = delete;

    /// \brief Возвращает асинхронное соединение для текущего устройства.
    [[nodiscard]] AsyncConnection *async() noexcept;
    /// \brief Возвращает синхронное соединение для текущего устройства.
    [[nodiscard]] SyncConnection *sync() noexcept;
    /// \brief Возвращает BSI текущего устройства.
    [[nodiscard]] const BlockStartupInfo &bsi() noexcept;

    /// \brief Возвращает хранилище BSI текущего устройства.
    [[nodiscard]] BsiHolder *getBsiHolder() noexcept;
    /// \brief Возвращает хранилище XML настроек текущего устройства.
    [[nodiscard]] ConfigStorage *getConfigStorage() noexcept;
    /// \brief Возвращает хранилище S2 конфигурации текущего устройства.
    [[nodiscard]] S2DataManager *getS2Datamanager() noexcept;
};

/// \brief Фабрика для создания экземпляров класса подключенного устройства.
class DeviceFabric final
{
public:
    [[nodiscard]] static CurrentDevice *create(AsyncConnection *connection);
};

} // namespace Device
