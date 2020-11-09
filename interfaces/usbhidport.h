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
    explicit UsbHidPort(const DeviceConnectStruct &dev, LogClass *logh, bool writelog = false, QObject *parent = 0);
    ~UsbHidPort();

    LogClass *log;

    Error::Msg setupConnection();
    void WriteDataAttempt(QByteArray &ba);

    void closeConnection();

    void initiateReceive(QByteArray ba);
    void initiateSend(const CommandStruct &cmdStr);
    DeviceConnectStruct deviceInfo() const;
    void setDeviceInfo(DeviceConnectStruct deviceInfo);

signals:
    void NewDataReceived(QByteArray ba);
    void finished();
    void started();

public slots:
    void poll();

private:
    hid_device *HidDevice;
    void writeLog(bool in_out, QByteArray ba);
    void writeLog(bool in_out, Error::Msg msg)
    {
        writeLog(in_out, QVariant::fromValue(msg).toByteArray());
    }

    QMutex mutex_;
    QList<QByteArray> WriteQueue;
    DeviceConnectStruct m_deviceInfo;

    QPair<quint64, QByteArray> m_buffer;
    void handle(const CN::Commands cmd);

    bool WriteData(QByteArray &ba);

    CommandStruct m_currentCommand;
    void CheckWriteQueue();
    void checkQueue();
    void Finish();
};
