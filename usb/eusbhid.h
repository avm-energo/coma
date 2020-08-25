#pragma once

#include "eabstractprotocomchannel.h"
#include "eusbworker.h"

#include <QByteArray>
#include <QThread>
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
    ~EUsbHid();

    static EUsbHid *GetInstance(QObject *parent = nullptr);

    bool Connect() override;
    void Disconnect() override;

    QByteArray RawRead(int bytes) override;
    int RawWrite(QByteArray &ba) override;
    void RawClose() override;
    QStringList DevicesFound() const override;

    EUsbWorker *usbWorker() const;

    QThread *workerThread();

    QString deviceName() const;
    void setDeviceName(const QString &deviceName);

protected:
    explicit EUsbHid(QObject *parent = nullptr);
    EUsbHid(EUsbHid &) = delete;

    void operator=(const EUsbHid &) = delete;

private:
    static EUsbHid *pinstance_;
    static QMutex mutex_;

    QString m_deviceName;
    EUsbWorker *m_usbWorker;
    QThread m_workerThread;

    void ClosePort();
};
