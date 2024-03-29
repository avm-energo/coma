#pragma once

#include <QSerialPort>
#include <interfaces/types/iec104_settings.h>
#include <interfaces/types/usbhidsettings.h>
#include <variant>

struct SerialPortSettings final : public BaseSettings
{
    QString name;
    quint32 baud;
    quint8 address;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stop;

    bool isValid() const
    {
        return !name.isEmpty();
    }
};

struct EmulatorSettings final : public BaseSettings
{
    quint16 typeB;
    quint16 typeM;
};

struct ConnectStruct
{
    QString name;
    std::variant<IEC104Settings, SerialPortSettings, UsbHidSettings, EmulatorSettings> settings;
};
