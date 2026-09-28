#include "dialogs/connDialogs/IEC104Dialog/interfaceethernetdialog.h"

#include <interfaces/types/iec104_settings.h>
#include <interfaces/types/settings_keys.h>
#include <libavm-gen/settings.h>

InterfaceEthernetDialog::InterfaceEthernetDialog(QWidget *parent) : AbstractEthernetDialog(parent) { }

quint16 InterfaceEthernetDialog::defaultPort() const
{
    return Settings::get(SettingsKeys::Iec104::iec104DefaultPort, 2404);
}

quint16 InterfaceEthernetDialog::defaultAddress() const
{
    return Settings::get(SettingsKeys::Iec104::iec104DefaultBsAddress, 205);
}

QString InterfaceEthernetDialog::settingsGroup() const
{
    return "Ethernet";
}

QString InterfaceEthernetDialog::addressLabel() const
{
    return "Адрес БС";
}

QString InterfaceEthernetDialog::addressSettingsKey() const
{
    return "iec104BsAddress";
}

QString InterfaceEthernetDialog::zeroAddressError() const
{
    return "Адрес базовой станции не может быть равен нулю";
}

BaseSettings *InterfaceEthernetDialog::buildSettings(const QString &ip, quint16 port, quint16 address) const
{
    IEC104Settings *settings = new IEC104Settings;
    settings->set("ip", ip);
    settings->set("port", port);
    settings->set("bsAddress", address);
    settings->set("timeout", Settings::get("iec104Timeout", 1000));
    settings->set("reconnectInterval", Settings::get("iec104Reconnect", 1000));
    settings->set("disconnectTimeout", Settings::get("iec104DisconnectTimeout", 5000));
    settings->set("connectTimeout", Settings::get("iec104ConnectTimeout", 5000));
    settings->set("t0", Settings::get("iec104T0", 30));
    settings->set("t1", Settings::get("iec104T1", 15));
    settings->set("t2", Settings::get("iec104T2", 10));
    settings->set("t3", Settings::get("iec104T3", 20));
    settings->set("k", Settings::get("iec104K", 12));
    settings->set("w", Settings::get("iec104W", 8));

    if (!settings->isValid())
    {
        delete settings;
        return nullptr;
    }
    return settings;
}

ConnectionSettings InterfaceEthernetDialog::wrapSettings(const QString &name, BaseSettings *settings) const
{
    return ConnectionSettings { name, qobject_cast<IEC104Settings *>(settings) };
}
