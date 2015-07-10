#include <QCoreApplication>
#include <QEventLoop>
#include <QTime>
#include "serialthread.h"
#include "publicclass.h"

SerialThread::SerialThread(QObject *parent) :
    QObject(parent)
{
    DataToSend = new QByteArray(1000, 0x00);
    OutDataBuf.clear();
    ClosePortAndFinishThread = false;

}

void SerialThread::run()
{
    port = new QSerialPort;
    port->setPort(portinfo);
    connect(port,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(Error(QSerialPort::SerialPortError)));
    if (!port->open(QIODevice::ReadWrite))
    {
        emit finished();
        return;
    }
    port->setBaudRate(115200);
    port->setParity(QSerialPort::NoParity);
    port->setDataBits(QSerialPort::Data8);
    port->setFlowControl(QSerialPort::NoFlowControl);
    port->setStopBits(QSerialPort::OneStop);
    connect(port,SIGNAL(readyRead()),this,SLOT(CheckForData()));
    emit canalisready();
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
            }
            emit finished();
            return;
        }
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void SerialThread::CheckForData()
{
    QByteArray ba = port->read(1000);
    emit newdataarrived(ba);
}

void SerialThread::WriteData()
{
    qint64 res = port->write(OutDataBuf);
    if (res == -1)
        emit datawritten(QByteArray()); // ошибка
    emit datawritten(OutDataBuf); // всё гут
    OutDataBuf.clear();
}

void SerialThread::InitiateWriteDataToPort(QByteArray ba)
{
    OutDataBufMtx.lock();
    OutDataBuf = ba;
    OutDataBufMtx.unlock();
}

void SerialThread::stop()
{
    ClosePortAndFinishThread = true;
}

void SerialThread::Error(QSerialPort::SerialPortError err)
{
    if (!err) // нет ошибок
        return;
    quint16 ernum = err + 50;
    emit error(ernum);
}
