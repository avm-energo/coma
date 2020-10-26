#include "serialport.h"

#include "../gen/board.h"

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

Error::Msg SerialPort::Init(BaseInterface::SerialPortSettings settings)
{
    Q_ASSERT(Board::GetInstance().interfaceType() == Board::InterfaceType::RS485);

    Port = new QSerialPort(settings.Port);
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
    if (Port->open(QIODevice::ReadWrite))
        Board::GetInstance().setConnectionState(Board::ConnectionState::Connected);
    else
    {
        Board::GetInstance().setConnectionState(Board::ConnectionState::Closed);
        ERMSG("Error opening COM-port");
        return Error::Msg::GeneralError;
    }
    QTimer *connectionTimer = new QTimer(this);
    connectionTimer->setInterval(TIMEOUT);
    connect(Port, &QIODevice::bytesWritten, [connectionTimer] { connectionTimer->start(); });
    connect(Port, &QIODevice::readyRead, connectionTimer, &QTimer::stop);
    connect(connectionTimer, &QTimer::timeout, [] { qCritical("RS485 Timeout Error"); });
    return Error::Msg::NoError;
}

void SerialPort::WriteBytes(QByteArray ba)
{
    Port->write(ba.data(), ba.size());
    QCoreApplication::processEvents();
}

void SerialPort::Disconnect()
{
    Port->close();
}

void SerialPort::ErrorOccurred(QSerialPort::SerialPortError err)
{
    if (!err)
        return;
    qCritical() << QVariant::fromValue(err).toString();
    Board::GetInstance().setConnectionState(Board::ConnectionState::Closed);
    emit Reconnect();
}

void SerialPort::ReadBytes()
{
    QByteArray ba;
    while (Port->bytesAvailable())
        ba += Port->readAll();
    if (ba.size())
        emit Read(ba);
}
