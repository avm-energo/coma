#include "interfaces/connectionmanager.h"

#include <QDebug>
#include <gen/std_ext.h>
#include <interfaces/exec/query_executor_fabric.h>
#include <interfaces/ifaces/ethernet.h>
#include <interfaces/ifaces/serialport.h>
#include <interfaces/ifaces/usbhidport.h>

namespace Interface
{

ConnectionManager::ConnectionManager(QObject *parent)
    : QObject(parent)
    , m_currentConnection(nullptr)
    , m_silentTimer(new QTimer(this))
    , m_reconnectMode(ReconnectMode::Loud)
    , m_isReconnectOccurred(false)
    , m_isInitialBSIRequest(true)
    , m_timeoutCounter(0)
    , m_timeoutMax(5)
    , m_errorCounter(0)
    , m_errorMax(5)
{
    m_silentTimer->setSingleShot(true);
    connect(m_silentTimer, &QTimer::timeout, this, [this] { emit reconnectUI(); });
}

AsyncConnection *ConnectionManager::createConnection(const ConnectStruct &connectionData)
{
    if (m_currentConnection != nullptr)
        breakConnection();
    m_currentConnection = new AsyncConnection(this);
    connect(m_currentConnection, &AsyncConnection::silentReconnectMode, this, //
        [this] { setReconnectMode(ReconnectMode::Silent); });
    m_connBSI = m_currentConnection->connection(this, &ConnectionManager::fastCheckBSI);

    std::visit([this](const auto &settings) { setup(settings); }, connectionData.settings);
    std::visit( // Инициализация контекста для обмена данными
        overloaded {
            [this](const UsbHidSettings &settings) {
                auto interface = new UsbHidPort(settings);
                auto executor = QueryExecutorFabric::makeProtocomExecutor(m_currentConnection->getQueue(), settings);
                m_context.init(interface, executor, Strategy::Sync, Qt::DirectConnection);
            },
            [this](const SerialPortSettings &settings) {
                auto interface = new SerialPort(settings);
                auto executor = QueryExecutorFabric::makeModbusExecutor(m_currentConnection->getQueue(), settings);
                m_context.init(interface, executor, Strategy::Sync, Qt::QueuedConnection);
            },
            [this](const IEC104Settings &settings) {
                auto interface = new Ethernet(settings);
                auto executor = QueryExecutorFabric::makeIec104Executor(m_currentConnection->getQueue(), settings);
                m_context.init(interface, executor, Strategy::Sync, Qt::QueuedConnection);
            },
            [](const EmulatorSettings &settings) {
                /// TODO: доделать
                Q_UNUSED(settings);
            } //
        },
        connectionData.settings);

    connect(m_context.m_iface, &BaseInterface::error, //
        this, &ConnectionManager::handleInterfaceErrors, Qt::QueuedConnection);
    connect(m_context.m_executor, &DefaultQueryExecutor::timeout, //
        this, &ConnectionManager::handleQueryExecutorTimeout);
    connect(this, &ConnectionManager::reconnectInterface, //
        m_context.m_iface, &BaseInterface::reconnect, Qt::QueuedConnection);
    connect(this, &ConnectionManager::reconnectInterface, //
        m_context.m_executor, &DefaultQueryExecutor::reconnectEvent, Qt::QueuedConnection);
    connect(m_context.m_iface, &BaseInterface::reconnected, //
        this, &ConnectionManager::interfaceReconnected, Qt::QueuedConnection);

    m_currentConnection->reqBSI();
    if (!m_context.run(m_currentConnection))
    {
        m_currentConnection->deleteLater();
        m_currentConnection = nullptr;
    }
    return m_currentConnection;
}

void ConnectionManager::setup(const BaseSettings &settings) noexcept
{
    m_silentTimer->setInterval(settings.m_silentInterval);
    m_errorMax = settings.m_maxErrors;
    m_timeoutMax = settings.m_maxTimeouts;
}

void ConnectionManager::setReconnectMode(const ReconnectMode newMode) noexcept
{
    m_reconnectMode = newMode;
}

void ConnectionManager::reconnect()
{
    if (!m_isReconnectOccurred)
    {
        qCritical() << "Произошла ошибка соединения";
        emit reconnectInterface();
        if (m_reconnectMode == ReconnectMode::Loud)
            emit reconnectUI();
        else
            m_silentTimer->start();
        m_isReconnectOccurred = true;
    }
}

void ConnectionManager::breakConnection()
{
    m_context.reset();
    m_currentConnection = nullptr;
    m_isInitialBSIRequest = true;
}

void ConnectionManager::handleInterfaceErrors(const InterfaceError error)
{
    switch (error)
    {
    case InterfaceError::ReadError:
    case InterfaceError::WriteError:
        ++m_errorCounter;
        if (m_errorCounter > m_errorMax && !m_isReconnectOccurred)
            reconnect();
        break;
    case InterfaceError::OpenError:
        if (m_isInitialBSIRequest)
        {
            QString errMsg("Произошла ошибка открытия интерфейса. Disconnect...");
            qCritical() << errMsg;
            emit connectFailed(errMsg);
        }
        break;
    }
}

void ConnectionManager::handleQueryExecutorTimeout()
{
    ++m_timeoutCounter;
    if (m_isInitialBSIRequest)
    {
        QString errMsg("Превышено время ожидания блока BSI. Disconnect...");
        qCritical() << errMsg;
        emit connectFailed(errMsg);
        breakConnection();
    }
    if (m_timeoutCounter > m_timeoutMax && !m_isReconnectOccurred)
        reconnect();
}

void ConnectionManager::fastCheckBSI(const DataTypes::BitStringStruct &data)
{
    // fast checking
    if (data.sigAdr == addr::bsiStartReg)
    {
        if (m_isInitialBSIRequest)
        {
            m_isInitialBSIRequest = false;
            emit connectSuccesfull();
        }
        else if (m_isReconnectOccurred)
        {
            /// TODO: проверять BSI
            // при реконнекте отключить устройство и подключить другое -> что будет? (modbus same)
            if (m_reconnectMode == ReconnectMode::Silent)
                m_silentTimer->stop();
            setReconnectMode(ReconnectMode::Loud);
            m_errorCounter = 0;
            m_timeoutCounter = 0;
            qCritical() << "Соединение восстановлено";
            m_isReconnectOccurred = false;
            emit reconnectSuccess(); // Сообщаем, что переподключение прошло успешно
        }
        disconnect(m_connBSI);
    }
}

void ConnectionManager::interfaceReconnected()
{
    m_connBSI = m_currentConnection->connection(this, &ConnectionManager::fastCheckBSI);
    m_context.m_executor->run();
    m_currentConnection->reqBSI();
}

} // namespace Interface
