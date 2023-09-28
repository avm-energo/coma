#pragma once

#include <QWaitCondition>
#include <gen/error.h>
#include <interfaces/ports/baseport.h>
#include <interfaces/types/protocom_types.h>
#include <interfaces/types/settingstypes.h>

struct USBMessage
{
    QString guid;
    quint32 type;
};

struct hid_device_;
using hid_device = hid_device_; ///< opaque hidapi structure

namespace HID
{
constexpr int MaxSegmenthLength = 64;  // максимальная длина одного сегмента (0x40)
constexpr unsigned MainLoopDelay = 20; // 20 ms main loop sleep
constexpr char headerValidator[] = "[a-zA-Z]{3}(?=#)";
} // namespace HID

class UsbHidPort final : public BasePort
{
    Q_OBJECT
public:
    explicit UsbHidPort(const UsbHidSettings &dev, QObject *parent = 0);
    ~UsbHidPort() = default;

    UsbHidSettings deviceInfo() const;
    void setDeviceInfo(const UsbHidSettings &deviceInfo);

    void usbEvent(const USBMessage message, quint32 type);
    bool shouldBeStopped() const;
    void shouldBeStopped(bool isShouldBeStopped);

public slots:
    bool connect() override;
    void disconnect() override;
    void usbEvent(const QString &guid, quint32 msgType);

private:
    virtual QByteArray read(bool *status = nullptr) override;
    virtual bool write(const QByteArray &ba) override;

    bool writeDataToPort(QByteArray &ba);
    void clear();
    void deviceConnected(const UsbHidSettings &st);
    void deviceDisconnected(const UsbHidSettings &st);
    void deviceConnected();
    void deviceDisconnected();

    bool m_waitForReply;
    hid_device *m_hidDevice;
    bool m_shouldBeStopped;
    // QMutex _mutex;
    QByteArray m_currCommand;
    UsbHidSettings m_deviceInfo;
    int missingCounter = 0;
    int missingCounterMax;
};
