#include <QtTest/QTest>
#include "serialthread.h"
#include "publicclass.h"
#include <QTimer>
#include "config.h"

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
    ReadDataMtx.lock();
    ReadData->append(ba);
    if (NewReceive) // если предыдущие все посылки обработаны, и мы имеем дело с новой посылкой
    {
        if (ReadData->size() > 2) // если есть, чего обрабатывать
        {
            if ((ReadData->data()[0] == 0x3c) && (!ThereMustBeDataToReceive)) // если это начало посылки, а не следующий сегмент текущей посылки
            {
                RcvDataLength = ReadData->at(1)*256+ReadData->at(2);
                ReadData->remove(1, 2); // убираем длину из посылки
                NewReceive = false;
            }
            else if (ThereMustBeDataToReceive) // для посылок - продолжений блоков длина посылка находится в первых двух байтах
            {
                RcvDataLength = ReadData->at(0)*256+ReadData->at(1);
                ReadData->remove(0, 2);
                NewReceive = false;
            }
            else // посылка не распознана - игнорируем
                ReadData->clear();
        }
    }
    if (ReadData->size() > RcvDataLength) // Если приняли больше, чем длина, указанная в начале блока (на самом деле RcvDataLength не учитывает
        // символ "<" в начале посылки)
    {
        emit receivecompleted(); // очередная посылка принята успешно, надо разбирать
        TimeoutTimer->stop();
    }
    else // если ещё не принято, перезапускаем таймер таймаута
    {
        if (TimeoutTimer->isActive())
            TimeoutTimer->start();
    }
    ReadDataMtx.unlock();
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

QByteArray SerialThread::data()
{
    ReadDataMtx.lock();
    QByteArray ba = *ReadData;
    ReadData->clear();
    return ba;
    ReadDataMtx.unlock();
}

void SerialThread::InitiateWriteDataToPort(QByteArray *ba)
{
    ReadDataMtx.lock();
    OutDataBufMtx.lock();
    ReadData->clear();
    ReadDataMtx.unlock();
    NothingReceived = true;
    if (ba->size() <= 519) // 512 байт + заголовок 7 байт (>,32,~32,№,длина)
    {
        DataToSend->clear(); // для того, чтобы правильно выйти по таймауту
        OutDataBuf = ba->data();
        ThereIsDataToSend = false;
    }
    else
    {
        OutDataBuf = ba->left(519);
        ba->remove(0, 519);
        DataToSend->clear();
        DataToSend->append(ba->data());
        ThereIsDataToSend = true;
    }
    OutDataBufMtx.unlock();
}

void SerialThread::Timeout()
{
    if (NothingReceived)
    {
        emit timeout();
        TimeoutTimer->stop();
//        emit receivecompleted();
    }
    else
    {
        if (ThereIsDataToSend) // если есть, что ещё послать
        {
            // надо проверить, получили ли правильный промежуточный ответ
            if (ReadData->at(0) == 0x55)
            {
                ReadDataMtx.lock();
                OutDataBufMtx.lock();
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
                ReadDataMtx.unlock();
                OutDataBufMtx.unlock();
            }
            else
                emit error(SegFaultError);
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
