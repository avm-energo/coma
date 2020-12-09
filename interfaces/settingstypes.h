#pragma once
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
    /// From Windows message string
    static UsbHidSettings fromWString(std::wstring &wstr)
    {
        auto strList = QString::fromStdWString(wstr).split("#");
        strList.pop_back();
        strList.pop_front();
        auto vidpidList = strList.front().split("&");
        const unsigned short vid = vidpidList.first().split("_").last().toInt(nullptr, 16);
        const unsigned short pid = vidpidList.last().split("_").last().toInt(nullptr, 16);
        const QString serialNumber = strList.last().split("_").last();
        UsbHidSettings settings { vid, pid, serialNumber, NULL };
        return settings;
    }
};

struct ConnectStruct
{
    QString name;
    std::variant<IEC104Settings, SerialPortSettings, UsbHidSettings> settings;
};
