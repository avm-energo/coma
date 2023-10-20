#include "interfaces/connectionmanager.h"

#include <gen/std_ext.h>
#include <interfaces/ifaces/serialport.h>
#include <interfaces/ifaces/usbhidport.h>
#include <interfaces/parsers/device_query_executor.h>

namespace Interface
{

ConnectionManager::ConnectionManager(QWidget *parent)
    : QObject(parent)
    , m_currentConnection(nullptr)
    , m_silentTimer(new QTimer(this))
    , m_reconnectMode(ReconnectMode::Loud)
    , m_isReconnectEmitted(false)
    , m_timeoutCounter(0)
    , m_errorCounter(0)
{
    /// TODO: брать значение из настроек
    m_silentTimer->setInterval(10000);
    m_errorMax = 5;
    m_timeoutMax = 5;

    m_silentTimer->setSingleShot(true);
    connect(m_silentTimer, &QTimer::timeout, this, [this] { emit reconnectUI(); });
}

void ConnectionManager::createConnection(const ConnectStruct &connectionData)
{
    if (m_currentConnection != nullptr)
        breakConnection();
    m_currentConnection = new Connection(this);
    connect(m_currentConnection, &Connection::silentReconnectMode, this, //
        [this] { setReconnectMode(ReconnectMode::Silent); });

    std::visit( // Инициализация контекста для обмена данными
        overloaded {
            [this](const UsbHidSettings &settings) {
                auto interface = new UsbHidPort(settings);
                auto executor = DeviceQueryExecutor::makeProtocomExecutor(m_currentConnection->m_queue);
                m_context.init(interface, executor, Strategy::Sync, Qt::DirectConnection);
            },
            [this](const SerialPortSettings &settings) {
                auto interface = new SerialPort(settings);
                /// TODO: device address
                auto executor = DeviceQueryExecutor::makeModbusExecutor(m_currentConnection->m_queue);
                m_context.init(interface, executor, Strategy::Sync, Qt::QueuedConnection);
            },
            [this](const IEC104Settings &settings) {
                Q_UNUSED(settings);
                Q_UNUSED(settings);
            },
            [this](const EmulatorSettings &settings) {
#ifdef ENABLE_EMULATOR
                Q_UNUSED(settings);
#endif
                Q_UNUSED(settings);
            } //
        },
        connectionData.settings);

    connect(m_context.m_iface, &BaseInterface::error, //
        this, &ConnectionManager::handleInterfaceErrors, Qt::QueuedConnection);
    connect(this, &ConnectionManager::reconnectDevice, //
        m_context.m_iface, &BaseInterface::reconnect, Qt::QueuedConnection);
    connect(m_context.m_iface, &BaseInterface::reconnected, //
        this, &ConnectionManager::deviceReconnected, Qt::QueuedConnection);

    if (m_context.run(m_currentConnection))
        emit connectSuccesfull();
    else
        emit connectFailed();
    Connection::setIface(Connection::InterfacePointer { m_currentConnection });
}

void ConnectionManager::setReconnectMode(const ReconnectMode newMode) noexcept
{
    m_reconnectMode = newMode;
}

void ConnectionManager::reconnect()
{
    if (!m_isReconnectEmitted)
    {
        emit reconnectDevice();
        if (m_reconnectMode == ReconnectMode::Loud)
            emit reconnectUI();
        else
            m_silentTimer->start();
        m_isReconnectEmitted = true;
    }
}

void ConnectionManager::breakConnection()
{
    m_context.reset();
    m_currentConnection = nullptr;
    Connection::s_connection.reset();
}

void ConnectionManager::handleInterfaceErrors(const InterfaceError error)
{
    switch (error)
    {
    case InterfaceError::ReadError:
    case InterfaceError::WriteError:
        ++m_errorCounter;
        if (m_errorCounter > m_errorMax)
            reconnect();
        break;
    case InterfaceError::Timeout:
        ++m_timeoutCounter;
        if (m_context.m_executor->getLastRequestedCommand() == Commands::C_ReqBSI)
        {
            qCritical() << "Превышено время ожидания блока BSI. Disconnect...";
            breakConnection();
        }
        if (m_timeoutCounter > m_timeoutMax)
            reconnect();
    }
}

void ConnectionManager::deviceReconnected()
{
    if (m_reconnectMode == ReconnectMode::Silent)
        m_silentTimer->stop();
    setReconnectMode(ReconnectMode::Loud);
    m_errorCounter = 0;
    m_timeoutCounter = 0;
    m_isReconnectEmitted = false;
    emit reconnectSuccess(); // Сообщаем, что переподключение прошло успешно
}

} // namespace Interface
