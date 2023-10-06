#pragma once

#include <QWaitCondition>
#include <gen/error.h>
#include <interfaces/ifaces/baseinterface.h>
#include <interfaces/types/protocom_types.h>
#include <interfaces/types/settingstypes.h>

struct hid_device_;
using hid_device = hid_device_; ///< opaque hidapi structure

namespace HID
{
constexpr int MaxSegmenthLength = 64;  // максимальная длина одного сегмента (0x40)
constexpr unsigned MainLoopDelay = 20; // 20 ms main loop sleep
constexpr char headerValidator[] = "[a-zA-Z]{3}(?=#)";
} // namespace HID

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
