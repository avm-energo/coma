#pragma once

#include <QWaitCondition>
#include <gen/error.h>
#include <interfaces/ifaces/baseinterface.h>
#include <interfaces/types/protocom_types.h>
#include <interfaces/types/settingstypes.h>

struct hid_device_;
using hid_device = hid_device_; ///< opaque hidapi structure

class UsbHidPort final : public BaseInterface
{
    Q_OBJECT
public:
    explicit UsbHidPort(const UsbHidSettings &dev, QObject *parent = 0);
    ~UsbHidPort() = default;
    const UsbHidSettings &deviceInfo() const;

public slots:
    bool connect() override;
    void disconnect() override;

private:
    QByteArray read(bool *status = nullptr) override;
    bool write(const QByteArray &ba) override;
    bool writeDataToPort(QByteArray &command);
    bool tryToReconnect() override;
    void hidErrorHandle();

    hid_device *m_hidDevice;
    UsbHidSettings m_deviceInfo;
};
