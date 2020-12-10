#pragma once

#include "../gen/error.h"
#include "../gen/logclass.h"
#include "hidapi/hidapi.h"
#include "protocomprivate.h"
#include "settingstypes.h"

#include <QWaitCondition>
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

    bool isShouldBeStopped() const;
    void shouldBeStopped();

signals:
    void dataReceived(QByteArray ba);
    void finished();
    void started();
    void clearQueries();

public slots:
    void poll();
    void deviceStateChanged(const UsbHidSettings &st, bool isConnected);

private:
    void writeLog(QByteArray ba, Proto::Direction dir = Proto::NoDirection);
    void writeLog(Error::Msg msg, Proto::Direction dir = Proto::NoDirection)
    {
        writeLog(QVariant::fromValue(msg).toByteArray(), dir);
    }

    bool writeData(const QByteArray &ba);
    bool writeData(QByteArray &ba);

    void checkQueue();
    void finish();
    void clear();
    bool m_waitForReply;
    QWaitCondition _waiter;
    hid_device *m_hidDevice;
    bool m_isShouldBeStopped;
    LogClass *log;
    QMutex _mutex;
    QList<QByteArray> m_writeQueue;
    UsbHidSettings m_deviceInfo;
};
