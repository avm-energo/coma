#include "ethernet.h"

#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"

#include <QCoreApplication>
#include <QNetworkProxy>
#include <QSettings>
#include <QThread>

Ethernet::Ethernet(QObject *parent) : QObject(parent)
{
    Log = new LogClass(this);
    Log->Init("ethernet.log");
    Log->info("=== Log started ===");
    OutDataBuf.clear();
    ClosePortAndFinishThread = false;
}

Ethernet::~Ethernet()
{
}

void Ethernet::Run()
{
    EthConnected = false;
    StdFunc::SetDeviceIP(IP);
    sock = new QTcpSocket(this);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(sock, qOverload<QAbstractSocket::SocketError>(&QAbstractSocket::error), this, &Ethernet::seterr);
#else
    connect(sock, &QAbstractSocket::errorOccurred, this, &Ethernet::seterr);
#endif
    connect(sock, &QAbstractSocket::stateChanged, this, &Ethernet::EthStateChanged);
    connect(sock, &QAbstractSocket::connected, this, &Ethernet::Connected);
    connect(sock, &QAbstractSocket::connected, this, &Ethernet::EthSetConnected);
    connect(sock, &QAbstractSocket::disconnected, this, &Ethernet::Disconnected);
    Log->info("Connecting to host: " + StdFunc::ForDeviceIP() + ", port: " + QString::number(PORT104));
    sock->setProxy(QNetworkProxy::NoProxy);
    connect(sock, &QIODevice::readyRead, this, &Ethernet::CheckForData);
    sock->connectToHost(StdFunc::ForDeviceIP(), PORT104, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
    QEventLoop loop;
    connect(sock, &QAbstractSocket::connected, &loop, &QEventLoop::quit);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(sock, qOverload<QAbstractSocket::SocketError>(&QAbstractSocket::error), &loop, &QEventLoop::quit);
#else
    connect(sock, &QAbstractSocket::errorOccurred, &loop, &QEventLoop::quit);
#endif
    loop.exec();
    //    TimeFunc::WaitFor(EthConnected, TIMEOUT_BIG);
    while (!ClosePortAndFinishThread)
    {
        OutDataBufMtx.lock();
        if (!OutDataBuf.isEmpty()) // что-то пришло в выходной буфер для записи
            SendData();
        OutDataBufMtx.unlock();
        TimeFunc::Wait(10);
    }
    if (sock->isOpen())
    {
        sock->close();
        sock->disconnect();
        sock->deleteLater();
    }
    // emit Finished();
    emit finished();
}

void Ethernet::Stop()
{
    Log->info("Stop signal");
    ClosePortAndFinishThread = true;
}

void Ethernet::seterr(QAbstractSocket::SocketError error)
{
    Log->info("Error: " + QVariant::fromValue(error).toString());
    //   emit error(err); // до 24 другие ошибки, err от -1
    ClosePortAndFinishThread = true;
}

void Ethernet::SendData()
{
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

void Ethernet::InitiateWriteDataToPort(QByteArray ba)
{
    OutDataBufMtx.lock();
    OutDataBuf = ba;
    OutDataBufMtx.unlock();
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
    QByteArray ba = sock->readAll();
    Log->info("PC <- " + ba.toHex());
    emit NewDataArrived(ba);
}

void Ethernet::EthSetConnected()
{
    EthConnected = true;
}
