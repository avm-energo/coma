#include <QThread>
#include <QCoreApplication>

#include "ethernet.h"
#include "../publicclass.h"

ethernet::ethernet(QObject *parent) :
    QObject(parent)
{
    OutDataBuf.clear();
    ClosePortAndFinishThread = false;
}

void ethernet::run()
{
    sock = new QTcpSocket(this);
    connect(sock,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(seterr(QAbstractSocket::SocketError)));
    connect(sock,SIGNAL(connected()),this,SIGNAL(connected()));
    sock->connectToHost(pc.MIPIP,PORT104,QIODevice::ReadWrite,QAbstractSocket::IPv4Protocol);
    connect(sock,SIGNAL(readyRead()),this,SLOT(CheckForData()));
    while (1)
    {
        OutDataBufMtx.lock();
        if (!OutDataBuf.isEmpty()) // что-то пришло в выходной буфер для записи
            SendData();
        OutDataBufMtx.unlock();
        if (ClosePortAndFinishThread)
        {
            if (sock->isOpen())
            {
                sock->close();
                sock->disconnect();
                delete sock;
            }
            emit finished();
            break;
        }
        QThread::msleep(10);
        qApp->processEvents();
    }
}

void ethernet::stop()
{
    ClosePortAndFinishThread = true;
}

void ethernet::seterr(QAbstractSocket::SocketError err)
{
    emit error(err+25); // до 24 другие ошибки, err от -1
}

void ethernet::SendData()
{
    qint64 res = sock->write(OutDataBuf);
    if (res == -1)
        emit error(SKT_SENDDATAER); // ошибка
    OutDataBuf.clear();
}

void ethernet::InitiateWriteDataToPort(QByteArray ba)
{
    OutDataBufMtx.lock();
    OutDataBuf = ba;
    OutDataBufMtx.unlock();
}

void ethernet::CheckForData()
{
    QByteArray ba = sock->readAll();
    emit newdataarrived(ba);
}
