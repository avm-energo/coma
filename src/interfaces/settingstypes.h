#pragma once
#include "usbhidsettings.h"

#include <QObject>
#include <variant>
struct IEC104Settings
{
    quint16 baseadr;
    QString ip;
};

struct SerialPortSettings
{
    quint32 Baud;
    QString Parity;
    QString Stop;
    quint8 Address;
    QString Port;
};

struct ConnectStruct
{
    QString name;
    std::variant<IEC104Settings, SerialPortSettings, UsbHidSettings> settings;
};

// how much entries can we have for interfaces of each type in registry
constexpr int MAXREGISTRYINTERFACECOUNT = 5;
