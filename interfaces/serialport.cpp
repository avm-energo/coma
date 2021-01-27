#include "serialport.h"

#include "baseinterface.h"

#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

#define TIMEOUT 3000
SerialPort::SerialPort(QObject *parent) : QObject(parent)
{
}

SerialPort::~SerialPort()
{
    if (Port->isOpen())
        Port->close();
}

bool SerialPort::Init(SerialPortSettings settings)
{
    Port = new QSerialPort(settings.Port, this);
    Port->setBaudRate(settings.Baud);
    Port->setDataBits(QSerialPort::Data8);
    if (settings.Parity == "Нет")
        Port->setParity(QSerialPort::NoParity);
    else if (settings.Parity == "Чет")
        Port->setParity(QSerialPort::EvenParity);
    else
        Port->setParity(QSerialPort::OddParity);
    Port->setStopBits(settings.Stop == "1" ? QSerialPort::OneStop : QSerialPort::TwoStop);
    Port->setFlowControl(QSerialPort::NoFlowControl);
    Port->setReadBufferSize(1024);
    connect(Port.data(), &QSerialPort::errorOccurred, this, &SerialPort::ErrorOccurred);
    connect(Port, &QIODevice::readyRead, this, &SerialPort::ReadBytes);
    if (!Port->open(QIODevice::ReadWrite))
        return false;

    QTimer *connectionTimer = new QTimer(this);
    connectionTimer->setInterval(TIMEOUT);
    connect(Port, &QIODevice::bytesWritten, [connectionTimer] { connectionTimer->start(); });
    connect(Port, &QIODevice::readyRead, connectionTimer, &QTimer::stop);
    connect(connectionTimer, &QTimer::timeout, [=] {
        qInfo() << this->metaObject()->className() << Error::Timeout;
        reconnect();
    });
    return true;
}

void SerialPort::WriteBytes(QByteArray ba)
{
    if (!Port->isOpen())
        return;
    Port->write(ba.data(), ba.size());
    QCoreApplication::processEvents();
}

void SerialPort::Disconnect()
{
    Port->close();
}

void SerialPort::reconnect()
{
    if (!Port->open(QIODevice::ReadWrite))
    {
        qCritical() << Port->metaObject()->className() << Port->portName() << Error::OpenError;
    }
    else
        emit connected();
}

void SerialPort::ErrorOccurred(QSerialPort::SerialPortError err)
{
    if (!err)
        return;
    if (Port->isOpen())
        Port->close();
    if (err == QSerialPort::NotOpenError || err == QSerialPort::ResourceError || err == QSerialPort::TimeoutError)
    {
        qCritical() << QVariant::fromValue(err).toString();
        emit errorOccurred();
    }
    else
        qDebug() << QVariant::fromValue(err).toString();
}

void SerialPort::ReadBytes()
{
    QByteArray ba;
    while (Port->bytesAvailable())
        ba += Port->readAll();
    if (ba.size())
        emit Read(ba);
}
