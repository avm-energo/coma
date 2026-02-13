#include <common/names.h>
#include <device/bsi.h>
#include <avm-gen/std_ext.h>
#include <interfaces/conn/sync_connection.h>
#include <interfaces/connectionmanager.h>
#include <interfaces/exec/query_executor_fabric.h>
#include <interfaces/ifaces/ethernet.h>
#include <interfaces/ifaces/serialport.h>
#include <interfaces/ifaces/usbhidport.h>

#include <QDebug>

namespace Interface
{

ConnectionManager::ConnectionManager(QObject *parent)
    : QObject(parent)
    , m_currentConnection(nullptr)
    , m_silentTimer(new QTimer(this))
    , m_reconnectMode(ReconnectMode::Loud)
    , m_isReconnectOccurred(false)
    , m_isInitial(true)
    , m_errorCounter(0)
    , m_errorMax(5)
{
    m_silentTimer->setSingleShot(true);
    connect(m_silentTimer, &QTimer::timeout, this, [this] { emit reconnectUI(); });
}

AsyncConnection *ConnectionManager::createConnection(const ConnectionSettings &connectionData)
{
    if (m_currentConnection != nullptr)
        breakConnection();
    m_currentConnection = new AsyncConnection(this);
    connect(
        m_currentConnection, &AsyncConnection::silentReconnectMode, this, //
        [this] { setReconnectMode(ReconnectMode::Silent); }, Qt::DirectConnection);
    m_connBSI = m_currentConnection->connection(this, &ConnectionManager::fastCheckBSI);

    std::visit([this](auto &settings) { setup(settings); }, connectionData.settings);
    std::visit( // Инициализация контекста для обмена данными
        overloaded {
            [this](BaseSettings *settings)
            {
                UsbHidSettings *concSettings = qobject_cast<UsbHidSettings *>(settings);
                auto interface = new UsbHidPort(concSettings);
                auto executor
                    = QueryExecutorFabric::makeProtocomExecutor(m_currentConnection->getQueue(), concSettings);
                connect(this, &ConnectionManager::usbSettingsChanged, executor, &DefaultQueryExecutor::settingsChanged,
                    Qt::QueuedConnection);
                m_currentConnection->setInterfaceType(IfaceType::USB);
                m_context.init(interface, executor, Strategy::Sync, Qt::DirectConnection);
            },
            [this](SerialSettings *settings)
            {
                SerialSettings *concSettings = qobject_cast<SerialSettings *>(settings);
                auto interface = new SerialPort(concSettings);
                auto executor = QueryExecutorFabric::makeModbusExecutor(m_currentConnection->getQueue(), concSettings);
                connect(this, &ConnectionManager::modbusSettingsChanged, executor,
                    &DefaultQueryExecutor::settingsChanged, Qt::QueuedConnection);
                m_currentConnection->setInterfaceType(IfaceType::RS485);
                m_context.init(interface, executor, Strategy::Sync, Qt::QueuedConnection);
            },
            [this](IEC104Settings *settings)
            {
                IEC104Settings *concSettings = qobject_cast<IEC104Settings *>(settings);
                auto interface = new Ethernet(concSettings);
                auto executor = QueryExecutorFabric::makeIec104Executor(m_currentConnection->getQueue(), concSettings);
                connect(this, &ConnectionManager::iec104SettingsChanged, executor,
                    &DefaultQueryExecutor::settingsChanged, Qt::QueuedConnection);
                m_currentConnection->setInterfaceType(IfaceType::Ethernet);
                m_context.init(interface, executor, Strategy::Sync, Qt::QueuedConnection);
            },
            [](EmulatorSettings *settings)
            {
                /// TODO: доделать
                Q_UNUSED(settings);
            } //
        },
        connectionData.settings);

    connect(m_context.m_iface, &BaseInterface::error,             //
        this, &ConnectionManager::handleInterfaceErrors, Qt::QueuedConnection);
    connect(m_context.m_executor, &DefaultQueryExecutor::timeout, //
        this, &ConnectionManager::handleQueryExecutorTimeout);
    connect(this, &ConnectionManager::reconnectInterface,         //
        m_context.m_iface, &BaseInterface::reconnect, Qt::QueuedConnection);
    connect(this, &ConnectionManager::reconnectInterface,         //
        m_context.m_executor, &DefaultQueryExecutor::reconnectEvent, Qt::QueuedConnection);
    connect(m_context.m_iface, &BaseInterface::reconnected,       //
        this, &ConnectionManager::interfaceReconnected, Qt::QueuedConnection);
    connect(m_currentConnection, &AsyncConnection::cancel, m_context.m_executor, &DefaultQueryExecutor::cancelQuery,
        Qt::QueuedConnection);

    m_currentConnection->reqBSI();
    if (!m_context.run(m_currentConnection))
    {
        m_currentConnection->deleteLater();
        m_currentConnection = nullptr;
        m_isInitial = true;
    }

    return m_currentConnection;
}

void ConnectionManager::setup(const BaseSettings *settings) noexcept
{
    m_silentTimer->setInterval(settings->get(MemKeys::silentInterval));
    m_errorMax = settings->get(MemKeys::maxErrors);
    // m_timeoutMax = settings.m_maxTimeouts;
}

void ConnectionManager::setReconnectMode(const ReconnectMode newMode) noexcept
{
    m_reconnectMode = newMode;
}

void ConnectionManager::reconnect()
{
    if (!m_isReconnectOccurred)
        qCritical() << "Произошла ошибка соединения";
    emit reconnectInterface();
    m_context.m_executor->wakeUp();
    if (m_reconnectMode == ReconnectMode::Loud)
        emit reconnectUI();
    else
        m_silentTimer->start();
    m_isReconnectOccurred = true;
}

void ConnectionManager::breakConnection()
{
    m_context.reset();
    m_currentConnection = nullptr;
    m_isInitial = true;
    m_isReconnectOccurred = false;
    m_reconnectMode = ReconnectMode::Loud;
    m_errorCounter = 0;
    // m_timeoutCounter = 0;
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
    case InterfaceError::OpenError:
        if (m_isInitial)
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
    // ++m_timeoutCounter;
    // if (m_timeoutCounter > m_timeoutMax)
    reconnect();
}

void ConnectionManager::fastCheckBSI(const DataTypes::BitStringStruct &data)
{
    // fast checking
    if (data.sigAdr == Device::bsiStartReg)
    {
        if (m_isInitial)
            m_isInitial = false;

        if (m_isReconnectOccurred)
        {
            /// TODO: проверять BSI
            // при реконнекте отключить устройство и подключить другое -> что будет? (modbus same)
            if (m_reconnectMode == ReconnectMode::Silent)
                m_silentTimer->stop();
            setReconnectMode(ReconnectMode::Loud);
            m_errorCounter = 0;
            // m_timeoutCounter = 0;
            m_currentConnection->getQueue().activate();
            m_isReconnectOccurred = false;
            qInfo() << "Соединение восстановлено";
            emit reconnectSuccess(); // Сообщаем, что переподключение прошло успешно
        }
        disconnect(m_connBSI);
    }
}

void ConnectionManager::interfaceReconnected()
{
    m_connBSI = m_currentConnection->connection(this, &ConnectionManager::fastCheckBSI);
    m_currentConnection->getQueue().activate();
    m_currentConnection->reqBSI();
    m_context.m_executor->start();
    m_currentConnection->getQueue().deactivate();
}

void ConnectionManager::settingsChanged(const QString &key, const QVariant &value)
{
    if (key.startsWith(SettingsKeys::USB::prefix))
        emit usbSettingsChanged(key, value);
    else if (key.startsWith(SettingsKeys::Iec104::prefix))
        emit iec104SettingsChanged(key, value);
    else if (key.startsWith(SettingsKeys::Serial::prefix))
        emit modbusSettingsChanged(key, value);
}

} // namespace Interface
