#include "ethernet.h"

#include <QCoreApplication>
#include <QNetworkProxy>
#include <QSettings>
#include <QThread>
#include <gen/error.h>
#include <gen/pch.h>
#include <gen/stdfunc.h>
#include <gen/timefunc.h>

Ethernet::Ethernet(QObject *parent) : BasePort("iec104port", parent)
{
    ClosePortAndFinishThread = false;
}

Ethernet::~Ethernet()
{
    disconnect();
    emit finished();
}

bool Ethernet::init(IEC104Settings settings)
{
    m_baseadr = settings.baseadr;
    m_ip = settings.ip;
    sock = new QTcpSocket(this);
    QObject::connect(sock, &QAbstractSocket::errorOccurred, this, [=](QAbstractSocket::SocketError error) {
        Log->info("Error: " + QVariant::fromValue(error).toString());
        sock->disconnectFromHost();
    });
    QObject::connect(sock, &QAbstractSocket::stateChanged, this, &Ethernet::EthStateChanged, Qt::DirectConnection);
    QObject::connect(sock, &QAbstractSocket::connected, this, &Ethernet::Connected, Qt::DirectConnection);
    QObject::connect(sock, &QAbstractSocket::connected, this, &Ethernet::EthSetConnected, Qt::DirectConnection);
    QObject::connect(sock, &QAbstractSocket::disconnected, this, &Ethernet::Disconnected, Qt::DirectConnection);
    sock->setProxy(QNetworkProxy::NoProxy);
    QObject::connect(
        sock, &QIODevice::readyRead, this,
        [=] {
            qDebug() << __PRETTY_FUNCTION__;
            QByteArray ba = sock->readAll();
            Log->info("PC <- " + ba.toHex());
            emit dataReceived(ba);
        },
        Qt::QueuedConnection);
    return reconnect();
}

void Ethernet::poll()
{
    while (!ClosePortAndFinishThread)
    // while (Board::GetInstance().connectionState() != Board::ConnectionState::Closed)
    {
        qDebug() << __PRETTY_FUNCTION__;
        QMutexLocker locker(&OutDataBufMtx);
        if (!OutDataBuf.isEmpty())
            SendData();
        else
            _waiter.wait(&OutDataBufMtx /*, 1000*/);
        // QCoreApplication::processEvents();
    }
    disconnect();
}

void Ethernet::SendData()
{
    qDebug() << __PRETTY_FUNCTION__;
    if (!sock->isOpen())
    {
        qCritical("Ethernet write data to closed port");
        return;
    }
    Log->info("PC -> " + OutDataBuf.toHex());
    qint64 res = sock->write(OutDataBuf);
    Log->info(QString::number(res) + " bytes written");
    if (res == -1)
    {
        qCritical("Ethernet write error");
        // emit error(QAbstractSocket::SocketError::NetworkError); // ошибка
    }
    OutDataBuf.clear();
}

bool Ethernet::writeData(const QByteArray &ba)
{
    // qDebug() << __PRETTY_FUNCTION__;
    OutDataBufMtx.lock();
    OutDataBuf = ba;
    OutDataBufMtx.unlock();
    _waiter.wakeOne();
    return true;
}

void Ethernet::disconnect()
{
    if (sock->isOpen())
    {
        qDebug() << "Socket close";
        sock->close();
        sock->disconnect();
        sock->deleteLater();
    }
    Log->info("Stop signal");
    ClosePortAndFinishThread = true;
    setState(Interface::State::Disconnect);
}

bool Ethernet::reconnect()
{
    bool result = false;
    disconnect();

    EthConnected = false;
    StdFunc::SetDeviceIP(IP);

    Log->info("Connecting to host: " + m_ip + ", port: " + QString::number(m_baseadr));

    sock->connectToHost(StdFunc::ForDeviceIP(), PORT104, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
    QEventLoop loop;
    QObject::connect(sock, &QAbstractSocket::connected, this, [&]() {
        setState(Interface::State::Run);
        loop.quit();
        result = true;
    });
    QObject::connect(sock, &QAbstractSocket::errorOccurred, this, [&]() {
        setState(Interface::State::Disconnect);
        loop.quit();
        result = false;
    });
    loop.exec();
    return result;
}

void Ethernet::EthStateChanged(QAbstractSocket::SocketState state)
{
    switch (state)
    {
    case QAbstractSocket::UnconnectedState:
        Log->info("Socket unconnected");
        break;
    case QAbstractSocket::HostLookupState:
        Log->info("Socket enters host lookup state");
        break;
    case QAbstractSocket::ConnectingState:
        Log->info("Socket enters connecting state");
        break;
    case QAbstractSocket::ConnectedState:
        Log->info("Socket connected!");
        break;
    case QAbstractSocket::BoundState:
        Log->info("Socket is bound to address and port");
        break;
    case QAbstractSocket::ClosingState:
        Log->info("Socket is in closing state");
        break;
    case QAbstractSocket::ListeningState:
        Log->info("Socket is in listening state");
        break;
    default:
        Log->info("Unprocessed state");
        break;
    }
}

void Ethernet::CheckForData()
{
    qDebug() << __PRETTY_FUNCTION__;
    QByteArray ba = sock->readAll();
    Log->info("PC <- " + ba.toHex());
    emit dataReceived(ba);
}

void Ethernet::EthSetConnected()
{
    EthConnected = true;
}
