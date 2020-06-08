#include <QCoreApplication>
#include <QElapsedTimer>
#include <QThread>
#include "eusbhid.h"
#include "error.h"
#include "stdfunc.h"

EUsbHid::EUsbHid(QObject *parent) : EAbstractProtocomChannel(parent)
{
    UThreadRunning = false;
}

EUsbHid::~EUsbHid()
{
}

bool EUsbHid::Connect()
{
    if (Connected)
        Disconnect();
    QThread *thr = new QThread;
    UThread = new EUsbThread(UsbPort, CnLog, IsWriteUSBLog());
    UThread->moveToThread(thr);
    connect(thr,SIGNAL(started()),UThread,SLOT(Run()));
    connect(UThread,SIGNAL(Started()),this,SLOT(UThreadStarted()));
    connect(UThread,SIGNAL(Finished()),thr,SLOT(quit()));
    connect(thr,SIGNAL(finished()),thr,SLOT(deleteLater()));
    connect(UThread,SIGNAL(Finished()),UThread,SLOT(deleteLater()));
    connect(UThread,SIGNAL(Finished()),this,SLOT(UThreadFinished()));
    connect(UThread,SIGNAL(NewDataReceived(QByteArray)),this,SLOT(ParseIncomeData(QByteArray)));
    connect(this,SIGNAL(StopUThread()),UThread,SLOT(Stop()));
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
    int res;
    if (!Connected)
        return GENERALERROR;
    while ((res = UThread->WriteDataAttempt(ba)) == RESEMPTY) // while out queue is busy
        StdFunc::Wait(UH_MAINLOOP_DELAY);
    if (res < 0)
    {
        ERMSG("error writedata");
        return GENERALERROR;
    }
    return res;
}

void EUsbHid::RawClose()
{
    if (Connected)
        emit StopUThread();
    while (UThreadRunning)
        QCoreApplication::processEvents();
    Connected = false;
}

QStringList EUsbHid::DevicesFound()
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
            QString tmps = "VEN_" + QString::number(venid, 16) + "_ & DEV_" + QString::number(prodid, 16) + \
                    "_ & SN_" + sn;
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

EUsbThread::EUsbThread(EAbstractProtocomChannel::DeviceConnectStruct &devinfo, \
                       LogClass *logh, bool writelog, QObject *parent) : QObject(parent)
{
    log = logh;
    AboutToFinish = false;
    HidDevice = nullptr;
    WriteUSBLog = writelog;
    Busy = false;
    DeviceInfo = devinfo;
}

EUsbThread::~EUsbThread()
{
}

int EUsbThread::Set()
{
    if ((DeviceInfo.product_id == 0) || (DeviceInfo.vendor_id == 0))
    {
        ERMSG("DeviceInfo is null");
        return GENERALERROR;
    }
    HidDevice = hid_open(DeviceInfo.vendor_id, DeviceInfo.product_id, DeviceInfo.serial);
    if (!HidDevice)
    {
        ERMSG("Error opening HID device");
        return GENERALERROR;
    }
    hid_set_nonblocking(HidDevice, 1);
    INFOMSG("HID opened successfully");
    return NOERROR;
}

void EUsbThread::Run()
{
    int bytes;
    unsigned char data[UH_MAXSEGMENTLENGTH+1]; // +1 to ID
    try
    {
        if (Set() != NOERROR)
        {
            Finish();
            return;
        }
        emit Started();
        while(!AboutToFinish)
        {
            // check if there's any data in input buffer
            if (HidDevice != nullptr)
            {
                bytes = hid_read(HidDevice, data, UH_MAXSEGMENTLENGTH+1);
                if (bytes < 0)
                {
                    if (WriteUSBLog)
                        log->WriteRaw("UsbThread: Unable to hid_read()");
                    Finish();
                    ERMSG("HID USB thread finishing");
                    return;
                }
                if (bytes > 0)
                {
                    QByteArray ba(reinterpret_cast<char*>(data), bytes);
                    emit NewDataReceived(ba);
                    Busy = false;
                }
                CheckWriteQueue(); // write data to port if there's something delayed in out queue
                QElapsedTimer tmr;
                tmr.start();
                while (tmr.elapsed() < UH_MAINLOOP_DELAY)
                    QCoreApplication::processEvents(QEventLoop::AllEvents);
            }
        }
        Finish();
    }
    catch(...)
    {
        emit Finished();
    }
}

void EUsbThread::Stop()
{
    AboutToFinish = true;
}

int EUsbThread::WriteDataAttempt(QByteArray &ba)
{
    if (Busy)
    {
//        WriteQueue.append(ba);
        INFOMSG("UThread: busy");
//        while (Busy)
//            QCoreApplication::processEvents();
    }
    return WriteData(ba);
}

void EUsbThread::Finish()
{
    if (HidDevice != nullptr)
    {
        hid_close(HidDevice);
        HidDevice = nullptr;
    }
    INFOMSG("UThread finished");
    emit Finished();
}

int EUsbThread::WriteData(QByteArray &ba)
{
    Busy = true;
    if (HidDevice != nullptr)
    {
        if (ba.size() > UH_MAXSEGMENTLENGTH)
        {
            if (WriteUSBLog)
                log->WriteRaw("UsbThread: WRONG SEGMENT LENGTH!\n");
            ERMSG("Длина сегмента больше "+QString::number(UH_MAXSEGMENTLENGTH)+" байт");
            return GENERALERROR;
        }
        if (ba.size() < UH_MAXSEGMENTLENGTH)
            ba.append(UH_MAXSEGMENTLENGTH - ba.size(), static_cast<char>(0x00));
        ba.prepend(static_cast<char>(0x00)); // inserting ID field
        if (WriteUSBLog)
        {
            QByteArray tmpba = "UsbThread: ->" + ba.toHex() + "\n";
            log->WriteRaw(tmpba);
        }
        size_t tmpt = static_cast<size_t>(ba.size());
        int res = hid_write(HidDevice, reinterpret_cast<unsigned char *>(ba.data()), tmpt);
        return res;
    }
    log->WriteRaw("UsbThread: null hid device");
    return GENERALERROR;
}

void EUsbThread::CheckWriteQueue()
{
    if (!Busy && (!WriteQueue.isEmpty()))
    {
        QByteArray ba = WriteQueue.takeFirst();
        WriteData(ba);
    }
}
