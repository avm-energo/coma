#include <QtTest/QTest>
#include "serialthread.h"
#include "publicclass.h"
#include <QTimer>

SerialThread::SerialThread(QObject *parent) :
    QObject(parent)
{
    NewReceive = true;
    ReadData = new QByteArray(50000, 0x00);
    ReadData->clear();
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
    connect(this,SIGNAL(finished()),port,SLOT(deleteLater()));
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
    if (NewReceive)
    {
        if (ba.at(0) == "<")
        {
            RcvDataLength = ba.at(1)*256+ba.at(2);
            ba.remove(1, 2); // убираем длину из посылки
        }
        else // для посылок - продолжений блоков
        {
            RcvDataLength = ba.at(0)*256+ba.at(1);
            ba.remove(0, 2);
        }
        NewReceive = false;
    }
    NothingReceived = false;
    ReadData->append(ba);
    if (ReadData->size() > RcvDataLength) // Если приняли больше, чем длина, указанная в начале блока (на самом деле RcvDataLength не учитывает
        // символ "<" в начале посылки
    emit newdataarrived(ba);
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

void SerialThread::InitiateWriteDataToPort(QByteArray *ba)
{
    ReadData->clear();
    NothingReceived = true;
    if (ba->size() <= 516) // 512 байт + заголовок 4 байта (>WFx)
    {
        DataToSend->clear(); // для того, чтобы правильно выйти по таймауту
        OutDataBuf = ba->data();
        ThereIsDataToSend = false;
    }
    else
    {
        OutDataBuf = ba->left(516);
        ba->remove(0, 516);
        DataToSend->clear();
        DataToSend->append(*ba);
        ThereIsDataToSend = true;
    }
}

void SerialThread::Timeout()
{
    if (NothingReceived)
    {
        emit timeout();
        TimeoutTimer->stop();
        emit receivecompleted();
    }
    else
    {
        if (ThereIsDataToSend) // если есть, что ещё послать
        {
            if (QString::fromLocal8Bit(*ReadData) == "<RDY") // надо проверить, получили ли правильный промежуточный ответ
            {
                ReadData->clear();
                if (DataToSend->size() > 512)
                {
                    OutDataBuf = DataToSend->left(512);
                    DataToSend->remove(0, 512);
                }
                else if (DataToSend->size())
                {
                    OutDataBuf = DataToSend->data();
                    DataToSend->clear();
                    ThereIsDataToSend = false;
                }
                else
                {
                    ThereIsDataToSend = false;
                    TimeoutTimer->stop();
                    emit receivecompleted();
                }
            }
        }
        else
        {
            DataToSend->clear();
            ThereIsDataToSend = false;
            TimeoutTimer->stop();
            emit receivecompleted();
        }
    }
    NewReceive = true;
}

void SerialThread::stop()
{
    ClosePortAndFinishThread = true;
}
