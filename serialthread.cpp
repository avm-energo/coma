#include "serialthread.h"
#include "publicclass.h"
#include <QTimer>

SerialThread::SerialThread(QSerialPort *port, QObject *parent) :
    QObject(parent)
{
    this->port = new QSerialPort;
    this->port = port;
}

void SerialThread::run()
{
//    QTimer *timer = new QTimer;
//    timer->setInterval(100);
//    timer->start();
//    connect(timer,SIGNAL(timeout()),this,SLOT(CheckForData()));
    connect(port,SIGNAL(readyRead()),this,SLOT(CheckForData()));
}

void SerialThread::CheckForData()
{
    QByteArray ba = port->read(1000000);
    ReadData.append(ba);
    emit newdataarrived();
}

void SerialThread::WriteData(QByteArray outbuf)
{
    qint64 res = port->write(outbuf);
    if (res == -1)
        emit datawritten(1); // ошибка
    emit datawritten(0); // всё гут
}

QByteArray SerialThread::data()
{
    QByteArray ba = ReadData;
    ReadData.clear();
    return ba;
}
