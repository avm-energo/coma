#pragma once

#include <QTimer>
#include <QWidget>
#include <interfaces/connection.h>
#include <interfaces/connectioncontext.h>
#include <interfaces/types/settingstypes.h>

namespace Interface
{

/// \brief Перечисление для описания режима переподключения к устройству
enum class ReconnectMode : quint8
{
    /// Громкий режим: менеджер соединений сообщает UI сразу о том,
    /// что устройство не выходит на связь.
    Loud,
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
    Connection *m_currentConnection;
    QTimer *m_silentTimer;
    ReconnectMode m_reconnectMode;
    bool m_isReconnectEmitted;
    quint16 m_timeoutCounter, m_timeoutMax;
    quint16 m_errorCounter, m_errorMax;

    void reconnect();

public:
    explicit ConnectionManager(QWidget *parent = nullptr);
    void createConnection(const ConnectStruct &connectionData);
    void setReconnectMode(const ReconnectMode newMode) noexcept;

signals:
    /// \brief Сигнал, который вызывается, если соединение к устройству произошло успешно.
    void connectSuccesfull();
    /// \brief Сигнал, который вызывается, если соединение к устройству провалилось.
    void connectFailed();
    /// \brief Сигнал, который вызывается при переподключении к устройству.
    /// \details Данный сигнал передаётся родительскому окну для отрисовки виджета
    /// и информарования пользователя о проблемах связи с устройством.
    void reconnectUI();
    /// \brief Сигнал, который вызывается при переподключении к устройству.
    /// \details Данный сигнал информирует интерфейс о переходе в режим переподключения к устройству.
    /// \see BasePort.
    void reconnectDevice();
    void reconnectSuccess();

public slots:
    /// \brief Слот для разрыва текущего соединения с устройством.
    void breakConnection();

private slots:
    /// \brief Хэндл для принятия ошибок от интерфейса.
    void handleInterfaceErrors(const InterfaceError error);
    /// \brief Слот для принятия уведомления о том, что
    /// связь с устройством была успешно восстановлена.
    void deviceReconnected();
};

} // namespace Interface

using IfaceConnManager = Interface::ConnectionManager;
