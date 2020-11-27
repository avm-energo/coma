#pragma once
#include <variant>
#include <QObject>

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

struct UsbHidSettings
{
    explicit UsbHidSettings() = default;
    explicit UsbHidSettings(unsigned short dev, unsigned short pid, const QString &arr, const QString &str)
        : vendor_id(dev), product_id(pid), serial(arr), path(str)
    {
    }
    // Не сравниваем path т.к. если переподключить устройство то path может сменится но остальные параметры - нет
    friend bool operator==(const UsbHidSettings &lhs, const UsbHidSettings &rhs)
    {
        return ((lhs.product_id == rhs.product_id) && (lhs.vendor_id == rhs.vendor_id) && (lhs.serial == rhs.serial));
    }
    friend bool operator!=(const UsbHidSettings &lhs, const UsbHidSettings &rhs)
    {
        return !(lhs == rhs);
    }

    unsigned short vendor_id;
    unsigned short product_id;
    QString serial;
    QString path;
};

struct ConnectStruct
{
    QString name;
    std::variant<IEC104Settings, SerialPortSettings, UsbHidSettings> settings;
};
