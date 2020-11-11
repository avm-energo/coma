#pragma once

#include "../gen/error.h"
#include "../gen/logclass.h"
#include "defines.h"
#include "hidapi/hidapi.h"

namespace HID
{
constexpr unsigned VID = 0xC251;
constexpr unsigned PID = 0x3505;
// максимальная длина одного сегмента (0x40)
constexpr int MaxSegmenthLength = 64;
// 20 ms main loop sleep
constexpr unsigned MainLoopDelay = 20;

} // namespace HID

class UsbHidPort : public QObject
{
    Q_OBJECT
public:
    explicit UsbHidPort(const DeviceConnectStruct &dev, LogClass *logh, QObject *parent = 0);
    ~UsbHidPort();

    bool setupConnection();
    void closeConnection();
    void writeDataAttempt(const QByteArray &ba);

    DeviceConnectStruct deviceInfo() const;
    void setDeviceInfo(const DeviceConnectStruct &deviceInfo);
    static QList<DeviceConnectStruct> devicesFound(quint16 vid = 0xC251);
    void usbStateChanged(void *message);

signals:
    void dataReceived(QByteArray ba);
    void finished();
    void started();

public slots:
    void poll();

private:
    void writeLog(QByteArray ba, Direction dir = NoDirection);
    void writeLog(Error::Msg msg, Direction dir = NoDirection)
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
    DeviceConnectStruct m_deviceInfo;
};
