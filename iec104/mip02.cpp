#include <QTime>
#include <QCoreApplication>

#include "mip02.h"
#include "../publicclass.h"

mip02::mip02(QObject *parent) :
    QObject(parent)
{

}

void mip02::run()
{
    sock = new QTcpSocket(this);
    connect(sock,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(seterr(QAbstractSocket::SocketError)));
    connect(sock,SIGNAL(connected()),this,SIGNAL(connected()));
    sock->connectToHost(pc.MIPIP,PORT104,QIODevice::ReadWrite,QAbstractSocket::IPv4Protocol);
    connect(sock,SIGNAL(readyRead()),this,SLOT(Parse104()));
    while (1)
    {
        OutDataBufMtx.lock();
        if (!OutDataBuf.isEmpty()) // что-то пришло в выходной буфер для записи
            SendData();
        OutDataBufMtx.unlock();
        if (ClosePortAndFinishThread)
        {
            stop();
            return;
        }
        QTime tmr;
        tmr.start();
        while (tmr.elapsed() < 100)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void mip02::Parse104()
{

}

void mip02::stop()
{
    if (sock->isOpen())
    {
        sock->close();
        sock->disconnect();
        delete sock;
    }
    emit finished();
}

void mip02::seterr(QAbstractSocket::SocketError err)
{
    emit error(err+25); // до 24 другие ошибки, err от -1
}

void mip02::SendData()
{
    qint64 res = sock->write(OutDataBuf);
    if (res == -1)
        emit error(SKT_SENDDATAER); // ошибка
    OutDataBuf.clear();
}

void mip02::InitiateWriteDataToPort(QByteArray *ba)
{
    OutDataBufMtx.lock();
    OutDataBuf = *ba;
    OutDataBufMtx.unlock();
}
