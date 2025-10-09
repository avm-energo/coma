#pragma once

#include <interfaces/ifaces/baseinterface.h>
#include <interfaces/types/serial_settings.h>

struct hid_device_;
using hid_device = hid_device_; ///< opaque hidapi structure

class UsbHidPort final : public BaseInterface
{
    Q_OBJECT
private:
    hid_device *m_hidDevice;
    UsbHidSettings *m_settings;

public:
    explicit UsbHidPort(UsbHidSettings *settings, QObject *parent = nullptr);

public slots:
    bool connect() override;
    void disconnect() override;

private:
    QByteArray read(bool &status) override;
    bool write(const QByteArray &ba) override;
    bool writeDataToPort(QByteArray &command);
    void hidErrorHandle();
};
