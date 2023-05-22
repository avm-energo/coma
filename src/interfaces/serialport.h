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

private:
    QPointer<QSerialPort> port;
    QTimer *m_connectionTimer;

public slots:
    bool writeData(const QByteArray &ba) override;
    bool connect() override;
    void disconnect() override;
    void poll() override {};
    QByteArray readData() override
    {
        return {};
    };

private slots:
    void readBytes();
    void errorOccurred(QSerialPort::SerialPortError err);
};

#endif // SERIALPORT_H
