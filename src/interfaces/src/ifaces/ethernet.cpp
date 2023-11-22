#include "interfaces/ifaces/ethernet.h"

#include <QDebug>
#include <QNetworkProxy>

Ethernet::Ethernet(const IEC104Settings &settings, QObject *parent)
    : BaseInterface("EthernetPort", parent), m_settings(settings), m_socket(new QTcpSocket(this))
{
    m_socket->setProxy(QNetworkProxy::NoProxy);
    QObject::connect(m_socket, &QAbstractSocket::stateChanged, //
        this, &Ethernet::handleSocketState, Qt::DirectConnection);
}

bool Ethernet::connect()
{
    bool result = false;
    QEventLoop loop;
    QObject::connect(m_socket, &QAbstractSocket::connected, this, [&]() {
        setState(Interface::State::Run);
        loop.quit();
        result = true;
    });
    QObject::connect(m_socket, &QAbstractSocket::errorOccurred, this, [&]() {
        setState(Interface::State::Disconnect);
        loop.quit();
        result = false;
    });
    m_socket->connectToHost(m_settings.ip, m_settings.port, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
    loop.exec();
    return result;
}

void Ethernet::disconnect()
{
    if (m_socket->isOpen())
        m_socket->close();
}

// blocking read from serial port with timeout implementation
QByteArray Ethernet::read(bool *status)
{
    Q_UNUSED(status);
    QByteArray data;
    return data;
};

bool Ethernet::write(const QByteArray &data)
{
    if (!m_socket->isOpen())
    {
        qCritical("Ethernet write data to closed port");
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
    case QAbstractSocket::UnconnectedState:
        m_log.info("Socket unconnected");
        break;
    case QAbstractSocket::HostLookupState:
        m_log.info("Socket enters host lookup state");
        break;
    case QAbstractSocket::ConnectingState:
        m_log.info("Socket enters connecting state");
        break;
    case QAbstractSocket::ConnectedState:
        m_log.info("Socket connected!");
        break;
    case QAbstractSocket::BoundState:
        m_log.info("Socket is bound to address and port");
        break;
    case QAbstractSocket::ClosingState:
        m_log.info("Socket is in closing state");
        break;
    case QAbstractSocket::ListeningState:
        m_log.info("Socket is in listening state");
        break;
    default:
        m_log.warning("Unprocessed state");
        break;
    }
}
