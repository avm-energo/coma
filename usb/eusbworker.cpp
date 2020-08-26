#include "eusbworker.h"

#include "../gen/error.h"
#include "../gen/stdfunc.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QThread>

#define __PRETTY_FUNCTION__ __FUNCSIG__
EUsbWorker::EUsbWorker(DeviceConnectStruct &devinfo, LogClass *logh, bool writelog, QObject *parent)
    : DeviceInfo(devinfo), log(logh), WriteUSBLog(writelog), QObject(parent)
{
    AboutToFinish = false;
    HidDevice = nullptr;
}

EUsbWorker::~EUsbWorker()
{
}

int EUsbWorker::setupConnection()
{
    if ((DeviceInfo.product_id == 0) || (DeviceInfo.vendor_id == 0))
    {
        ERMSG("DeviceInfo is null");
        Finish();
        return GENERALERROR;
    }
    HidDevice = hid_open(DeviceInfo.vendor_id, DeviceInfo.product_id, DeviceInfo.serial);
    if (!HidDevice)
    {
        ERMSG("Error opening HID device");
        Finish();
        return GENERALERROR;
    }
    hid_set_nonblocking(HidDevice, 1);
    INFOMSG("HID opened successfully");
    return NOERROR;
}

void EUsbWorker::interact()
{
    int bytes;
    unsigned char data[UH::MaxSegmenthLength + 1]; // +1 to ID

    while (!AboutToFinish)
    {
        // check if there's any data in input buffer
        if (HidDevice != nullptr)
        {
            bytes = hid_read(HidDevice, data, UH::MaxSegmenthLength + 1);
            // Write
            if (bytes < 0)
            {
                if (WriteUSBLog)
                    log->WriteRaw("UsbThread: Unable to hid_read()");
                continue;
            }
            // Read
            if (bytes > 0)
            {
                QByteArray ba(reinterpret_cast<char *>(data), bytes);
                emit NewDataReceived(ba);
            }
            CheckWriteQueue(); // write data to port if there's something delayed in out queue
        }
    }
    Finish();
}

void EUsbWorker::Stop()
{
    AboutToFinish = true;
}

int EUsbWorker::WriteDataAttempt(QByteArray &ba)
{
    WriteQueue.append(ba);
    return ba.size();
}

void EUsbWorker::Finish()
{
    if (HidDevice != nullptr)
    {
        mutex_.lock();
        WriteQueue.clear();
        mutex_.unlock();
        hid_close(HidDevice);

        HidDevice = nullptr;
    }
    INFOMSG("UThread finished");
    emit Finished();
}

int EUsbWorker::WriteData(QByteArray &ba)
{
    if (HidDevice != nullptr)
    {
        if (ba.size() > UH::MaxSegmenthLength)
        {
            if (WriteUSBLog)
                log->WriteRaw("UsbThread: WRONG SEGMENT LENGTH!\n");
            ERMSG("Длина сегмента больше " + QString::number(UH::MaxSegmenthLength) + " байт");
            return GENERALERROR;
        }
        if (ba.size() < UH::MaxSegmenthLength)
            ba.append(UH::MaxSegmenthLength - ba.size(), static_cast<char>(0x00));
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
    if (log != nullptr)
        log->WriteRaw("UsbThread: null hid device");
    return GENERALERROR;
}

void EUsbWorker::CheckWriteQueue()
{
    QMutexLocker locker(&mutex_);
    if (!WriteQueue.isEmpty())
    {
        QByteArray ba = WriteQueue.takeFirst();
        WriteData(ba);
    }
}
