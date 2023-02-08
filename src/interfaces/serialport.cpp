#include "serialport.h"

#include "baseinterface.h"

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
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

    m_connectionTimer = new QTimer(this);
    m_connectionTimer->setInterval(TIMEOUT);
    connect(port, &QIODevice::bytesWritten, this, [&] { m_connectionTimer->start(); });
    connect(port, &QIODevice::readyRead, m_connectionTimer, &QTimer::stop);
    connect(m_connectionTimer, &QTimer::timeout, [this] {
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
    if (port->isOpen())
        port->close();
}

bool SerialPort::reconnect()
{
    disconnect();
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
    {
        ba += port->readAll();
        QThread::msleep(2);
    }
    if (ba.size())
        emit read(ba);
}
