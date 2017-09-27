#ifndef EUSBHID_H
#define EUSBHID_H

#include <QObject>
#include <QByteArray>
#include <QTimer>
#include <QThread>
#include <QLabel>
#include <QMutex>

#include "publicclass.h"
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
    explicit EUsbThread(Log *logh, QObject *parent = 0);
    ~EUsbThread();

    Log *log;

    int Set();

signals:
    void NewDataReceived(QByteArray ba);

public slots:
    void Run();
    qint64 WriteData(QByteArray &ba);
    void Finish();

private:
    hid_device *HidDevice;
    bool AboutToFinish;
};

class EUsbHid : public EAbstractProtocomChannel
{
    Q_OBJECT
public:
    explicit EUsbHid(QObject *parent = 0);
    ~EUsbHid();

    bool FirstRun;

    bool Connect();
    QByteArray RawRead(int bytes);
    qint64 RawWrite(QByteArray &ba);
    void RawClose();

signals:
    void Disconnected();
    void StartUThread(QThread::Priority);
    void StopUThread();

public slots:

private slots:

private:
    EUsbThread *UThread;
    QThread *UThr;

    void ClosePort();
};

#endif // EUSBHID_H
