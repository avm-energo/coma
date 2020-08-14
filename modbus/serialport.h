#ifndef SERIALPORT_H
#define SERIALPORT_H

#include "../gen/maindef.h"

#include <QObject>
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
    int Init(Settings settings);

signals:
    void Read(QByteArray);
    void State(ConnectionStates);
    void Reconnect();

private:
    QSerialPort *Port;

public slots:
    void WriteBytes(QByteArray ba);

private slots:
    void ReadBytes();
    void ErrorOccurred(QSerialPort::SerialPortError err);
};

#endif // SERIALPORT_H
