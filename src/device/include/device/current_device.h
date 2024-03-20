#pragma once

#include <device/bsi.h>
#include <device/bsi_ext.h>
#include <device/configstorage.h>
#include <device/file_provider.h>
#include <device/health.h>
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
    BlockStartupInfo m_bsi, m_previous;
    BlockStartupInfoExtended m_bsiExt;
    ConfigStorage m_cfgStorage;
    S2DataManager m_s2manager;
    FileProvider m_fileProvider;
    QTimer m_timeoutTimer;
    u32 m_bsiCounter;
    bool m_isInitStage;

    /// \brief Приватный конструктор.
    explicit CurrentDevice(AsyncConnection *conn);

    /// \brief Проверяет полученный BSI от устройства с предыдущим.
    /// \details В случае, если BSI отличаются, то класс будет информировать
    /// подписчиков на вышестоящем уровне об изменении определённых полей BSI.
    /// \see typeChanged, healthChanged
    void compareAndUpdate() noexcept;

    /// \brief Отправляет сигнал initBSIFinished со статусом инициализации BSI,
    /// останавливает запущенный таймер для инициализации этого блока.
    void initBSIEvent(const Error::Msg status) noexcept;

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
    /// \brief Возвращает BSI Extended текущего устройства.
    [[nodiscard]] const BlockStartupInfoExtended *bsiExt() const noexcept;
    /// \brief Возвращает блок состояния текущего устройства.
    [[nodiscard]] Health health() const noexcept;

    /// \brief Возвращает хранилище XML настроек текущего устройства.
    [[nodiscard]] ConfigStorage *getConfigStorage() noexcept;
    /// \brief Возвращает хранилище S2 конфигурации текущего устройства.
    [[nodiscard]] S2DataManager *getS2Datamanager() noexcept;
    /// \brief Возвращает провайдер файлов текущего устройства.
    [[nodiscard]] FileProvider *getFileProvider() noexcept;

    /// \brief Возвращает тип базовой платы устройства из блока BSI.
    [[nodiscard]] u16 getBaseType() const noexcept;
    /// \brief Возвращает тип мезонинной платы устройства из блока BSI.
    [[nodiscard]] u16 getMezzType() const noexcept;
    /// \brief Возвращает тип устройства из блока BSI.
    [[nodiscard]] u16 getDeviceType() const noexcept;
    /// \brief Возвращает имя устройства.
    [[nodiscard]] QString getDeviceName() const noexcept;
    /// \brief Возвращает UID устройства из блока BSI.
    [[nodiscard]] QString getUID() const noexcept;
    /// \brief Возвращает серийный номер текущего устройства.
    [[nodiscard]] u32 getSerialNumber() const noexcept;

    /// \brief Проверяет, является ли ВПО устройства устаревшим.
    [[nodiscard]] bool isOutdatedFirmware(const u32 configVersion) const noexcept;

    /// \brief Метод для инициализации BSI.
    void initBSI() noexcept;
    /// \brief Обновление внутренних данных протокола для работы с текущим устройством.
    void internalProtocolUpdate() noexcept;

private slots:
    /// \brief Слот для обновления блока BSI.
    /// \see compareAndUpdate
    void updateBSI(const DataTypes::BitStringStruct &value);

signals:
    /// \brief Сигнал для уведомления о получении блока BSI.
    void bsiReceived();
    /// \brief Сигнал для уведомления об окончании инициализации блока BSI.
    void initBSIFinished(const Error::Msg status);

    /// \brief Сигнал для уведомления об изменении типа устройства.
    void typeChanged(const u16 type);
    /// \brief Сигнал для уведомления об изменении состояния устройства.
    void healthChanged(const u32 health);
    /// \brief Сигнал для уведомления об изменении серийного номера устройства.
    void serialChanged(const u32 serial);
};

/// \brief Фабрика для создания экземпляров класса подключенного устройства.
class DeviceFabric final
{
public:
    /// \brief Метод для создания экземпляров класса подключенного устройства.
    /// \see CurrentDevice, AsyncConnection.
    [[nodiscard]] static CurrentDevice *create(AsyncConnection *connection);
};

} // namespace Device
