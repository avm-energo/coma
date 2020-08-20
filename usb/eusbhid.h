#pragma once

#include "eabstractprotocomchannel.h"
#include "eusbthread.h"

#include <QByteArray>
#include <QObject>

// Канал связи с модулем

namespace UH
{
constexpr unsigned VID = 0xC251;
constexpr unsigned PID = 0x3505;

}

class EUsbHid : public EAbstractProtocomChannel
{
    Q_OBJECT
public:
    explicit EUsbHid(QObject *parent = nullptr);
    ~EUsbHid();

    EUsbHid(EUsbHid &) = delete;

    void operator=(const EUsbHid &) = delete;

    static EUsbHid *GetInstance(QObject *parent = nullptr);

    //    bool ThreadRunning;

    bool Connect() override;
    QByteArray RawRead(int bytes) override;
    int RawWrite(QByteArray &ba) override;
    void RawClose() override;
    QStringList DevicesFound() const override;

signals:
    void StopUThread();

public slots:

private slots:
    void UThreadFinished();
    void UThreadStarted();

private:
    static EUsbHid *pinstance_;
    static QMutex mutex_;

    EUsbThread *UThread;
    bool UThreadRunning;

    void ClosePort();
};
