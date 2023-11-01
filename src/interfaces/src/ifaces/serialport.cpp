#include "interfaces/ifaces/serialport.h"

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QTimer>

constexpr auto TIMEOUT = 3000;

SerialPort::SerialPort(const SerialPortSettings &settings, QObject *parent)
    : BaseInterface("ModbusPort", parent), m_port(new QSerialPort(settings.name, this))
{
    m_port->setBaudRate(settings.baud);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(settings.parity);
    m_port->setStopBits(settings.stop);
    m_port->setFlowControl(QSerialPort::NoFlowControl);
    m_port->setReadBufferSize(1024);
    QObject::connect(m_port, &QSerialPort::errorOccurred, this, &SerialPort::errorOccurred);
}

bool SerialPort::connect()
{
    if (m_port->open(QIODevice::ReadWrite))
    {
        if (getState() != Interface::State::Disconnect)
        {
            setState(Interface::State::Run);
            emit started();
            return true;
        }
    }
    else
        m_log.error("Can't open " + m_port->portName());
    return false;
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
    m_port->clear(QSerialPort::Direction::Input);     // flush input buffer
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
        emit error(InterfaceError::OpenError);
        break;
    // port unexpected removed
    case QSerialPort::SerialPortError::ResourceError:
        break;
    default:
        qWarning() << QVariant::fromValue(err).toString();
    }
}
