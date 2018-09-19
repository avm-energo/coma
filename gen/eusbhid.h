#ifndef EUSBHID_H
#define EUSBHID_H

#include <QObject>
#include <QByteArray>
#include <QTimer>
#include <QLabel>

#include "log.h"
#include "eabstractprotocomchannel.h"
#include "hidapi/hidapi.h"

// Канал связи с модулем

#define UH_MAXSEGMENTLENGTH 64 // максимальная длина одного сегмента (0x40)
#define UH_MAINLOOP_DELAY   50 // 100 ms main loop sleep

#define UH_VID  0xC251
#define UH_PID  0x3505

class EUsbThread : public QObject
{
    Q_OBJECT
public:
    explicit EUsbThread(Log *logh, bool writelog = false, QObject *parent = 0);
    ~EUsbThread();

    Log *log;

    int Set(EAbstractProtocomChannel::DeviceConnectStruct &devinfo);

signals:
    void NewDataReceived(QByteArray ba);
    void Finished();

public slots:
    void Run();
    int WriteData(QByteArray &ba);
    void Finish();

private:
    hid_device *HidDevice;
    bool AboutToFinish, Device;
    bool RunWait;
    bool WriteUSBLog;

private slots:
};

class EUsbHid : public EAbstractProtocomChannel
{
    Q_OBJECT
public:
    explicit EUsbHid(QObject *parent = nullptr);
    ~EUsbHid();

    bool ThreadRunning;

    bool Connect();
    QByteArray RawRead(int bytes);
    int RawWrite(QByteArray &ba);
    void RawClose();
    QStringList DevicesFound();

signals:
    void StopUThread();

public slots:

private slots:

private:
    EUsbThread *UThread;

    void ClosePort();
};

#endif // EUSBHID_H
