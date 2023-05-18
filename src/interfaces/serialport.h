#ifndef SERIALPORT_H
#define SERIALPORT_H

#include "baseport.h"
#include "settingstypes.h"

#include <QPointer>
#include <QSerialPort>
#include <QTimer>

class SerialPort : public BasePort
{

    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort();
    bool init(SerialPortSettings settings);
    bool clear();

signals:
    void error();

private:
    QPointer<QSerialPort> port;
    QTimer *m_connectionTimer;

public slots:
    bool writeData(const QByteArray &ba) override;
    void disconnect() override;
    bool reconnect() override;
    void poll() override {};

private slots:
    void readBytes();
    void errorOccurred(QSerialPort::SerialPortError err);
};

#endif // SERIALPORT_H
