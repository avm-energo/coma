#include "interfaces/connectioncontext.h"

namespace Interface
{

ConnectionContext::ConnectionContext() noexcept : m_port(nullptr), m_parser(nullptr), m_strategy(Strategy::None)
{
}

void ConnectionContext::init(BasePort *port, BaseConnectionThread *parser, //
    const Strategy strategy, const Qt::ConnectionType connPolicy)
{
    m_port = port;
    m_parser = parser;
    m_strategy = strategy;

    if (m_port != nullptr && m_parser != nullptr && m_strategy != Strategy::None)
    {
        // Обмен данными
        QObject::connect(m_port, &BasePort::dataReceived, //
            m_parser, &BaseConnectionThread::processReadBytes, connPolicy);
        QObject::connect(m_parser, &BaseConnectionThread::sendDataToPort, m_port, &BasePort::writeDataSync, connPolicy);
        // Отмена команды
        QObject::connect(m_port, &BasePort::clearQueries, m_parser, &BaseConnectionThread::clear, connPolicy);
        // Конец работы
        QObject::connect(m_port, &BasePort::finished, m_parser, &BaseConnectionThread::wakeUp, connPolicy);
        QObject::connect(m_port, &BasePort::finished, m_parser, &BaseConnectionThread::finished, connPolicy);

        if (m_strategy == Strategy::Sync)
        {
            auto portThread = new QThread;
            auto parseThread = new QThread;
            // Старт
            QObject::connect(portThread, &QThread::started, m_port, &BasePort::poll);
            QObject::connect(parseThread, &QThread::started, m_parser, &BaseConnectionThread::run);
            // Остановка
            QObject::connect(m_port, &BasePort::stateChanged, //
                m_parser, &BaseConnectionThread::setState, Qt::DirectConnection);
            QObject::connect(m_port, &BasePort::finished, portThread, &QThread::quit);
            QObject::connect(m_port, &BasePort::finished, parseThread, &QThread::quit);
            QObject::connect(m_parser, &BaseConnectionThread::finished, parseThread, &QThread::quit);
            QObject::connect(portThread, &QThread::finished, m_port, &QObject::deleteLater);
            QObject::connect(parseThread, &QThread::finished, m_parser, &QObject::deleteLater);
            QObject::connect(portThread, &QThread::finished, &QObject::deleteLater);
            QObject::connect(parseThread, &QThread::finished, &QObject::deleteLater);
            // Если порт успешно запустился
            QObject::connect(port, &BasePort::started, m_port, [=] {
                qInfo() << m_port->metaObject()->className() << " connected";
                parser->moveToThread(parseThread);
                port->moveToThread(portThread);
                parseThread->start();
                portThread->start();
            });
            m_syncThreads.first = portThread;
            m_syncThreads.second = parseThread;
        }
        else
        {
            m_threadPool = QThreadPool::globalInstance();
            /// TODO: для 104 используем асинхронную модель обмена данными
        }
    }
}

bool ConnectionContext::run(Connection *connection)
{
    // Обмен данными для соединения
    QObject::connect(connection, &Connection::wakeUpParser, //
        m_parser, &BaseConnectionThread::wakeUp, Qt::DirectConnection);
    QObject::connect(m_parser, &BaseConnectionThread::responseSend, //
        connection, &Connection::responseHandle, Qt::DirectConnection);
    QObject::connect(m_port, &BasePort::stateChanged, connection, //
        &Connection::stateChanged, Qt::QueuedConnection);

    if (m_strategy == Strategy::Sync)
    {
        if (!m_port->connect())
        {
            m_port->closeConnection();
            m_port->deleteLater();
            m_parser->deleteLater();
            m_syncThreads.first->deleteLater();
            m_syncThreads.second->deleteLater();
            return false;
        }
        return true;
    }
    else
    {
        /// TODO: для 104 используем асинхронную модель обмена данными
        return false;
    }
}

void ConnectionContext::reset()
{
    if (m_port != nullptr && m_parser != nullptr)
    {
        m_port->closeConnection();
        m_parser->wakeUp();
        if (m_strategy == Strategy::Sync)
            StdFunc::Wait(20);
        m_port = nullptr;
        m_parser = nullptr;
    }
    m_strategy = Strategy::None;
}

} // namespace Interface
