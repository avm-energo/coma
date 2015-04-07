#include <QtTest/QTest>
#include "serialthread.h"
#include "publicclass.h"
#include <QTimer>

SerialThread::SerialThread(QObject *parent) :
    QObject(parent)
{
    ReadData = new QByteArray(50000, 0x00);
    ReadData->clear();
    OutDataBuf.clear();
    ClosePortAndFinishThread = false;
}

void SerialThread::run()
{
    TimeoutTimer = new QTimer;
    TimeoutTimer->setInterval(400);
    connect(TimeoutTimer, SIGNAL(timeout()),this,SLOT(Timeout())); // temporary
    port = new QSerialPort;
    port->setPort(portinfo);
    port->setBaudRate(baud);
    port->setParity(QSerialPort::NoParity);
    port->setDataBits(QSerialPort::Data8);
    port->setFlowControl(QSerialPort::NoFlowControl);
    port->setStopBits(QSerialPort::OneStop);
    if (!port->open(QIODevice::ReadWrite))
    {
        emit finished();
        emit error(1);
    }
    connect(port,SIGNAL(readyRead()),this,SLOT(CheckForData()));
    while (1)
    {
        if (OutDataBuf.size()>0) // что-то пришло в выходной буфер для записи
            WriteData();
        if (ClosePortAndFinishThread)
        {
            emit finished();
            if (port->isOpen())
            {
                port->close();
                delete port;
                TimeoutTimer->stop();
                delete TimeoutTimer;
            }
            break;
        }
        QTest::qWait(100);
    }
}

void SerialThread::SetPort(QString str)
{
    QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts();
    for (int i = 0; i < info.size(); i++)
    {
        if (info.at(i).portName() == str)
        {
            portinfo = info.at(i);
            return;
        }
    }
}

void SerialThread::SetBaud(QString str)
{
    baud = str.toInt();
}

void SerialThread::CheckForData()
{
    QByteArray ba = port->read(1000000);
    NothingReceived = false;
    ReadData->append(ba);
    emit newdataarrived(ba);
//    emit receivecompleted(); // temporary
    if (TimeoutTimer->isActive())
        TimeoutTimer->start();
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

QByteArray SerialThread::data()
{
    QByteArray ba = *ReadData;
    ReadData->clear();
    return ba;
}

void SerialThread::InitiateWriteDataToPort(QByteArray ba)
{
    ReadData->clear();
    NothingReceived = true;
    OutDataBuf = ba;
}

void SerialThread::Timeout()
{
    if (NothingReceived)
        emit timeout();
    TimeoutTimer->stop();
    emit receivecompleted();
}
