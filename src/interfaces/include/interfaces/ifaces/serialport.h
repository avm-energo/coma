#pragma once

#include <QPointer>
#include <QSerialPort>
#include <QTimer>
#include <interfaces/ifaces/baseinterface.h>
#include <interfaces/types/settingstypes.h>

class SerialPort final : public BaseInterface
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    void init(SerialPortSettings settings);
    bool connect() override;
    void reconnect() override;

private:
    UniquePointer<QSerialPort> port;
    QTimer *timeoutTimer;

    void disconnect() override;
    QByteArray read(bool *status = nullptr) override;
    virtual bool write(const QByteArray &ba) override;

private slots:
    void errorOccurred(const QSerialPort::SerialPortError err);
};
