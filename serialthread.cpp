#include <QtTest/QTest>
#include "serialthread.h"
#include "publicclass.h"
#include <QTimer>
#include "config.h"

SerialThread::SerialThread(QObject *parent) :
    QObject(parent)
{
    DataToSend = new QByteArray(1000, 0x00);
    OutDataBuf.clear();
    ClosePortAndFinishThread = false;

}

void SerialThread::run()
{
    TimeoutTimer = new QTimer;
    TimeoutTimer->setInterval(1000);
    connect(TimeoutTimer, SIGNAL(timeout()),this,SLOT(Timeout()));
    port = new QSerialPort;
    port->setPort(portinfo);
    if (!port->open(QIODevice::ReadWrite))
    {
        emit finished();
        emit error(1);
    }
    port->setBaudRate(baud);
    port->setParity(QSerialPort::NoParity);
    port->setDataBits(QSerialPort::Data8);
    port->setFlowControl(QSerialPort::NoFlowControl);
    port->setStopBits(QSerialPort::OneStop);
    connect(port,SIGNAL(readyRead()),this,SLOT(CheckForData()));
//    connect(this,SIGNAL(finished()),port,SLOT(deleteLater()));
    while (1)
    {
        if (OutDataBuf.size()>0) // что-то пришло в выходной буфер для записи
            WriteData();
        if (ClosePortAndFinishThread)
        {
            if (port->isOpen())
            {
                port->close();
                delete port;
                TimeoutTimer->stop();
                delete TimeoutTimer;
            }
            emit finished();
            return;
        }
        QTest::qWait(100);
    }
}

void SerialThread::CheckForData()
{
    QByteArray ba = port->read(1000);
    emit newdataarrived(ba);
}

void SerialThread::WriteData()
{
    OutDataBufMtx.lock();
    qint64 res = port->write(OutDataBuf);
    if (res == -1)
        emit datawritten(QByteArray()); // ошибка
    TimeoutTimer->start();
    emit datawritten(OutDataBuf); // всё гут
    OutDataBuf.clear();
    OutDataBufMtx.unlock();
}

void SerialThread::InitiateWriteDataToPort(QByteArray *ba)
{
    OutDataBufMtx.lock();
    OutDataBuf = ba->data();
    OutDataBufMtx.unlock();
}

void SerialThread::Timeout()
{
    emit timeout();
    TimeoutTimer->stop();
}

void SerialThread::stop()
{
    ClosePortAndFinishThread = true;
}
