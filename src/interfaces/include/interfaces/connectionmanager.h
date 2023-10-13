#pragma once

#include <QTimer>
#include <QWidget>
#include <interfaces/connection.h>
#include <interfaces/connectioncontext.h>
#include <interfaces/types/settingstypes.h>

namespace Interface
{

enum class ReconnectMode : quint8
{
    Loud,
    Silent
};

class ConnectionManager : public QObject
{
    Q_OBJECT
private:
    ConnectionContext m_context;
    Connection *m_currentConnection;
    QTimer *m_silentTimer;
    ReconnectMode m_reconnectMode;
    quint16 m_timeoutCounter, m_timeoutMax;
    quint16 m_errorCounter, m_errorMax;
    bool m_isReconnectEmitted;

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
    /// \details Данный сигнал информирует порт о переходе в режим переподключения
    /// к устройству. Зависит от конкректной реализации дочернего класса BasePort.
    /// \see BasePort.
    void reconnectDevice();
    void reconnectSuccess();

public slots:
    /// \brief Слот для разрыва текущего соединения с устройством.
    void breakConnection();

private slots:
    /// \brief Хэндл для принятия ошибок от порта.
    void handleInterfaceErrors(const InterfaceError error);
    /// \brief Слот для принятия уведомления о том, что
    /// связь с устройством была успешно восстановлена.
    void deviceReconnected();
};

} // namespace Interface

using IfaceConnManager = Interface::ConnectionManager;
