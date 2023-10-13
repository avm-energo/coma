#pragma once

#include <QSerialPort>
#include <interfaces/types/usbhidsettings.h>
#include <variant>

struct IEC104Settings
{
    quint16 baseadr;
    QString ip;

    bool isValid() const
    {
        auto parts = ip.split(".");
        bool ok = true;
        for (const auto &part : parts)
            if ((part.toUInt(&ok) >= 256) || !ok)
                return false;
        return ok;
    }
};

struct SerialPortSettings
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

struct EmulatorSettings
{
    quint16 typeB;
    quint16 typeM;
};

struct ConnectStruct
{
    QString name;
    std::variant<IEC104Settings, SerialPortSettings, UsbHidSettings, EmulatorSettings> settings;
};

// how much entries can we have for interfaces of each type in registry
constexpr int MAXREGISTRYINTERFACECOUNT = 5;
