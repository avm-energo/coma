#include "interfaces/ifaces/serialport.h"

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QTimer>

constexpr auto TIMEOUT = 3000;

SerialPort::SerialPort(const SerialPortSettings &settings, QObject *parent)
    : BaseInterface("ModbusPort", parent), m_port(new QSerialPort(settings.name, this)), timeoutTimer(new QTimer(this))
{
    m_port->setBaudRate(settings.baud);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(settings.parity);
    m_port->setStopBits(settings.stop);
    m_port->setFlowControl(QSerialPort::NoFlowControl);
    m_port->setReadBufferSize(1024);

    timeoutTimer->setInterval(TIMEOUT);
    QObject::connect(m_port, &QSerialPort::errorOccurred, this, &SerialPort::errorOccurred);
    QObject::connect(m_port, &QIODevice::bytesWritten, timeoutTimer, //
        [timeoutTimer = timeoutTimer] { timeoutTimer->start(); });
    QObject::connect(m_port, &QIODevice::readyRead, timeoutTimer, &QTimer::stop);
    QObject::connect(timeoutTimer, &QTimer::timeout, this, [this] {
        emit error(InterfaceError::Timeout);
        qWarning() << this->metaObject()->className() << Error::Timeout;
        writeLog(Error::Timeout);
        emit clearQueries();
    });
}

bool SerialPort::connect()
{
    if (!m_port->open(QIODevice::ReadWrite))
    {
        qCritical() << m_port->metaObject()->className() << m_port->portName() << Error::OpenError;
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
    if (m_port->isOpen())
        m_port->close();
}

// blocking read from serial port with timeout implementation
QByteArray SerialPort::read(bool *status)
{
    Q_UNUSED(status);
    QByteArray data;
    bool readyRead = true;                        // enabling read flag
    if (!m_port->bytesAvailable())                // if no data
        readyRead = m_port->waitForReadyRead(10); // wait data (timeout 10 ms)
    if (readyRead)
    {
        m_dataGuard.lock(); // lock port
        while (m_port->bytesAvailable())
        {
            data += m_port->readAll(); // read data
            QThread::msleep(2);
        }
        m_dataGuard.unlock(); // unlock port
    }
    if (data.isEmpty())
        QCoreApplication::processEvents();
    return data;
};

bool SerialPort::write(const QByteArray &ba)
{
    if (!m_port->isOpen())
        return false;
    m_dataGuard.lock();                               // lock port
    auto bytes = m_port->write(ba.data(), ba.size()); // write data
    m_dataGuard.unlock();                             // unlock port
    if (bytes <= 0)
    {
        qCritical() << "Error with data writing";
        emit error(InterfaceError::WriteError);
        return false;
    }
    return true;
}

bool SerialPort::tryToReconnect()
{
    disconnect();  // Закрываем порт
    if (connect()) // Заново открываем порт
    {
        // Если порт открылся, то это не даёт гарантии, что устройство вышло на связь
        ;
    }

    return false;
}

void SerialPort::errorOccurred(const QSerialPort::SerialPortError err)
{
    switch (err)
    {
    // ignore these cases
    case QSerialPort::SerialPortError::NoError:
    case QSerialPort::SerialPortError::TimeoutError:
        break;
    // read error
    case QSerialPort::SerialPortError::ReadError:
        emit error(InterfaceError::ReadError);
        break;
    // write error
    case QSerialPort::SerialPortError::WriteError:
        emit clearQueries();
        emit error(InterfaceError::WriteError);
        break;
    // port open error
    case QSerialPort::SerialPortError::DeviceNotFoundError:
    case QSerialPort::SerialPortError::PermissionError:
    case QSerialPort::SerialPortError::OpenError:
        break;
    // port unexpected removed
    case QSerialPort::SerialPortError::ResourceError:
        break;
    default:
        qWarning() << QVariant::fromValue(err).toString();
    }
}
