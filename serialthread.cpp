#include "serialthread.h"
#include "publicclass.h"
#include <QTimer>

SerialThread::SerialThread(QSerialPort *port, QObject *parent) :
    QObject(parent)
{
    this->port = new QSerialPort;
    this->port = port;
    TimeoutTimer = new QTimer;
    TimeoutTimer->setInterval(400);
    connect(TimeoutTimer, SIGNAL(timeout()),this,SLOT(Timeout()));
}

void SerialThread::run()
{
    connect(port,SIGNAL(readyRead()),this,SLOT(CheckForData()));
}

void SerialThread::CheckForData()
{
    QByteArray ba = port->read(1000000);
    NothingReceived = false;
    ReadData.append(ba);
    emit newdataarrived(ba);
    if (TimeoutTimer->isActive())
        TimeoutTimer->start();
}

void SerialThread::WriteData(QByteArray outbuf)
{
    qint64 res = port->write(outbuf);
    if (res == -1)
        emit datawritten(QByteArray()); // ошибка
    TimeoutTimer->start();
    emit datawritten(outbuf); // всё гут
}

QByteArray SerialThread::data()
{
    QByteArray ba = ReadData;
    ReadData.clear();
    return ba;
}

void SerialThread::InitiateWriteDataToPort(QByteArray ba)
{
    ReadData.clear();
    WriteData(ba);
    NothingReceived = true;
}

void SerialThread::Timeout()
{
    if (NothingReceived)
        emit timeout();
    TimeoutTimer->stop();
    emit receivecompleted();
}
