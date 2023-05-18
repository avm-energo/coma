#include "serialport.h"

#include "baseinterface.h"

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QTimer>

#define TIMEOUT 3000
SerialPort::SerialPort(QObject *parent) : BasePort("ModbusPort", parent)
{
}

SerialPort::~SerialPort()
{
    disconnect();
    emit finished();
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
    connect(m_connectionTimer, &QTimer::timeout, this, [this] {
        qInfo() << this->metaObject()->className() << Error::Timeout;
        reconnect();
    });
    return reconnect();
}

bool SerialPort::clear()
{
    return port->clear();
}

bool SerialPort::writeData(const QByteArray &ba)
{
    if (!port->isOpen())
        return false;
    port->write(ba.data(), ba.size());
    QCoreApplication::processEvents();
    return true;
}

void SerialPort::disconnect()
{
    if (port->isOpen())
        port->close();
    setState(Interface::State::Stop);
}

bool SerialPort::reconnect()
{
    disconnect();
    if (!port->open(QIODevice::ReadWrite))
    {
        qCritical() << port->metaObject()->className() << port->portName() << Error::OpenError;
        return false;
    }
    emit started();
    setState(Interface::State::Run);
    return true;
}

void SerialPort::errorOccurred(QSerialPort::SerialPortError err)
{
    if (!err)
        return;
    if (err == QSerialPort::NotOpenError || err == QSerialPort::ResourceError || err == QSerialPort::TimeoutError)
    {
        qCritical() << QVariant::fromValue(err).toString();
        emit error();
    }
    else
        qDebug() << QVariant::fromValue(err).toString();
    reconnect();
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
        emit dataReceived(ba);
}
