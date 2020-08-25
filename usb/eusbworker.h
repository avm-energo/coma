#pragma once

#include "eabstractprotocomchannel.h"
#include "hidapi/hidapi.h"

#include <QByteArray>
#include <QObject>

namespace UH
{
// максимальная длина одного сегмента (0x40)
constexpr unsigned MaxSegmenthLength = 64;
// 20 ms main loop sleep
constexpr unsigned MainLoopDelay = 20;
}

class EUsbWorker : public QObject
{
    Q_OBJECT
public:
    explicit EUsbWorker(EAbstractProtocomChannel::DeviceConnectStruct &devinfo, LogClass *logh, bool writelog = false,
        QObject *parent = 0);
    ~EUsbWorker();

    LogClass *log;

    int setupConnection();
    int WriteDataAttempt(QByteArray &ba);

signals:
    void NewDataReceived(QByteArray ba);
    void Finished();
    void started();

public slots:
    void interact();
    void Stop();

private:
    hid_device *HidDevice;
    bool AboutToFinish;
    bool WriteUSBLog;

    QMutex mutex_;
    QList<QByteArray> WriteQueue;
    EAbstractProtocomChannel::DeviceConnectStruct DeviceInfo;

    int WriteData(QByteArray &ba);
    void CheckWriteQueue();
    void Finish();

private slots:
};
