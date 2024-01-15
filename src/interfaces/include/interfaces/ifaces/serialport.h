#pragma once

#include <QSerialPort>
#include <interfaces/ifaces/baseinterface.h>
#include <interfaces/types/settingstypes.h>

class SerialPort final : public BaseInterface
{
    Q_OBJECT
private:
    QSerialPort *m_port;

public:
    explicit SerialPort(const SerialPortSettings &settings, QObject *parent = nullptr);

private:
    bool connect() override;
    void disconnect() override;
    QByteArray read(bool &status) override;
    bool write(const QByteArray &ba) override;

private slots:
    void errorOccurred(const QSerialPort::SerialPortError err);
};
