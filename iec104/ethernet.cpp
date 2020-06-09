#include <QSettings>
#include <QThread>
#include <QCoreApplication>

#include "ethernet.h"
#include "../gen/stdfunc.h"
#include "../gen/error.h"
#include "../widgets/emessagebox.h"
#include "../gen/timefunc.h"

ethernet::ethernet(QObject *parent) :
    QObject(parent)
{
    OutDataBuf.clear();
    ClosePortAndFinishThread = false;
}

ethernet::~ethernet()
{
}

void ethernet::Run()
{
    //quint8 sendStatus = 0;
/*    stateMessage = 0;
    disconflag = 0;
    timerstart = 0; */
    EthConnected = false;
//    QTimer timeout;
//    QString tmps = IP;
//    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
//    StdFunc::SetDeviceIP(sets->value("DeviceIP", IP).toString());
    StdFunc::SetDeviceIP(IP);
    sock = new QTcpSocket(this);
    connect(sock,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(seterr(QAbstractSocket::SocketError)));
    connect(sock,SIGNAL(connected()),this,SIGNAL(connected()));
    connect(sock,SIGNAL(connected()),this,SLOT(EthSetConnected()));
    connect(sock,SIGNAL(disconnected()),this,SIGNAL(disconnected()));
    sock->connectToHost(StdFunc::ForDeviceIP(),PORT104,QIODevice::ReadWrite,QAbstractSocket::IPv4Protocol);
    connect(sock,SIGNAL(readyRead()),this,SLOT(CheckForData()));
/*    connect(&timeout,SIGNAL(timeout()),this,SLOT(SetFlag()));
    timeout.setInterval(1000); */
    TimeFunc::WaitFor(EthConnected, TIMEOUT_BIG);
    while(!ClosePortAndFinishThread)
    {

        OutDataBufMtx.lock();
        if (!OutDataBuf.isEmpty()) // что-то пришло в выходной буфер для записи
            SendData();
        OutDataBufMtx.unlock();

/*        if(!sock->isValid())
        {
            if(stateMessage != 1)
            {
              if(!timerstart)
              {
                  timeout.start();
                  timerstart = 1;
              }

              if(disconflag == 1)
              {
                  timeout.stop();
                  emit ethNoconnection();
                  stateMessage = 1;
                  disconflag = 0;
                  timerstart = 0;
              }
            }

        } */
        //sock->write(nullptr,1);

        /*if(!sendStatus)
        {
            if (!sock->state())
            {
              sendStatus = 1;
              emit disconnected();
            }
        }*/

        QThread::msleep(10);
        qApp->processEvents();
    }
    if (sock->isOpen())
    {
        sock->close();
        sock->disconnect();
        sock->deleteLater();
    }
    emit Finished();
}

void ethernet::Stop()
{
    ClosePortAndFinishThread = true;
}

void ethernet::seterr(QAbstractSocket::SocketError err)
{
    ERMSG("Ethernet error: " + sock->errorString());
    emit error(err+25); // до 24 другие ошибки, err от -1
    ClosePortAndFinishThread = true;
}

void ethernet::SendData()
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

void ethernet::InitiateWriteDataToPort(QByteArray ba)
{
//    TimeFunc::Wait(1);
//    mutexflag = 1;
    OutDataBufMtx.lock();
    OutDataBuf = ba;
    OutDataBufMtx.unlock();
//    mutexflag = 0;
}

void ethernet::CheckForData()
{
    QByteArray ba = sock->readAll();
    emit newdataarrived(ba);
}

/*void ethernet::SetFlag()
{
//    disconflag = 1;
} */

void ethernet::EthSetConnected()
{
    EthConnected = true;
}
