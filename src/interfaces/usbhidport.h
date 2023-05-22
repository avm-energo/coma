#pragma once

#include "baseinterface.h"
#include "baseport.h"
#include "protocomprivate.h"
#include "settingstypes.h"

#include <QWaitCondition>
#include <gen/error.h>
#include <gen/logclass.h>

struct USBMessage
{
    QString guid;
    quint32 type;
};

struct hid_device_;
using hid_device = hid_device_; ///< opaque hidapi structure
// typedef struct hid_device_ hid_device;

namespace HID
{
constexpr int MaxSegmenthLength = 64;  // максимальная длина одного сегмента (0x40)
constexpr unsigned MainLoopDelay = 20; // 20 ms main loop sleep
constexpr char headerValidator[] = "[a-zA-Z]{3}(?=#)";
} // namespace HID

class UsbHidPort : public BasePort
{
    Q_OBJECT
public:
    explicit UsbHidPort(const UsbHidSettings &dev, QObject *parent = 0);
    ~UsbHidPort();

    UsbHidSettings deviceInfo() const;
    void setDeviceInfo(const UsbHidSettings &deviceInfo);

    void usbEvent(const USBMessage message, quint32 type);
    bool shouldBeStopped() const;
    void shouldBeStopped(bool isShouldBeStopped);

signals:
    void clearQueries();

public slots:
    bool connect() override;
    void disconnect() override;
    bool writeData(const QByteArray &ba) override;
    void poll() override;

private:
    virtual QByteArray readData() override;
    void writeLog(QByteArray ba, Proto::Direction dir = Proto::NoDirection);
    void writeLog(Error::Msg msg, Proto::Direction dir = Proto::NoDirection);
    bool writeDataToPort(QByteArray &ba);
    bool checkCurrentCommand();
    // bool checkQueue();
    void finish();
    void clear();
    void deviceConnected(const UsbHidSettings &st);
    void deviceDisconnected(const UsbHidSettings &st);
    void deviceConnected();
    void deviceDisconnected();

    bool m_waitForReply;
    hid_device *m_hidDevice;
    bool m_shouldBeStopped;
    QMutex _mutex;
    // QList<QByteArray> m_writeQueue;
    QByteArray m_currCommand;
    UsbHidSettings m_deviceInfo;
    int missingCounter = 0;
    int missingCounterMax;
};
