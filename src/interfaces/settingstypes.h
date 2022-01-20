#pragma once
#include "usbhidsettings.h"

#include <QObject>
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
    quint32 Baud;
    QString Parity;
    QString Stop;
    quint8 Address;
    QString Port;
    bool isValid() const
    {
        return !Port.isEmpty();
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
