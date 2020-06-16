#include <QSettings>
#include <QThread>
#include <QCoreApplication>

#include "ethernet.h"
#include "../gen/stdfunc.h"
#include "../gen/error.h"
#include "../widgets/emessagebox.h"
#include "../gen/timefunc.h"

Ethernet::Ethernet(QObject *parent) :
    QObject(parent)
{
    Log = new LogClass;
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
    connect(sock,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(seterr(QAbstractSocket::SocketError)));
    connect(sock,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(EthStateChanged(QAbstractSocket::SocketState)));
    connect(sock,SIGNAL(connected()),this,SIGNAL(Connected()));
    connect(sock,SIGNAL(connected()),this,SLOT(EthSetConnected()));
    connect(sock,SIGNAL(disconnected()),this,SIGNAL(Disconnected()));
    Log->info("Connecting to host: " + StdFunc::ForDeviceIP() + ", port: " + QString::number(PORT104));
    sock->connectToHost(StdFunc::ForDeviceIP(),PORT104,QIODevice::ReadWrite,QAbstractSocket::IPv4Protocol);
    connect(sock,SIGNAL(readyRead()),this,SLOT(CheckForData()));
    TimeFunc::WaitFor(EthConnected, TIMEOUT_BIG);
    while(!ClosePortAndFinishThread)
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
    emit Finished();
}

void Ethernet::Stop()
{
    Log->info("Stop signal");
    ClosePortAndFinishThread = true;
}

void Ethernet::seterr(QAbstractSocket::SocketError err)
{
    Log->info("Error: " + sock->errorString());
    emit error(err+25); // до 24 другие ошибки, err от -1
    ClosePortAndFinishThread = true;
}

void Ethernet::SendData()
{
    if (!sock->isOpen())
    {
        ERMSG("Ethernet write data to closed port");
        return;
    }
    Log->info("PC -> " + OutDataBuf.toHex());
    qint64 res = sock->write(OutDataBuf);
    Log->info(QString::number(res) + " bytes written");
    if (res == -1)
    {
        ERMSG("Ethernet write error");
        emit error(SKT_SENDDATAER); // ошибка
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
