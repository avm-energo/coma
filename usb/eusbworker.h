#pragma once

#include "../gen/logclass.h"
#include "defines.h"
#include "hidapi/hidapi.h"

class EUsbWorker : public QObject
{
    Q_OBJECT
public:
    explicit EUsbWorker(DeviceConnectStruct &devinfo, LogClass *logh, bool writelog = false, QObject *parent = 0);
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
    DeviceConnectStruct DeviceInfo;

    int WriteData(QByteArray &ba);
    void CheckWriteQueue();
    void Finish();

private slots:
};
