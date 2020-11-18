#pragma once

#include "../gen/error.h"
#include "../gen/logclass.h"
#include "hidapi/hidapi.h"
#include "protocomprivate.h"
#include "settingstypes.h"

namespace HID
{
// максимальная длина одного сегмента (0x40)
constexpr int MaxSegmenthLength = 64;
// 20 ms main loop sleep
constexpr unsigned MainLoopDelay = 20;

} // namespace HID

class UsbHidPort : public QObject
{
    Q_OBJECT
public:
    explicit UsbHidPort(const UsbHidSettings &dev, LogClass *logh, QObject *parent = 0);
    ~UsbHidPort();

    bool setupConnection();
    void closeConnection();
    void writeDataAttempt(const QByteArray &ba);

    UsbHidSettings deviceInfo() const;
    void setDeviceInfo(const UsbHidSettings &deviceInfo);
    void usbStateChanged(void *message);

signals:
    void dataReceived(QByteArray ba);
    void finished();
    void started();

public slots:
    void poll();

private:
    void writeLog(QByteArray ba, Proto::Direction dir = Proto::NoDirection);
    void writeLog(Error::Msg msg, Proto::Direction dir = Proto::NoDirection)
    {
        writeLog(QVariant::fromValue(msg).toByteArray(), dir);
    }

    bool writeData(QByteArray &ba);

    void checkQueue();
    void finish();

    hid_device *m_hidDevice;
    LogClass *log;
    QMutex mutex_;
    QList<QByteArray> m_writeQueue;
    UsbHidSettings m_deviceInfo;
};
