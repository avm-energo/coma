#include "eusbhid.h"

#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "eusbthread.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QThread>

EUsbHid *EUsbHid::pinstance_ { nullptr };
QMutex EUsbHid::mutex_;

EUsbHid::EUsbHid(QObject *parent) : EAbstractProtocomChannel(parent)
{
    UThreadRunning = false;
}

EUsbHid::~EUsbHid()
{
}
/**
 * The first time we call GetInstance we will lock the storage location
 *      and then we make sure again that the variable is null and then we
 *      set the value. RU:
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
    if (Connected)
        Disconnect();
    QThread *thr = new QThread;
    UThread = new EUsbThread(UsbPort, CnLog, IsWriteUSBLog());
    UThread->moveToThread(thr);
    connect(thr, &QThread::started, UThread, &EUsbThread::Run);
    connect(UThread, &EUsbThread::Started, this, &EUsbHid::UThreadStarted);
    connect(UThread, &EUsbThread::Finished, thr, &QThread::quit);
    connect(thr, &QThread::finished, thr, &QObject::deleteLater);
    connect(UThread, &EUsbThread::Finished, UThread, &QObject::deleteLater);
    connect(UThread, &EUsbThread::Finished, this, &EUsbHid::UThreadFinished);
    connect(UThread, &EUsbThread::NewDataReceived, this, &EAbstractProtocomChannel::ParseIncomeData);
    connect(this, &EUsbHid::StopUThread, UThread, &EUsbThread::Stop);
    thr->start();
    while (!Connected)
        QCoreApplication::processEvents();
    return true;
}

QByteArray EUsbHid::RawRead(int bytes)
{
    Q_UNUSED(bytes);
    return QByteArray();
}

int EUsbHid::RawWrite(QByteArray &ba)
{
    // int res;
    if (!Connected)
        return GENERALERROR;
    //    while ((res = UThread->WriteDataAttempt(ba)) == RESEMPTY) // while out queue is busy
    //        StdFunc::Wait(UH_MAINLOOP_DELAY);
    return UThread->WriteDataAttempt(ba);
    //    if (res < 0)
    //    {
    //        ERMSG("error writedata");
    //        return GENERALERROR;
    //    }
    //    return NOERROR;
}

void EUsbHid::RawClose()
{
    if (Connected)
        emit StopUThread();
    while (UThreadRunning)
        QCoreApplication::processEvents();
    Connected = false;
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

void EUsbHid::UThreadFinished()
{
    UThreadRunning = false;
}

void EUsbHid::UThreadStarted()
{
    UThreadRunning = true;
    Connected = true;
}
