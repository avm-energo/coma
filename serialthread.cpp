#include <QCoreApplication>
#include <QEventLoop>
#include <QTime>
#include "serialthread.h"
#include "publicclass.h"
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
    TimeoutTimer->setInterval(10000);
    connect(TimeoutTimer, SIGNAL(timeout()),this,SLOT(Timeout()));
    port = new QSerialPort;
    port->setPort(portinfo);
    if (!port->open(QIODevice::ReadWrite))
    {
        emit finished();
        emit error(1);
        return;
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
        OutDataBufMtx.lock();
        if (!OutDataBuf.isEmpty()) // что-то пришло в выходной буфер для записи
            WriteData();
        OutDataBufMtx.unlock();
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
        QTime tmr;
        tmr.start();
        while (tmr.elapsed() < 100)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void SerialThread::CheckForData()
{
    TimeoutTimer->stop();
    QByteArray ba = port->read(1000);
    emit newdataarrived(ba);
}

void SerialThread::WriteData()
{
    qint64 res = port->write(OutDataBuf);
    if (res == -1)
        emit datawritten(QByteArray()); // ошибка
    TimeoutTimer->start();
    emit datawritten(OutDataBuf); // всё гут
    OutDataBuf.clear();
}

void SerialThread::InitiateWriteDataToPort(QByteArray *ba)
{
    OutDataBufMtx.lock();
    OutDataBuf = *ba;
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
