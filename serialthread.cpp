#include "serialthread.h"
#include "publicclass.h"
#include <QTimer>

SerialThread::SerialThread(QSerialPort *port, QObject *parent) :
    QObject(parent)
{
    port = new QSerialPort;
    this->port = port;
}

void SerialThread::run()
{
    QTimer *timer = new QTimer;
    timer->setInterval(100);
    connect(timer,SIGNAL(timeout()),this,SLOT(CheckForData()));
}

void SerialThread::CheckForData()
{
    QByteArray ba = port->readAll();
    if (ba.size() > 0)
        emit newdataarrived(ba);
}

void SerialThread::WriteData(QByteArray outbuf)
{
    qint64 res = port->write(outbuf);
    if (res == -1)
        emit datawritten(1); // ошибка
    if (port->waitForBytesWritten(1000))
        emit datawritten(0); // всё гут
    emit datawritten(2); // произошёл тайм-аут или ошибка
}
