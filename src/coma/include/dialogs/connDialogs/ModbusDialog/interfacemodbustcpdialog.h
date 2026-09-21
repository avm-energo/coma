#pragma once

#include <dialogs/connDialogs/IEC104Dialog/interfaceethernetdialog.h>

class InterfaceModbusTcpDialog final : public InterfaceEthernetDialog
{
    Q_OBJECT
public:
    explicit InterfaceModbusTcpDialog(QWidget *parent = nullptr);

    quint16 defaultPort() const override;
    quint16 defaultAddress() const override;

protected:
    QString settingsGroup() const override;
    QString addressLabel() const override;
    QString addressSettingsKey() const override;
    QString zeroAddressError() const override;
    BaseSettings *buildSettings(const QString &ip, quint16 port, quint16 address) const override;
    ConnectionSettings wrapSettings(const QString &name, BaseSettings *settings) const override;
};
