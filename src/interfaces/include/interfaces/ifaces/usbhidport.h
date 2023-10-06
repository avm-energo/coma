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
    void reconnect() override;

private:
    virtual QByteArray read(bool *status = nullptr) override;
    virtual bool write(const QByteArray &ba) override;
    void hidErrorHandle();
    bool writeDataToPort(QByteArray &command);
    void clear();

    bool m_waitForReply;
    hid_device *m_hidDevice;
    QByteArray m_currCommand;
    UsbHidSettings m_deviceInfo;
    int missingCounter = 0;
    int missingCounterMax;
};
