#include "iec104.h"

#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/timefunc.h"
#include "ethernet.h"

#include <QCoreApplication>
#include <QDialog>
#include <QThread>
#include <QVBoxLayout>

IEC104::IEC104(QObject *parent) : BaseInterface(parent)
{
    EthThreadWorking = false;
    ParseThreadWorking = false;
    AboutToFinish = false;
    Log = new LogClass;
    Log->Init("iec104.log");
    Log->info("=== Log started ===");
}

IEC104::~IEC104() { }

bool IEC104::Working() { return (EthThreadWorking | ParseThreadWorking); }

void IEC104::start(const ConnectStruct &st)
{
    INFOMSG("IEC104: connect");
    EthThreadWorking = false;
    ParseThreadWorking = false;
    AboutToFinish = false;
    QThread *thr = new QThread;
    Ethernet *eth = new Ethernet;
    eth->moveToThread(thr);
    eth->IP = st.iec104st.ip;
    connect(eth, &Ethernet::Finished, thr, &QThread::quit);
    connect(eth, &Ethernet::Finished, eth, &QObject::deleteLater);
    connect(thr, &QThread::started, eth, &Ethernet::Run);
    connect(thr, &QThread::finished, thr, &QObject::deleteLater);
    connect(this, &IEC104::StopAll, eth, &Ethernet::Stop);
    connect(eth, &Ethernet::Connected, this, &IEC104::EthThreadStarted);
    connect(eth, &Ethernet::Disconnected, this, &IEC104::EthThreadFinished);

    IEC104Thread *m_thread104 = new IEC104Thread(Log);
    QThread *thr2 = new QThread;
    m_thread104->moveToThread(thr2);
    connect(this, &IEC104::StopAll, m_thread104, &IEC104Thread::Stop);
    connect(m_thread104, &IEC104Thread::Finished, m_thread104, &QObject::deleteLater);
    connect(m_thread104, &IEC104Thread::Finished, thr2, &QThread::quit);
    connect(m_thread104, &IEC104Thread::Finished, this, &IEC104::ParseThreadFinished);
    connect(thr2, &QThread::finished, thr2, &QObject::deleteLater);
    connect(thr2, &QThread::started, m_thread104, &IEC104Thread::Run);
    connect(m_thread104, &IEC104Thread::Started, this, &IEC104::ParseThreadStarted);
    connect(eth, &Ethernet::Connected, m_thread104, &IEC104Thread::StartDT);
    connect(eth, &Ethernet::Finished, this, &IEC104::EmitReconnectSignal);
    connect(m_thread104, &IEC104Thread::WriteData, eth, &Ethernet::InitiateWriteDataToPort);
    connect(eth, &Ethernet::NewDataArrived, m_thread104, &IEC104Thread::GetSomeData);
    connect(m_thread104, &IEC104Thread::ReconnectSignal, this, &IEC104::EmitReconnectSignal);

    //    connect(m_thread104, &IEC104Thread::Bs104signalsreceived, this, &IEC104::Bs104signalsready,
    //        Qt::BlockingQueuedConnection);
    //    connect(m_thread104, &IEC104Thread::Floatsignalsreceived, this, &IEC104::Floatsignalsready,
    //        Qt::BlockingQueuedConnection);
    //    connect(
    //        m_thread104, &IEC104Thread::Sponsignalsreceived, this, &IEC104::Sponsignalsready,
    //        Qt::BlockingQueuedConnection);
    //    connect(m_thread104, &IEC104Thread::SendS2fromParse, this, &IEC104::SendS2fromiec104);
    //    connect(m_thread104, &IEC104Thread::SendJourSysfromParse, this, &IEC104::SendJourSysfromiec104);
    //    connect(m_thread104, &IEC104Thread::SendJourWorkfromParse, this, &IEC104::SendJourWorkfromiec104);
    //    connect(m_thread104, &IEC104Thread::SendJourMeasfromParse, this, &IEC104::SendJourMeasfromiec104);

    //    connect(m_thread104, &IEC104Thread::SendMessageOk, this, &IEC104::SendMessageOk);

    //    connect(m_thread104, &IEC104Thread::SetDataSize, this, &IEC104::SetDataSize);
    //    connect(m_thread104, &IEC104Thread::SetDataCount, this, &IEC104::SetDataCount);
    //    connect(m_thread104, &IEC104Thread::SendMessagefromParse, this, &IEC104::SendConfMessageOk);

    m_thread104->SetBaseAdr(st.iec104st.baseadr);
    //    m_thread104->incLS = 0;
    //    m_thread104->count = 0;

    thr->start();
    thr2->start();
}

void IEC104::reqStartup()
{
    Queries::Command inp { Queries::Commands::CM104_REQGROUP, STARTUPGROUP, 0, {} };
    DataManager::addToInQueue(inp);
}

// void IEC104::CorReadRequest()
//{
//    Queries::Command104 inp { Queries::Commands104::CM104_CORREADREQUEST, 0, 0, {} };
//    DataManager::addToInQueue(inp);
//    //    IEC104Thread::InputStruct inp;
//    //    inp.cmd = IEC104Thread::CM104_CORREADREQUEST;
//    //    IEC104Thread::s_ParseWriteMutex.lock();
//    //    m_inputQueue.enqueue(inp);
//    //    IEC104Thread::s_ParseWriteMutex.unlock();
//}

void IEC104::SelectFile(char numFile)
{
    Queries::Command inp { Queries::Commands::CM104_SELECTFILE, static_cast<quint32>(numFile), 0, {} };
    DataManager::addToInQueue(inp);
    //    IEC104Thread::InputStruct inp;
    //    inp.cmd = IEC104Thread::CM104_SELECTFILE;
    //    inp.args.uintarg = numFile;
    //    IEC104Thread::s_ParseWriteMutex.lock();
    //    m_inputQueue.enqueue(inp);
    //    IEC104Thread::s_ParseWriteMutex.unlock();
}

void IEC104::FileReady(S2ConfigType *s2config)
{
    QByteArray ba;
    S2::StoreDataMem(&ba.data()[0], s2config, Files::Config);
    Queries::Command inp { Queries::Commands::CM104_FILEREADY, Files::Config, 0, ba };
    DataManager::addToInQueue(inp);
    //    IEC104Thread::InputStruct inp;
    //    inp.cmd = IEC104Thread::CM104_FILEREADY;
    //    inp.args.ptrarg = File;
    //    IEC104Thread::s_ParseWriteMutex.lock();
    //    m_inputQueue.enqueue(inp);
    //    IEC104Thread::s_ParseWriteMutex.unlock();
}

void IEC104::Com45(quint32 com)
{
    Queries::Command inp { Queries::Commands::CM104_COM45, com, 0, {} };
    DataManager::addToInQueue(inp);
    //    IEC104Thread::InputStruct inp;
    //    inp.cmd = IEC104Thread::CM104_COM45;
    //    inp.args.uintarg = com;
    //    IEC104Thread::s_ParseWriteMutex.lock();
    //    m_inputQueue.enqueue(inp);
    //    IEC104Thread::s_ParseWriteMutex.unlock();
}

void IEC104::Com50(quint32 adr, float param)
{
    Queries::Command inp { Queries::Commands::CM104_COM50, adr, param, {} };
    DataManager::addToInQueue(inp);
    //    IEC104Thread::InputStruct inp;
    //    inp.cmd = IEC104Thread::CM104_COM50;
    //    inp.args.uintarg = adr;
    //    inp.args.flarg = param;
    //    IEC104Thread::s_ParseWriteMutex.lock();
    //    m_inputQueue.enqueue(inp);
    //    IEC104Thread::s_ParseWriteMutex.unlock();
}

void IEC104::getTime()
{
    Queries::Command inp { Queries::Commands::CM104_REQGROUP, TIMEGROUP, 0, {} };
    DataManager::addToInQueue(inp);
    //    IEC104Thread::InputStruct inp;
    //    inp.cmd = IEC104Thread::CM104_INTERROGATETIMEGR15;
    //    IEC104Thread::s_ParseWriteMutex.lock();
    //    m_inputQueue.enqueue(inp);
    //    IEC104Thread::s_ParseWriteMutex.unlock();
}

void IEC104::com51WriteTime(uint time)
{
    Queries::Command inp { Queries::Commands::CM104_COM51WRITETIME, time, 0, {} };
    DataManager::addToInQueue(inp);
    //    IEC104Thread::InputStruct inp;
    //    inp.cmd = IEC104Thread::CM104_COM51WRITETIME;
    //    inp.args.uintarg = time;
    //    IEC104Thread::s_ParseWriteMutex.lock();
    //    m_inputQueue.enqueue(inp);
    //    IEC104Thread::s_ParseWriteMutex.unlock();
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
