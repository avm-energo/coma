#include "serialport.h"

#include "../gen/error.h"

#include <QCoreApplication>

SerialPort::SerialPort(QObject *parent) : QObject(parent)
{
}

SerialPort::~SerialPort()
{
    if (Port->isOpen())
        Port->close();
}

int SerialPort::Init(SerialPort::Settings settings)
{
    //    if (Port != 0)
    //    {
    //        if (Port->isOpen())
    //            Port->close();
    //    }
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
    connect(Port, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this,
        SLOT(ErrorOccurred(QSerialPort::SerialPortError)));
    connect(Port, SIGNAL(readyRead()), this, SLOT(ReadBytes()));
    if (Board::GetInstance()->interfaceType() == Board::InterfaceType::RS485)
        if (Port->open(QIODevice::ReadWrite) == true)
            Board::GetInstance()->setConnectionState(Board::ConnectionState::ConnectedState);
        else
        {
            Board::GetInstance()->setConnectionState(Board::ConnectionState::ClosingState);
            ERMSG("Error opening COM-port");
            return GENERALERROR;
        }
    return NOERROR;
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
    if (err == 0)
        ; // no error
          //        emit State(ConnectionStates::ConnectedState);
    else
    {
        if (Board::GetInstance()->interfaceType() == Board::InterfaceType::RS485)
        {
            Board::GetInstance()->setConnectionState(Board::ConnectionState::ClosingState);
            emit Reconnect();
        }
    }
}

void SerialPort::ReadBytes()
{
    QByteArray ba;
    while (Port->bytesAvailable())
        ba += Port->readAll();
    if (ba.size())
        emit Read(ba);
}
