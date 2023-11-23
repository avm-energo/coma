#include "interfaces/ifaces/ethernet.h"

#include <QDebug>
#include <QNetworkProxy>

Ethernet::Ethernet(const IEC104Settings &settings, QObject *parent)
    : BaseInterface("Ethernet", parent), m_settings(settings), m_socket(new QTcpSocket(this))
{
    m_socket->setProxy(QNetworkProxy::NoProxy);
    QObject::connect(m_socket, &QAbstractSocket::stateChanged, //
        this, &Ethernet::handleSocketState, Qt::DirectConnection);
    QObject::connect(m_socket, &QAbstractSocket::errorOccurred, //
        this, &Ethernet::handleSocketError, Qt::DirectConnection);
}

bool Ethernet::connect()
{
    m_socket->connectToHost(m_settings.ip, m_settings.port, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
    /// TODO: сделать настраиваемым значение 5 секунд на подключение
    return m_socket->waitForConnected(5000);
}

void Ethernet::disconnect()
{
    if (m_socket->isOpen())
    {
        m_socket->disconnectFromHost();
        /// TODO: сделать настраиваемым значение 5 секунд на отключение
        if (m_socket->state() == QAbstractSocket::UnconnectedState || m_socket->waitForDisconnected(5000))
            m_log.info("Socket disconnected");
        else
            m_log.warning("Disconnect from host timeout!");
        m_socket->close();
    }
}

// Blocking data reading from the socket
QByteArray Ethernet::read(bool *status)
{
    QByteArray data;
    if (!m_socket->isOpen() || !m_socket->isReadable())
    {
        m_log.error("Ethernet reading data from the closed socket");
        return data;
    }
    if (m_socket->waitForReadyRead(-1))
    {
        m_dataGuard.lock();         // lock port
        data = m_socket->readAll(); // read data
        m_dataGuard.unlock();       // unlock port
        *status = true;
    }
    return data;
};

bool Ethernet::write(const QByteArray &data)
{
    if (!m_socket->isOpen() || !m_socket->isWritable())
    {
        m_log.error("Ethernet write data to the closed socket");
        return false;
    }
    m_dataGuard.lock();                 // lock ethernet iface
    auto bytes = m_socket->write(data); // write data
    m_dataGuard.unlock();               // unlock ethernet iface
    if (bytes <= 0)
    {
        qCritical() << "Error with data writing";
        emit error(InterfaceError::WriteError);
        return false;
    }
    return true;
}

void Ethernet::handleSocketState(const QAbstractSocket::SocketState state)
{
    switch (state)
    {
    case QAbstractSocket::SocketState::UnconnectedState:
        m_log.info("Socket unconnected");
        break;
    case QAbstractSocket::SocketState::HostLookupState:
        m_log.info("Socket enters host lookup state");
        break;
    case QAbstractSocket::SocketState::ConnectingState:
        m_log.info("Socket enters connecting state");
        break;
    case QAbstractSocket::SocketState::ConnectedState:
        m_log.info("Socket connected!");
        break;
    case QAbstractSocket::SocketState::BoundState:
        m_log.info("Socket is bound to address and port");
        break;
    case QAbstractSocket::SocketState::ClosingState:
        m_log.info("Socket is in closing state");
        break;
    case QAbstractSocket::SocketState::ListeningState:
        m_log.info("Socket is in listening state");
        break;
    default:
        m_log.warning("Unprocessed state");
        break;
    }
}

void Ethernet::handleSocketError(const QAbstractSocket::SocketError err)
{
    switch (err)
    {
    default:
        m_log.error(m_socket->errorString());
        emit error(InterfaceError::OpenError);
        break;
    }
}
