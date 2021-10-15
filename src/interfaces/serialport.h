#ifndef SERIALPORT_H
#define SERIALPORT_H

#include "settingstypes.h"

#include <QPointer>
#include <QSerialPort>

class SerialPort : public QObject
{

    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort();
    bool init(SerialPortSettings settings);
    bool clear();

signals:
    void read(QByteArray);
    void connected();
    void error();

private:
    QPointer<QSerialPort> port;
    bool m_connected = false;

public slots:
    void writeBytes(QByteArray ba);
    void disconnect();
    bool reconnect();

private slots:
    void readBytes();
    void errorOccurred(QSerialPort::SerialPortError err);
};

#endif // SERIALPORT_H
