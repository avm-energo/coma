#include "interfaces/connectioncontext.h"

#include <interfaces/connection.h>
#include <interfaces/device_query_executor.h>

namespace Interface
{

ConnectionContext::ConnectionContext() noexcept : m_iface(nullptr), m_executor(nullptr), m_strategy(Strategy::None)
{
}

bool ConnectionContext::isValid() const noexcept
{
    return (m_iface != nullptr && m_executor != nullptr);
}

void ConnectionContext::init(BaseInterface *iface, DeviceQueryExecutor *executor, //
    const Strategy strategy, const Qt::ConnectionType connPolicy)
{
    m_iface = iface;
    m_executor = executor;
    m_strategy = strategy;

    if (isValid() && m_strategy != Strategy::None)
    {
        // Обмен данными
        QObject::connect(m_iface, &BaseInterface::dataReceived, //
            m_executor, &DeviceQueryExecutor::receiveDataFromInterface, Qt::QueuedConnection);
        QObject::connect(m_executor, &DeviceQueryExecutor::sendDataToInterface, //
            m_iface, &BaseInterface::writeData, connPolicy);
        // Отмена команды
        QObject::connect(m_iface, &BaseInterface::clearQueries, //
            m_executor, &DeviceQueryExecutor::cancelQuery, connPolicy);
        // Конец работы
        QObject::connect(m_iface, &BaseInterface::finished, m_executor, &DeviceQueryExecutor::finished, connPolicy);

        if (m_strategy == Strategy::Sync)
        {
            auto ifaceThread = new QThread;
            auto parserThread = new QThread;
            // Старт
            QObject::connect(ifaceThread, &QThread::started, m_iface, &BaseInterface::poll);
            QObject::connect(parserThread, &QThread::started, m_executor, &DeviceQueryExecutor::exec);
            // Остановка
            QObject::connect(m_iface, &BaseInterface::finished, ifaceThread, &QThread::quit);
            QObject::connect(m_iface, &BaseInterface::finished, parserThread, &QThread::quit);
            QObject::connect(m_executor, &DeviceQueryExecutor::finished, parserThread, &QThread::quit);
            QObject::connect(ifaceThread, &QThread::finished, m_iface, &QObject::deleteLater);
            QObject::connect(parserThread, &QThread::finished, m_executor, &QObject::deleteLater);
            QObject::connect(ifaceThread, &QThread::finished, &QObject::deleteLater);
            QObject::connect(parserThread, &QThread::finished, &QObject::deleteLater);
            // Если интерфейс успешно запустился
            QObject::connect(iface, &BaseInterface::started, m_iface, [=] {
                qInfo() << m_iface->metaObject()->className() << " connected";
                executor->moveToThread(parserThread);
                iface->moveToThread(ifaceThread);
                parserThread->start();
                ifaceThread->start();
            });
            m_syncThreads.first = ifaceThread;
            m_syncThreads.second = parserThread;
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
    // Если нет интерфейсы, парсера или в качестве соединения передан
    // nullptr, то прерываем запуск контекста.
    if (!isValid() || connection == nullptr)
        return false;

    // Обмен данными для соединения
    QObject::connect(m_executor, &DeviceQueryExecutor::responseSend, //
        connection, &Connection::responseHandle, Qt::DirectConnection);
    QObject::connect(m_iface, &BaseInterface::stateChanged, connection, //
        &Connection::stateChanged, Qt::QueuedConnection);

    if (m_strategy == Strategy::Sync)
    {
        if (!m_iface->connect())
        {
            m_iface->close();
            m_iface->deleteLater();
            m_executor->deleteLater();
            m_syncThreads.first->deleteLater();
            m_syncThreads.second->deleteLater();
            return false;
        }
        m_executor->run();
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
    if (isValid())
    {
        m_iface->close();
        m_executor->stop();
        if (m_strategy == Strategy::Sync)
            StdFunc::Wait(20);
        m_iface = nullptr;
        m_executor = nullptr;
    }
    m_strategy = Strategy::None;
}

} // namespace Interface
