#pragma once

#include "../gen/error.h"
#include "../gen/logclass.h"
#include "defines.h"
#include "datamanager.h"
#include "hidapi/hidapi.h"

class EUsbWorker : public QObject
{
    Q_OBJECT
public:
    explicit EUsbWorker(DeviceConnectStruct dev, LogClass *logh, bool writelog = false, QObject *parent = 0);
    ~EUsbWorker();

    LogClass *log;

    Error::Msg setupConnection();
    int WriteDataAttempt(QByteArray &ba);

    void closeConnection();

    DeviceConnectStruct deviceInfo() const;
    void setDeviceInfo(DeviceConnectStruct deviceInfo);

signals:
    void NewDataReceived(QByteArray ba);
    void finished();
    void started();

public slots:
    void interact();

private:
    hid_device *HidDevice;

    bool WriteUSBLog;

    QMutex mutex_;
    QList<QByteArray> WriteQueue;
    DeviceConnectStruct m_deviceInfo;

    Error::Msg WriteData(QByteArray &ba);
    void CheckWriteQueue();
    void checkQueue();
    void Finish();

private slots:
};
