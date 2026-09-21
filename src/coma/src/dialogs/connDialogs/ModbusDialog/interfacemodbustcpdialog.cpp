#include "dialogs/connDialogs/ModbusDialog/interfacemodbustcpdialog.h"

#include <interfaces/types/modbustcp_settings.h>
#include <interfaces/types/settings_keys.h>
#include <libavm-gen/settings.h>

InterfaceModbusTcpDialog::InterfaceModbusTcpDialog(QWidget *parent) : InterfaceEthernetDialog(parent) { }

quint16 InterfaceModbusTcpDialog::defaultPort() const
{
    return Settings::get(SettingsKeys::ModbusTcp::modbusTcpDefaultPort, 502);
}

quint16 InterfaceModbusTcpDialog::defaultAddress() const
{
    return Settings::get(SettingsKeys::ModbusTcp::modbusTcpDefaultAddress, 1);
}

QString InterfaceModbusTcpDialog::settingsGroup() const
{
    return "ModbusTCP";
}

QString InterfaceModbusTcpDialog::addressLabel() const
{
    return "Адрес";
}

QString InterfaceModbusTcpDialog::addressSettingsKey() const
{
    return "modbusAddress";
}

QString InterfaceModbusTcpDialog::zeroAddressError() const
{
    return "Адрес устройства не может быть равен нулю";
}

BaseSettings *InterfaceModbusTcpDialog::buildSettings(const QString &ip, quint16 port, quint16 address) const
{
    ModbusTcpSettings *settings = new ModbusTcpSettings;
    settings->set("ip", ip);
    settings->set("port", port);
    settings->set("address", address);
    settings->set("timeout", Settings::get(SettingsKeys::ModbusTcp::modbusTcpTimeout, 3000));
    settings->set("reconnectInterval", Settings::get(SettingsKeys::ModbusTcp::modbusTcpReconnect, 1000));
    settings->set("connectTimeout", Settings::get(SettingsKeys::ModbusTcp::modbusTcpConnectTimeout, 5000));
    settings->set("disconnectTimeout", Settings::get(SettingsKeys::ModbusTcp::modbusTcpDisconnectTimeout, 5000));

    if (!settings->isValid())
    {
        delete settings;
        return nullptr;
    }
    return settings;
}

ConnectionSettings InterfaceModbusTcpDialog::wrapSettings(const QString &name, BaseSettings *settings) const
{
    return ConnectionSettings { name, qobject_cast<ModbusTcpSettings *>(settings) };
}
