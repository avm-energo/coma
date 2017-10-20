#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QTime>
#include "eusbhid.h"
#include "widgets/messagebox.h"

EUsbHid::EUsbHid(QObject *parent) : EAbstractProtocomChannel(parent)
{
    ThreadRunning = false;
}

EUsbHid::~EUsbHid()
{
}

bool EUsbHid::Connect()
{
    if (Connected)
        Disconnect();
    UThread = new EUsbThread(log);
    UThr = new QThread;
    UThread->moveToThread(UThr);
    connect(UThr,SIGNAL(started()),UThread,SLOT(Run()));
    connect(UThread,SIGNAL(NewDataReceived(QByteArray)),this,SLOT(ParseIncomeData(QByteArray)));
    connect(UThr,SIGNAL(finished()),UThread,SLOT(deleteLater()));
    connect(UThr,SIGNAL(finished()),UThr,SLOT(deleteLater()));
    connect(UThread,SIGNAL(Finished()),this,SLOT(ThreadFinished()));
    connect(this,SIGNAL(StopUThread()),UThread,SLOT(Finish()));
    if (UThread->Set() != NOERROR)
        return false;
    Connected = true;
    UThr->start();
    ThreadRunning = true;
    return true;
}

QByteArray EUsbHid::RawRead(int bytes)
{
    Q_UNUSED(bytes);
    return QByteArray();
}

qint64 EUsbHid::RawWrite(QByteArray &ba)
{
    if (!ThreadRunning)
        return GENERALERROR;
    qint64 res = UThread->WriteData(ba);
    if (res < 0)
        return GENERALERROR;
    return res;
}

void EUsbHid::RawClose()
{
    if (ThreadRunning)
    {
        emit StopUThread();
        while (ThreadRunning)
        {
            QTime tme;
            tme.start();
            while (tme.elapsed() < UH_MAINLOOP_DELAY)
                QCoreApplication::processEvents(QEventLoop::AllEvents);
        }
    }
    Connected = false;
}

void EUsbHid::ThreadFinished()
{
    ThreadRunning = false;
}

EUsbThread::EUsbThread(Log *logh, QObject *parent) : QObject(parent)
{
    log = logh;
    AboutToFinish = false;
//    isRunning = false;
}

EUsbThread::~EUsbThread()
{
}

int EUsbThread::Set()
{
    HidDevice = hid_open(UH_VID, UH_PID, NULL);
    if (!HidDevice)
        return GENERALERROR;
    hid_set_nonblocking(HidDevice, 1);
    return NOERROR;
}

void EUsbThread::Run()
{
    try
    {
        unsigned char data[UH_MAXSEGMENTLENGTH+1]; // +1 to ID
        while (!AboutToFinish)
        {
            // check if there's any data in input buffer
            int bytes = hid_read(HidDevice, data, UH_MAXSEGMENTLENGTH+1);
            if (bytes < 0)
            {
                if (pc.WriteUSBLog)
                    log->WriteRaw("UsbThread: Unable to hid_read()");
                AboutToFinish = true;
                emit Finished();
                return;
            }
            if (bytes > 0)
            {
                QByteArray ba(reinterpret_cast<char*>(data), bytes);
                emit NewDataReceived(ba);
            }
            QTime tme;
            tme.start();
            while (tme.elapsed() < UH_MAINLOOP_DELAY)
                QCoreApplication::processEvents(QEventLoop::AllEvents);
        }
        hid_close(HidDevice);
        emit Finished();
    }
    catch(...)
    {
        emit Finished();
    }
}

qint64 EUsbThread::WriteData(QByteArray &ba)
{
    if (ba.size() > UH_MAXSEGMENTLENGTH)
    {
        if (pc.WriteUSBLog)
            log->WriteRaw("UsbThread: WRONG SEGMENT LENGTH!\n");
        ERMSG("Длина сегмента больше "+QString::number(UH_MAXSEGMENTLENGTH)+" байт");
        return GENERALERROR;
    }
    if (ba.size() < UH_MAXSEGMENTLENGTH)
        ba.append(UH_MAXSEGMENTLENGTH - ba.size(), static_cast<char>(0x00));
    ba.prepend(static_cast<char>(0x00)); // inserting ID field
    if (pc.WriteUSBLog)
    {
        QByteArray tmpba = "UsbThread: ->" + ba.toHex() + "\n";
        log->WriteRaw(tmpba);
    }
    return hid_write(HidDevice, reinterpret_cast<unsigned char *>(ba.data()), ba.size());
}

void EUsbThread::Finish()
{
    AboutToFinish = true;
}
