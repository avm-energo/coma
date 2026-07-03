#pragma once

#include <interfaces/conn/async_connection.h>
#include <interfaces/connectioncontext.h>
#include <interfaces/types/connection_settings.h>

#include <QTimer>

namespace Interface
{

/// \brief Перечисление для описания режима переподключения к устройству
enum class ReconnectMode : quint8
{
    /// Громкий режим: менеджер соединений сообщает UI сразу о том,
    /// что устройство не выходит на связь.
    Loud = 0,
    /// Тихий режим: менеджер соединений в течение 10 секунд пытается
    /// восстановить связь с устройством, и если оно не вышло на связь за этот промежуток времени,
    /// то он сообщает UI о том, что устройство не выходит на связь. Может
    /// использоваться для конфигурирования устройства и записи ВПО.
    Silent
};

/// \brief Класс, описывающий менеджер соединений.
class ConnectionManager : public QObject
{
    Q_OBJECT
private:
    ConnectionContext m_context;
    AsyncConnection *m_currentConnection;
    QTimer *m_silentTimer;
    QMetaObject::Connection m_connBSI;
    ReconnectMode m_reconnectMode;
    bool m_isReconnectOccurred, m_isInitial;
    /// \brief true, пока цепочка BaseInterface::reconnect() физически не завершится
    /// (успешным connect() или сменой состояния). В отличие от m_isReconnectOccurred,
    /// не ждёт подтверждения BSI — сбрасывается сразу по interfaceReconnected(),
    /// чтобы новая ошибка могла запустить свежую попытку, даже если BSI так и не ответил.
    bool m_isInterfaceReconnecting;
    quint16 m_timeoutCounter, m_timeoutMax;
    quint16 m_errorCounter, m_errorMax;

    void setup(const BaseSettings *settings) noexcept;

public:
    explicit ConnectionManager(QObject *parent = nullptr);
    AsyncConnection *createConnection(const ConnectionSettings &connectionData);
    void setReconnectMode(const ReconnectMode newMode) noexcept;
    /// \brief Принудительно запускает переподключение к устройству, не дожидаясь ошибки интерфейса
    /// или таймаута запроса. Нужен для команд, после которых устройство заведомо ненадолго пропадает
    /// с шины (например, переход на новое ВПО, см. C_StartFirmwareUpgrade) — физическое отключение по
    /// USB в таком случае не всегда доходит до менеджера соединений как явная ошибка интерфейса.
    void reconnect();

signals:
    /// \brief Сигнал, который вызывается, если соединение к устройству провалилось.
    void connectFailed(const QString &message);
    /// \brief Сигнал, который вызывается при переподключении к устройству.
    /// \details Данный сигнал передаётся родительскому окну для отрисовки виджета
    /// и информарования пользователя о проблемах связи с устройством.
    void reconnectUI();
    /// \brief Сигнал, который вызывается при переподключении к устройству.
    /// \details Данный сигнал информирует интерфейс о переходе в режим переподключения к устройству.
    /// \see BasePort.
    void reconnectInterface();
    void reconnectSuccess();
    /// \brief Settings changed signal for USB interface
    void usbSettingsChanged(const QString &key, const QVariant &value);
    /// \brief Settings changed signal for USB interface
    void modbusSettingsChanged(const QString &key, const QVariant &value);
    /// \brief Settings changed signal for USB interface
    void iec104SettingsChanged(const QString &key, const QVariant &value);

public slots:
    /// \brief Слот для разрыва текущего соединения с устройством.
    void breakConnection();
    /// \brief slot for settings changes
    void settingsChanged(const QString &key, const QVariant &value);

private slots:
    /// \brief Слот для принятия ошибок от интерфейса.
    void handleInterfaceErrors(const InterfaceError error);
    /// \brief Слот для принятий уведомлений о таймаутах от исполнителя запросов.
    void handleQueryExecutorTimeout();
    void fastCheckBSI(const DataTypes::BitStringStruct &data);

    /// \brief Слот для принятия уведомления о том, что
    /// связь с устройством была успешно восстановлена.
    void interfaceReconnected();
};

} // namespace Interface

using IfaceConnManager = Interface::ConnectionManager;
