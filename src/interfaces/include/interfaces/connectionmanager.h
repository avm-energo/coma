#pragma once

#include <QTimer>
#include <interfaces/conn/async_connection.h>
#include <interfaces/connectioncontext.h>
#include <interfaces/types/settingstypes.h>

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
    bool m_isReconnectOccurred;
    quint16 m_timeoutCounter, m_timeoutMax;
    quint16 m_errorCounter, m_errorMax;

    void reconnect();
    void setup(const BaseSettings &settings) noexcept;

public:
    explicit ConnectionManager(QObject *parent = nullptr);
    AsyncConnection *createConnection(const ConnectStruct &connectionData);
    void setReconnectMode(const ReconnectMode newMode) noexcept;

signals:
    /// \brief Сигнал, который вызывается, если соединение к устройству произошло успешно.
    // void connectSuccesfull();
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

public slots:
    /// \brief Слот для разрыва текущего соединения с устройством.
    void breakConnection();

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
