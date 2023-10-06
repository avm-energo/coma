#include "interfaces/ifaces/serialport.h"

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QTimer>

#define TIMEOUT 3000

SerialPort::SerialPort(QObject *parent) : BaseInterface("ModbusPort", parent)
{
}

void SerialPort::init(SerialPortSettings settings)
{
    port.reset(new QSerialPort(settings.Port));
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
    timeoutTimer = new QTimer(this);
    timeoutTimer->setInterval(TIMEOUT);
    QObject::connect(port.get(), &QSerialPort::errorOccurred, this, &SerialPort::errorOccurred);
    QObject::connect(port.get(), &QIODevice::bytesWritten, this, [this] {
        timeoutTimer->start(); //
    });
    QObject::connect(port.get(), &QIODevice::readyRead, timeoutTimer, &QTimer::stop);
    QObject::connect(timeoutTimer, &QTimer::timeout, this, [this] {
        emit error(InterfaceError::Timeout);
        qWarning() << this->metaObject()->className() << Error::Timeout;
        writeLog(Error::Timeout);
        emit clearQueries();
    });
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
}

void SerialPort::reconnect()
{
    ;
}

// blocking read from serial port with timeout implementation
QByteArray SerialPort::read(bool *status)
{
    QByteArray ba;
    bool readyRead = true;                      // enabling read flag
    if (!port->bytesAvailable())                // if no data
        readyRead = port->waitForReadyRead(10); // wait data (timeout 10 ms)
    if (readyRead)
    {
        m_dataGuard.lock(); // lock port
        while (port->bytesAvailable())
        {
            ba += port->readAll(); // read data
            QThread::msleep(2);
        }
        m_dataGuard.unlock(); // unlock port
    }
    if (ba.isEmpty())
    {
        *status = false;
        QCoreApplication::processEvents();
    }
    else
        *status = true;
    return ba;
};

bool SerialPort::write(const QByteArray &ba)
{
    if (!port->isOpen())
        return false;
    m_dataGuard.lock();                             // lock port
    auto bytes = port->write(ba.data(), ba.size()); // write data
    m_dataGuard.unlock();                           // unlock port
    if (bytes <= 0)
    {
        qCritical() << "Error with data writing";
        emit error(InterfaceError::WriteError);
        return false;
    }
    return true;
}

void SerialPort::errorOccurred(const QSerialPort::SerialPortError err)
{
    // NOTE: TimeoutError is ok due calling the waitForReadyRead function of the QSerialPort instance.
    if (err == QSerialPort::NoError)
        return;
    else if (err == QSerialPort::NotOpenError || err == QSerialPort::ResourceError)
    {
        qWarning() << QVariant::fromValue(err).toString();
        emit error(InterfaceError::ReadError);
    }
    else
        qDebug() << QVariant::fromValue(err).toString();
}
