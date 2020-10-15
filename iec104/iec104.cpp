#include "iec104.h"

#include "../gen/error.h"
#include "../gen/timefunc.h"
#include "ethernet.h"

#include <QCoreApplication>
#include <QDialog>
#include <QThread>
#include <QVBoxLayout>

IEC104::IEC104(S2ConfigType *s2, QObject *parent) : QObject(parent)
{
    EthThreadWorking = false;
    ParseThreadWorking = false;
    AboutToFinish = false;
    S2Config = s2;
    Log = new LogClass;
    Log->Init("iec104.log");
    Log->info("=== Log started ===");
}

IEC104::~IEC104() { }

bool IEC104::Working() { return (EthThreadWorking | ParseThreadWorking); }

void IEC104::Connect(Settings &st)
{
    INFOMSG("IEC104: connect");
    EthThreadWorking = false;
    ParseThreadWorking = false;
    AboutToFinish = false;
    QThread *thr = new QThread;
    Ethernet *eth = new Ethernet;
    eth->moveToThread(thr);
    eth->IP = st.ip;
    connect(eth, &Ethernet::Finished, thr, &QThread::quit);
    connect(eth, &Ethernet::Finished, eth, &QObject::deleteLater);
    connect(thr, &QThread::started, eth, &Ethernet::Run);
    connect(thr, &QThread::finished, thr, &QObject::deleteLater);
    connect(this, &IEC104::StopAll, eth, &Ethernet::Stop);
    connect(eth, &Ethernet::Connected, this, &IEC104::EthThreadStarted);
    connect(eth, &Ethernet::Disconnected, this, &IEC104::EthThreadFinished);

    Parse = new IEC104Thread(Log, InputQueue, S2Config);
    QThread *thr2 = new QThread;
    Parse->moveToThread(thr2);
    connect(this, &IEC104::StopAll, Parse, &IEC104Thread::Stop);
    connect(Parse, &IEC104Thread::Finished, Parse, &QObject::deleteLater);
    connect(Parse, &IEC104Thread::Finished, thr2, &QThread::quit);
    connect(Parse, &IEC104Thread::Finished, this, &IEC104::ParseThreadFinished);
    connect(thr2, &QThread::finished, thr2, &QObject::deleteLater);
    connect(thr2, &QThread::started, Parse, &IEC104Thread::Run);
    connect(Parse, &IEC104Thread::Started, this, &IEC104::ParseThreadStarted);
    connect(eth, &Ethernet::Connected, Parse, &IEC104Thread::StartDT);
    connect(eth, &Ethernet::Finished, this, &IEC104::EmitReconnectSignal);
    connect(Parse, &IEC104Thread::WriteData, eth, &Ethernet::InitiateWriteDataToPort);
    connect(eth, &Ethernet::NewDataArrived, Parse, &IEC104Thread::GetSomeData);
    connect(Parse, &IEC104Thread::ReconnectSignal, this, &IEC104::EmitReconnectSignal);

    connect(Parse, &IEC104Thread::Bs104signalsreceived, this, &IEC104::Bs104signalsready, Qt::BlockingQueuedConnection);
    connect(Parse, &IEC104Thread::Floatsignalsreceived, this, &IEC104::Floatsignalsready, Qt::BlockingQueuedConnection);
    connect(Parse, &IEC104Thread::Sponsignalsreceived, this, &IEC104::Sponsignalsready, Qt::BlockingQueuedConnection);
    connect(Parse, &IEC104Thread::SendS2fromParse, this, &IEC104::SendS2fromiec104);
    connect(Parse, &IEC104Thread::SendJourSysfromParse, this, &IEC104::SendJourSysfromiec104);
    connect(Parse, &IEC104Thread::SendJourWorkfromParse, this, &IEC104::SendJourWorkfromiec104);
    connect(Parse, &IEC104Thread::SendJourMeasfromParse, this, &IEC104::SendJourMeasfromiec104);

    connect(Parse, &IEC104Thread::SendMessageOk, this, &IEC104::SendMessageOk);

    connect(Parse, &IEC104Thread::SetDataSize, this, &IEC104::SetDataSize);
    connect(Parse, &IEC104Thread::SetDataCount, this, &IEC104::SetDataCount);
    connect(Parse, &IEC104Thread::SendMessagefromParse, this, &IEC104::SendConfMessageOk);

    Parse->SetBaseAdr(st.baseadr);
    Parse->incLS = 0;
    Parse->count = 0;

    thr->start();
    thr2->start();
}

void IEC104::CorReadRequest()
{
    IEC104Thread::InputStruct inp;
    inp.cmd = IEC104Thread::CM104_CORREADREQUEST;
    IEC104Thread::s_ParseWriteMutex.lock();
    InputQueue.enqueue(inp);
    IEC104Thread::s_ParseWriteMutex.unlock();
}

void IEC104::SelectFile(char numFile)
{
    IEC104Thread::InputStruct inp;
    inp.cmd = IEC104Thread::CM104_SELECTFILE;
    inp.args.uintarg = numFile;
    IEC104Thread::s_ParseWriteMutex.lock();
    InputQueue.enqueue(inp);
    IEC104Thread::s_ParseWriteMutex.unlock();
}

void IEC104::FileReady(S2ConfigType *File)
{
    IEC104Thread::InputStruct inp;
    inp.cmd = IEC104Thread::CM104_FILEREADY;
    inp.args.ptrarg = File;
    IEC104Thread::s_ParseWriteMutex.lock();
    InputQueue.enqueue(inp);
    IEC104Thread::s_ParseWriteMutex.unlock();
}

void IEC104::Com45(quint32 com)
{
    IEC104Thread::InputStruct inp;
    inp.cmd = IEC104Thread::CM104_COM45;
    inp.args.uintarg = com;
    IEC104Thread::s_ParseWriteMutex.lock();
    InputQueue.enqueue(inp);
    IEC104Thread::s_ParseWriteMutex.unlock();
}

void IEC104::Com50(quint32 adr, float param)
{
    IEC104Thread::InputStruct inp;
    inp.cmd = IEC104Thread::CM104_COM50;
    inp.args.uintarg = adr;
    inp.args.flarg = param;
    IEC104Thread::s_ParseWriteMutex.lock();
    InputQueue.enqueue(inp);
    IEC104Thread::s_ParseWriteMutex.unlock();
}

void IEC104::InterrogateTimeGr15()
{
    IEC104Thread::InputStruct inp;
    inp.cmd = IEC104Thread::CM104_INTERROGATETIMEGR15;
    IEC104Thread::s_ParseWriteMutex.lock();
    InputQueue.enqueue(inp);
    IEC104Thread::s_ParseWriteMutex.unlock();
}

void IEC104::com51WriteTime(uint time)
{
    IEC104Thread::InputStruct inp;
    inp.cmd = IEC104Thread::CM104_COM51WRITETIME;
    inp.args.uintarg = time;
    IEC104Thread::s_ParseWriteMutex.lock();
    InputQueue.enqueue(inp);
    IEC104Thread::s_ParseWriteMutex.unlock();
}

void IEC104::EthThreadStarted() { EthThreadWorking = true; }

void IEC104::EthThreadFinished()
{
    EthThreadWorking = false;
    if (!ParseThreadWorking)
        emit Finished();
}

void IEC104::ParseThreadStarted() { ParseThreadWorking = true; }

void IEC104::ParseThreadFinished()
{
    ParseThreadWorking = false;
    if (!EthThreadWorking)
        emit Finished();
}

void IEC104::EmitReconnectSignal()
{
    if (!AboutToFinish)
        emit ReconnectSignal();
}

void IEC104::StopAllThreads()
{
    AboutToFinish = true;
    emit StopAll();
}
