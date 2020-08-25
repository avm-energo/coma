#ifndef EUSBHID_H
#define EUSBHID_H

#include "../gen/logclass.h"
#include "eabstractprotocomchannel.h"
#include "hidapi/hidapi.h"

#include <QByteArray>
#include <QLabel>
#include <QObject>
#include <QTimer>

// Канал связи с модулем

#define UH_MAXSEGMENTLENGTH 64 // максимальная длина одного сегмента (0x40)
#define UH_MAINLOOP_DELAY 20   // 20 ms main loop sleep

#define UH_VID 0xC251
#define UH_PID 0x3505

class EUsbThread : public QObject
{
    Q_OBJECT
public:
    explicit EUsbThread(EAbstractProtocomChannel::DeviceConnectStruct &devinfo, LogClass *logh, bool writelog = false,
        QObject *parent = 0);
    ~EUsbThread();

    LogClass *log;

    int Set();
    int WriteDataAttempt(QByteArray &ba);

signals:
    void NewDataReceived(QByteArray ba);
    void Finished();
    void Started();

public slots:
    void Run();
    void Stop();

private:
    hid_device *HidDevice;
    bool AboutToFinish, Busy;
    bool RunWait;
    bool WriteUSBLog;
    QList<QByteArray> WriteQueue;
    EAbstractProtocomChannel::DeviceConnectStruct DeviceInfo;

    int WriteData(QByteArray &ba);
    void CheckWriteQueue();
    void Finish();

private slots:
};

class EUsbHid : public EAbstractProtocomChannel
{
    Q_OBJECT
public:
    explicit EUsbHid(QObject *parent = nullptr);
    ~EUsbHid();

    //    bool ThreadRunning;

    bool Connect();
    QByteArray RawRead(int bytes);
    int RawWrite(QByteArray &ba);
    void RawClose();
    QStringList DevicesFound();

signals:
    void StopUThread();

public slots:

private slots:
    void UThreadFinished();
    void UThreadStarted();

private:
    EUsbThread *UThread;
    bool UThreadRunning;

    void ClosePort();
};

#endif // EUSBHID_H
