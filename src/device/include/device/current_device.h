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
    BlockStartupInfo m_bsi;
    ConfigStorage m_cfgStorage;
    S2DataManager m_s2manager;
    u32 m_bsiCounter;

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
    [[nodiscard]] const BlockStartupInfo &bsi() const noexcept;

    /// \brief Возвращает хранилище XML настроек текущего устройства.
    [[nodiscard]] ConfigStorage *getConfigStorage() noexcept;
    /// \brief Возвращает хранилище S2 конфигурации текущего устройства.
    [[nodiscard]] S2DataManager *getS2Datamanager() noexcept;

    /// \brief Возвращает тип устройства из блока BSI.
    [[nodiscard]] u16 getDeviceType() const noexcept;
    /// \brief Возвращает имя устройства.
    [[nodiscard]] QString getDeviceName() const noexcept;
    /// \brief Возвращает UID устройства из блока BSI.
    [[nodiscard]] QString getUID() const noexcept;
    /// \brief Проверяет, является ли ВПО устройства устаревшим.
    [[nodiscard]] bool isOutdatedFirmware(const u32 configVersion) const noexcept;

private slots:
    void updateBSI(const DataTypes::BitStringStruct &value);

signals:
    void healthChanged(const u32 health);
    void bsiReady();
};

/// \brief Фабрика для создания экземпляров класса подключенного устройства.
class DeviceFabric final
{
public:
    [[nodiscard]] static CurrentDevice *create(AsyncConnection *connection);
};

} // namespace Device
