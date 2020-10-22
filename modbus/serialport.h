#ifndef SERIALPORT_H
#define SERIALPORT_H

#include "../gen/error.h"

#include <QPointer>
#include <QSerialPort>

class SerialPort : public QObject
{

    Q_OBJECT
public:
    struct Settings
    {
        quint32 Baud;
        QString Parity;
        QString Stop;
        quint8 Address;
        QString Port;
    };

    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort();
    Error::Msg Init(Settings settings);

signals:
    void Read(QByteArray);

    void Reconnect();

private:
    QPointer<QSerialPort> Port;

public slots:
    void WriteBytes(QByteArray ba);
    void Disconnect();

private slots:
    void ReadBytes();
    void ErrorOccurred(QSerialPort::SerialPortError err);
};

#endif // SERIALPORT_H
