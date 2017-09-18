#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QTime>
#include "eusbhid.h"
#include "widgets/messagebox.h"

EUsbHid::EUsbHid(QObject *parent) : EAbstractProtocomChannel(parent)
{
}

EUsbHid::~EUsbHid()
{
}

bool EUsbHid::Connect()
{
    if (!Connected)
        pc.PrbMessage = "Загрузка данных...";
    else
        return true;
    UThread = new EUsbThread;
    UThr = new QThread;
    UThread->moveToThread(UThr);
//    connect(this,SIGNAL(StartUThread(QThread::Priority)),thr,SLOT(start(QThread::Priority)));
    connect(UThr,SIGNAL(started()),UThread,SLOT(Run()));
    connect(UThread,SIGNAL(NewDataReceived(QByteArray)),this,SLOT(ParseIncomeData(QByteArray)));
    connect(UThr,SIGNAL(finished()),UThread,SLOT(deleteLater()));
    connect(UThr,SIGNAL(finished()),UThr,SLOT(deleteLater()));
    connect(this,SIGNAL(StopUThread()),UThread,SLOT(Finish()));
    if (UThread->Set() != NOERROR)
        return false;
    Connected = true;
//    emit StartUThread(QThread::InheritPriority);
    UThr->start();
    return true;
}

QByteArray EUsbHid::RawRead(int bytes)
{
    Q_UNUSED(bytes);
    return QByteArray();
}

qint64 EUsbHid::RawWrite(QByteArray &ba)
{
    return UThread->WriteData(ba);
}

void EUsbHid::RawClose()
{
    emit StopUThread();
}

EUsbThread::EUsbThread(QObject *parent) : QObject(parent)
{
    log = new Log;
    log->Init("usb.log");
    log->WriteRaw("=== Log started ===");
    AboutToFinish = false;
//    isRunning = false;
}

EUsbThread::~EUsbThread()
{
    delete log;
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
    char *data;
    data = new char(UH_MAXSEGMENTLENGTH+1); // +1 to ID
    while (!AboutToFinish)
    {
        // check if there's any data in input buffer
        int bytes = hid_read(HidDevice, reinterpret_cast<unsigned char *>(data), UH_MAXSEGMENTLENGTH+1);
        if (bytes < 0)
        {
            if (pc.WriteUSBLog)
                log->WriteRaw("Unable to hid_read()");
            continue;
        }
        if (bytes > 0)
        {
            QByteArray ba(data, bytes);
            emit NewDataReceived(ba);
        }
        QTime tme;
        tme.start();
        while (tme.elapsed() < UH_MAINLOOP_DELAY)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    hid_close(HidDevice);
}

qint64 EUsbThread::WriteData(QByteArray &ba)
{
    if (ba.size() > UH_MAXSEGMENTLENGTH)
    {
        if (pc.WriteUSBLog)
        {
            log->WriteRaw("WRONG SEGMENT LENGTH!\n");
            log->WriteRaw(ba.toHex());
            log->WriteRaw("WRONG SEGMENT LENGTH!\n");
        }
        ERMSG("Длина сегмента больше "+QString::number(UH_MAXSEGMENTLENGTH)+" байт");
        return GENERALERROR;
    }
    if (ba.size() < UH_MAXSEGMENTLENGTH)
        ba.append(UH_MAXSEGMENTLENGTH - ba.size(), static_cast<char>(0x00));
    ba.prepend(static_cast<char>(0x00)); // inserting ID field
    if (pc.WriteUSBLog)
    {
        log->WriteRaw("->");
        log->WriteRaw(ba.toHex());
        log->WriteRaw("\n");
    }
    return hid_write(HidDevice, reinterpret_cast<unsigned char *>(ba.data()), ba.size());
}

void EUsbThread::Finish()
{
    AboutToFinish = true;
}
