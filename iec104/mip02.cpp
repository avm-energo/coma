#include "mip02.h"

mip02::mip02(QObject *parent) :
    QObject(parent)
{

}

void mip02::run()
{
    sock = new QTcpSocket(this);
    sock->connectToHost(Host,PORT104,QIODevice::ReadWrite,QAbstractSocket::IPv4Protocol);
    connect(sock,SIGNAL(readyRead()),this,SLOT(Parse104()));
}

void mip02::Parse104()
{

}
