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
    if (port->isOpen())
        port->close();
}

bool SerialPort::init(SerialPortSettings settings)
{
    m_connected = true;
    port = new QSerialPort(settings.Port, this);
    port->setBaudRate(settings.Baud);
    port->setDataBits(QSerialPort::Data8);
    if (settings.Parity == "Нет")
        port->setParity(QSerialPort::NoParity);
    else if (settings.Parity == "Чет")
        port->setParity(QSerialPort::EvenParity);
    else
        port->setParity(QSerialPort::OddParity);
    port->setStopBits(settings.Stop == "1" ? QSerialPort::OneStop : QSerialPort::TwoStop);
    port->setFlowControl(QSerialPort::NoFlowControl);
    port->setReadBufferSize(1024);
    connect(port.data(), &QSerialPort::errorOccurred, this, &SerialPort::errorOccurred);
    connect(port, &QIODevice::readyRead, this, &SerialPort::readBytes);

    QTimer *connectionTimer = new QTimer(this);
    connectionTimer->setInterval(TIMEOUT);
    connect(port, &QIODevice::bytesWritten, this, [connectionTimer] { connectionTimer->start(); });
    connect(port, &QIODevice::readyRead, connectionTimer, &QTimer::stop);
    connect(connectionTimer, &QTimer::timeout, [=] {
        qInfo() << this->metaObject()->className() << Error::Timeout;
        reconnect();
    });
    return reconnect();
}

bool SerialPort::clear()
{
    return port->clear();
}

void SerialPort::writeBytes(QByteArray ba)
{
    if (!port->isOpen())
        return;
    port->write(ba.data(), ba.size());
    QCoreApplication::processEvents();
}

void SerialPort::disconnect()
{
    m_connected = false;
    port->close();
}

bool SerialPort::reconnect()
{
    if (!m_connected)
        return false;
    if (!port->open(QIODevice::ReadWrite))
    {
        qCritical() << port->metaObject()->className() << port->portName() << Error::OpenError;
        return false;
    }
    emit connected();
    return true;
}

void SerialPort::errorOccurred(QSerialPort::SerialPortError err)
{
    if (!err)
        return;
    if (port->isOpen())
        port->close();
    if (err == QSerialPort::NotOpenError || err == QSerialPort::ResourceError || err == QSerialPort::TimeoutError)
    {
        qCritical() << QVariant::fromValue(err).toString();
        emit error();
    }
    else
        qDebug() << QVariant::fromValue(err).toString();
}

void SerialPort::readBytes()
{
    QByteArray ba;
    while (port->bytesAvailable())
        ba += port->readAll();
    if (ba.size())
        emit read(ba);
}
