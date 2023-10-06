#include "interfaces/connectionmanager.h"

#include <gen/std_ext.h>
#include <interfaces/ifaces/serialport.h>
#include <interfaces/ifaces/usbhidport.h>
#include <interfaces/parsers/modbusparser.h>
#include <interfaces/parsers/protocomparser.h>

namespace Interface
{

ConnectionManager::ConnectionManager(QWidget *parent)
    : QObject(parent)
    , m_currentConnection(nullptr)
    , m_silentTimer(new QTimer(this))
    , m_reconnectMode(ReconnectMode::Loud)
{
    // TODO: брать значение из настроек
    m_silentTimer->setInterval(10000);
    m_silentTimer->setSingleShot(true);
    connect(m_silentTimer, &QTimer::timeout, this, [this] { emit reconnectUI(); });
}

void ConnectionManager::createConnection(const ConnectStruct &connectionData)
{
    if (m_currentConnection != nullptr)
        breakConnection();

    m_currentConnection = new Connection(this);
    std::visit( // Инициализация контекста для обмена данными
        overloaded {
            [this](const UsbHidSettings &settings) {
                auto port = new UsbHidPort(settings);
                auto parser = new ProtocomParser(m_currentConnection->m_queue);
                m_context.init(port, parser, Strategy::Sync, Qt::DirectConnection);
            },
            [this](const SerialPortSettings &settings) {
                auto port = new SerialPort();
                port->init(settings);
                auto parser = new ModbusParser(m_currentConnection->m_queue);
                parser->setDeviceAddress(settings.Address);
                m_context.init(port, parser, Strategy::Sync, Qt::QueuedConnection);
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
    connect(m_context.m_iface, &BaseInterface::error, this, &ConnectionManager::handlePortError, Qt::DirectConnection);
    connect(
        this, &ConnectionManager::reconnectDevice, m_context.m_iface, &BaseInterface::reconnect, Qt::QueuedConnection);
    connect(this, &ConnectionManager::reconnectSuccess, m_context.m_iface, //
        &BaseInterface::finishReconnect, Qt::DirectConnection);
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
    emit reconnectDevice();
    if (m_reconnectMode == ReconnectMode::Loud)
        emit reconnectUI();
    else
        m_silentTimer->start();
}

void ConnectionManager::breakConnection()
{
    m_context.reset();
    m_currentConnection = nullptr;
    Connection::s_connection.reset();
}

void ConnectionManager::handlePortError(const InterfaceError error)
{
    if (error == InterfaceError::Timeout
        && m_context.m_parser->m_currentCommand.command == Interface::Commands::C_ReqBSI)
    {
        qCritical() << "Превышено время ожидания блока BSI. Disconnect...";
        breakConnection();
    }
}

bool ConnectionManager::isCurrentDevice(const QString &guid)
{
    if (m_context.isValid())
    {
        auto usbPort = dynamic_cast<UsbHidPort *>(m_context.m_iface);
        if (usbPort != nullptr)
        {
            const auto &devInfo = usbPort->deviceInfo();
            return devInfo.hasMatch(guid);
        }
    }
    return false;
}

void ConnectionManager::deviceConnected(const QString &guid)
{
    if (isCurrentDevice(guid))
    {
        if (m_reconnectMode == ReconnectMode::Silent)
            m_silentTimer->stop();
        // Выводим порт из состояния реконнекта
        emit reconnectSuccess();
    }
}

void ConnectionManager::deviceDisconnected(const QString &guid)
{
    if (isCurrentDevice(guid))
    {
        // Загоняем порт в состояние реконнекта
        reconnect();
    }
}

} // namespace Interface
