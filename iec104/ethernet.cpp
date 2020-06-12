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
    connect(sock,SIGNAL(Connected()),this,SIGNAL(Connected()));
    connect(sock,SIGNAL(Connected()),this,SLOT(EthSetConnected()));
    connect(sock,SIGNAL(Disconnected()),this,SIGNAL(Disconnected()));
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
    ClosePortAndFinishThread = true;
}

void Ethernet::seterr(QAbstractSocket::SocketError err)
{
    ERMSG("Ethernet error: " + sock->errorString());
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
    qint64 res = sock->write(OutDataBuf);
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

void Ethernet::CheckForData()
{
    QByteArray ba = sock->readAll();
    emit NewDataArrived(ba);
}

void Ethernet::EthSetConnected()
{
    EthConnected = true;
}
