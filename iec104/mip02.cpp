#include "mip02.h"

mip02::mip02(QObject *parent) :
    QObject(parent)
{

}

void mip02::run()
{
    sock = new QTcpSocket(this);
//    sock->connectToHost();
}

void mip02::Parse104()
{

}
