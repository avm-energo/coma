#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QPointer>
#include <QSerialPort>
#include <QTimer>
#include <interfaces/ports/baseport.h>
#include <interfaces/types/settingstypes.h>

class SerialPort final : public BasePort
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    bool init(SerialPortSettings settings);

private:
    UniquePointer<QSerialPort> port;
    QTimer *timeoutTimer;

    bool connect() override;
    void disconnect() override;
    QByteArray read(bool *status = nullptr) override;
    virtual bool write(const QByteArray &ba) override;

public slots:
    bool clear();

private slots:
    void errorOccurred(const QSerialPort::SerialPortError err);
};

#endif // SERIALPORT_H
