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
    port->deleteLater();
    // disconnect();
    // emit finished();
}

bool SerialPort::init(SerialPortSettings settings)
{
    port = new QSerialPort(settings.Port);
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
    QObject::connect(port, &QSerialPort::errorOccurred, this, &SerialPort::errorOccurred);
    // QObject::connect(port, &QIODevice::readyRead, this, &SerialPort::readBytes);

    m_connectionTimer = new QTimer(this);
    m_connectionTimer->setInterval(TIMEOUT);
    QObject::connect(port, &QIODevice::bytesWritten, this, [&] { m_connectionTimer->start(); });
    QObject::connect(port, &QIODevice::readyRead, m_connectionTimer, &QTimer::stop);
    QObject::connect(m_connectionTimer, &QTimer::timeout, this, [this] {
        qWarning() << this->metaObject()->className() << Error::Timeout;
        emit clearQueries();
        // reconnect();
    });
    return connect();
}

bool SerialPort::clear()
{
    return port->clear();
}

bool SerialPort::writeData(const QByteArray &ba)
{
    if (!port->isOpen())
        return false;
    m_dataGuard.lock();
    auto bytes = port->write(ba.data(), ba.size());
    m_dataGuard.unlock();
    writeLog(ba.toHex(), Interface::ToDevice);
    if (bytes <= 0)
    {
        qCritical() << "Error with data writing";
        reconnect();
    }
    QCoreApplication::processEvents();
    return true;
}

bool SerialPort::connect()
{
    if (!port->open(QIODevice::ReadWrite))
    {
        qCritical() << port->metaObject()->className() << port->portName() << Error::OpenError;
        return false;
    }
    else
    {
        setState(Interface::State::Run);
        emit started();
        return true;
    }
}

void SerialPort::disconnect()
{
    if (port->isOpen())
        port->close();
    // setState(Interface::State::Disconnect);
}

void SerialPort::errorOccurred(QSerialPort::SerialPortError err)
{
    if (err == QSerialPort::NoError)
        return;
    else if (err == QSerialPort::NotOpenError || err == QSerialPort::ResourceError || err == QSerialPort::TimeoutError)
    {
        qWarning() << QVariant::fromValue(err).toString();
        emit error(ReadError);
    }
    else
        qDebug() << QVariant::fromValue(err).toString();
    reconnect();
}

QByteArray SerialPort::read(bool *status)
{
    Q_UNUSED(status);
    QByteArray ba;
    while (port->bytesAvailable())
    {
        ba += port->readAll();
        QThread::msleep(2);
    }
    if (ba.isEmpty())
    {
        QCoreApplication::processEvents();
        emit error(NoData);
    }
    return ba;
};

// void SerialPort::readBytes()
//{
//    QByteArray ba;
//    while (port->bytesAvailable())
//    {
//        ba += port->readAll();
//        QThread::msleep(2);
//    }
//    if (ba.size())
//        emit dataReceived(ba);
//}
