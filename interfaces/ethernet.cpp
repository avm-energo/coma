#include "ethernet.h"

#include "../gen/error.h"
#include "../gen/pch.h"
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
    // OutDataBuf.clear();
    ClosePortAndFinishThread = false;
    sock = new QTcpSocket(this);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(sock, qOverload<QAbstractSocket::SocketError>(&QAbstractSocket::error), this,
        [=](QAbstractSocket::SocketError error) {
            Log->info("Error: " + QVariant::fromValue(error).toString());
            sock->disconnectFromHost();
        });
#else
    connect(sock, &QAbstractSocket::errorOccurred, this, [=](QAbstractSocket::SocketError error) {
        Log->info("Error: " + QVariant::fromValue(error).toString());
        sock->disconnectFromHost();
    });
#endif
    connect(sock, &QAbstractSocket::stateChanged, this, &Ethernet::EthStateChanged, Qt::DirectConnection);
    connect(sock, &QAbstractSocket::connected, this, &Ethernet::Connected, Qt::DirectConnection);
    connect(sock, &QAbstractSocket::connected, this, &Ethernet::EthSetConnected, Qt::DirectConnection);
    connect(sock, &QAbstractSocket::disconnected, this, &Ethernet::Disconnected, Qt::DirectConnection);
    sock->setProxy(QNetworkProxy::NoProxy);
    connect(
        sock, &QIODevice::readyRead, this,
        [=] {
            qDebug() << __PRETTY_FUNCTION__;
            QByteArray ba = sock->readAll();
            Log->info("PC <- " + ba.toHex());
            emit NewDataArrived(ba);
        },
        Qt::QueuedConnection);
}

Ethernet::~Ethernet()
{
}

void Ethernet::Run()
{
    EthConnected = false;
    StdFunc::SetDeviceIP(IP);

    Log->info("Connecting to host: " + StdFunc::ForDeviceIP() + ", port: " + QString::number(PORT104));

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
    //    while (!ClosePortAndFinishThread)
    //    {
    //        OutDataBufMtx.lock();
    //        if (!OutDataBuf.isEmpty()) // что-то пришло в выходной буфер для записи
    //            SendData();
    //        OutDataBufMtx.unlock();
    //  TimeFunc::Wait(10);
    //    }
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
    if (sock->isOpen())
    {
        sock->close();
        sock->disconnect();
        sock->deleteLater();
    }
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

void Ethernet::InitiateWriteDataToPort(QByteArray ba)
{
    // qDebug() << __PRETTY_FUNCTION__;
    OutDataBufMtx.lock();
    OutDataBuf = ba;
    OutDataBufMtx.unlock();
    _waiter.wakeOne();
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
    emit NewDataArrived(ba);
}

void Ethernet::EthSetConnected()
{
    EthConnected = true;
}
