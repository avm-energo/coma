#pragma once

#include <QThread>
#include <QThreadPool>
#include <interfaces/connection.h>
#include <interfaces/ports/baseport.h>
#include <interfaces/threads/baseconnectionthread.h>

namespace Interface
{

enum class Strategy : quint8
{
    Sync = 0,
    Async,
    None = 0xff
};

class ConnectionContext
{
    friend class ConnectionManager;

private:
    BasePort *m_port;
    BaseConnectionThread *m_parser;
    Strategy m_strategy;
    std::pair<QThread *, QThread *> m_syncThreads;
    QThreadPool *m_threadPool;

public:
    explicit ConnectionContext() noexcept;

    bool isValid() const noexcept;

    /// \brief Инициализация контекста соединения с устройством.
    /// \param connPolicy [in] - для взаимодействия порта и парсера используются
    /// по USB и последовательному порту используются разные типы коммуникации.
    /// Причина: использование QSerialPort, с которым нельзя работать из разных потоков.
    void init(BasePort *port, BaseConnectionThread *parser, //
        const Strategy strategy, const Qt::ConnectionType connPolicy);
    /// \brief Запускает соединение для текущего инициализированного контекста.
    bool run(Connection *connection);
    /// \brief Сбрасывает текущий контекст и закрывает активное соединение.
    void reset();
};

} // namespace Interface
