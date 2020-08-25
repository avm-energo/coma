#include "eusbhid.h"

#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "eusbworker.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QThread>

EUsbHid *EUsbHid::pinstance_ { nullptr };
QMutex EUsbHid::mutex_;

EUsbHid::EUsbHid(QObject *parent) : EAbstractProtocomChannel(parent)
{
}

QString EUsbHid::deviceName() const
{
    return m_deviceName;
}

void EUsbHid::setDeviceName(const QString &deviceName)
{
    m_deviceName = deviceName;
}

EUsbHid::~EUsbHid()
{
    m_workerThread.quit();
    m_workerThread.wait();
    pinstance_ = nullptr;
}
/**
 * The first time we call GetInstance we will lock the storage location
 *      and then we make sure again that the variable is null and then we
 *      set the value.
 */
EUsbHid *EUsbHid::GetInstance(QObject *parent)
{
    if (pinstance_ == nullptr)
    {
        QMutexLocker locker(&mutex_);
        if (pinstance_ == nullptr)
        {
            pinstance_ = new EUsbHid(parent);
        }
    }
    return pinstance_;
}

bool EUsbHid::Connect()
{
    QMutexLocker locker(&mutex_);
    if (isConnected())
        Disconnect();
    m_usbWorker = new EUsbWorker(UsbPort, CnLog, IsWriteUSBLog());

    m_usbWorker->moveToThread(&m_workerThread);
    connect(&m_workerThread, &QThread::started, m_usbWorker, &EUsbWorker::interact);

    connect(m_usbWorker, &EUsbWorker::Finished, &m_workerThread, &QThread::quit);

    connect(&m_workerThread, &QThread::finished, &m_workerThread, &QThread::deleteLater);
    connect(m_usbWorker, &EUsbWorker::Finished, m_usbWorker, &EUsbWorker::deleteLater);

    connect(m_usbWorker, &EUsbWorker::NewDataReceived, this, &EAbstractProtocomChannel::ParseIncomeData);

    if (m_usbWorker->setupConnection() == 0)
    {
        setConnected(true);
        m_workerThread.start();
    }
    else
        return false;
    return true;
}

void EUsbHid::Disconnect()
{
    RawClose();
    CnLog->WriteRaw("Disconnected!\n");
    delete EUsbHid::GetInstance();
}

QByteArray EUsbHid::RawRead(int bytes)
{
    Q_UNUSED(bytes);
    return QByteArray();
}

int EUsbHid::RawWrite(QByteArray &ba)
{
    if (!isConnected())
        return GENERALERROR;
    return m_usbWorker->WriteDataAttempt(ba);
}

void EUsbHid::RawClose()
{
    if (isConnected())
    {
        setConnected(false);
        m_usbWorker->Stop();
    }
}

QStringList EUsbHid::DevicesFound() const
{
    struct hid_device_info *devs, *cur_dev;

    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;
    int venid, prodid;
    QString sn;
    QStringList sl;
    while (cur_dev)
    {
        if (cur_dev->vendor_id == 0xC251)
        {
            venid = cur_dev->vendor_id;
            prodid = cur_dev->product_id;
            sn = QString::fromWCharArray(cur_dev->serial_number);
            QString tmps
                = "VEN_" + QString::number(venid, 16) + "_ & DEV_" + QString::number(prodid, 16) + "_ & SN_" + sn;
            sl << tmps;
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
    return sl;
}

QThread *EUsbHid::workerThread()
{
    return &m_workerThread;
}

EUsbWorker *EUsbHid::usbWorker() const
{
    return m_usbWorker;
}
