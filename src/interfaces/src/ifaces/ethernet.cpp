#include "interfaces/ifaces/ethernet.h"

#include <QDebug>
#include <QNetworkProxy>

Ethernet::Ethernet(IEC104Settings *settings, QObject *parent)
    : BaseInterface("Ethernet", settings, parent)
    , m_settings(settings)
    , m_socket(new QTcpSocket(this))
{
    m_socket->setProxy(QNetworkProxy::NoProxy);
    QObject::connect(m_socket, &QAbstractSocket::stateChanged,  //
        this, &Ethernet::handleSocketState, Qt::DirectConnection);
    QObject::connect(m_socket, &QAbstractSocket::errorOccurred, //
        this, &Ethernet::handleSocketError, Qt::DirectConnection);
    QObject::connect(m_socket, &QTcpSocket::readyRead, this, &BaseInterface::readyRead);
}

bool Ethernet::connect()
{
    m_socket->connectToHost(
        m_settings->get("ip"), m_settings->get("port"), QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
    if (m_socket->waitForConnected(m_settings->get<u16>("t0") * 1000))
    {
        if (getState() != Interface::State::Disconnect)
        {
            setState(Interface::State::Run);
            qDebug("Связь с устройством установлена");
            emit started();
            return true;
        }
    }
    return false;
}

void Ethernet::disconnect()
{
    if (m_socket->isOpen())
    {
        m_socket->disconnectFromHost();
        /// TODO: сделать настраиваемым значение 5 секунд на отключение
        if (m_socket->state() == QAbstractSocket::UnconnectedState || m_socket->waitForDisconnected(5000))
            m_log.writeLog(Logger::Info, "Socket disconnected");
        else
            m_log.writeLog(Logger::Warning, "Disconnect from host timeout!");
        m_socket->close();
    }
}

// Blocking data reading from the socket
QByteArray Ethernet::read(bool &status)
{
    QByteArray data;

    m_dataGuard.lock(); // Нужен ли мьютекс?
    if (m_socket->bytesAvailable())
        status = true;

    while (m_socket->bytesAvailable())
    {
        data += m_socket->readAll();
        m_socket->waitForReadyRead(20); // Нужен ли тут таймаут?
    }
    m_dataGuard.unlock();

    return data;
}

bool Ethernet::write(const QByteArray &data)
{
    if (!m_socket->isOpen() || !m_socket->isWritable())
    {
        m_log.writeLog(Logger::Critical, "Ethernet write data to the closed socket");
        return false;
    }
    m_dataGuard.lock();                 // lock ethernet iface
    auto bytes = m_socket->write(data); // write data
    m_dataGuard.unlock();               // unlock ethernet iface
    if (bytes <= 0)
    {
        qDebug() << "Ошибка записи данных в сокет";
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
        m_log.writeLog(Logger::Info, "Socket unconnected");
        break;
    case QAbstractSocket::SocketState::HostLookupState:
        m_log.writeLog(Logger::Info, "Socket enters host lookup state");
        break;
    case QAbstractSocket::SocketState::ConnectingState:
        m_log.writeLog(Logger::Info, "Socket enters connecting state");
        break;
    case QAbstractSocket::SocketState::ConnectedState:
        m_log.writeLog(Logger::Info, "Socket connected!");
        break;
    case QAbstractSocket::SocketState::BoundState:
        m_log.writeLog(Logger::Info, "Socket is bound to address and port");
        break;
    case QAbstractSocket::SocketState::ClosingState:
        m_log.writeLog(Logger::Info, "Socket is in closing state");
        break;
    case QAbstractSocket::SocketState::ListeningState:
        m_log.writeLog(Logger::Info, "Socket is in listening state");
        break;
    default:
        m_log.writeLog(Logger::Warning, "Unprocessed state");
        break;
    }
}

void Ethernet::handleSocketError(const QAbstractSocket::SocketError err)
{
    switch (err)
    {
    case QAbstractSocket::SocketError::SocketTimeoutError:
        // ignore
        break;
    default:
        m_log.writeLog(Logger::Critical, m_socket->errorString());
        emit error(InterfaceError::OpenError);
        break;
    }
}
