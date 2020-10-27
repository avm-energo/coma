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
    m_working = false;
    EthThreadWorking = false;
    ParseThreadWorking = false;
    AboutToFinish = false;
    Log = new LogClass;
    Log->Init("iec104.log");
    Log->info("=== Log started ===");
}

IEC104::~IEC104() { }

// bool IEC104::Working() { return (EthThreadWorking | ParseThreadWorking); }

bool IEC104::start(const ConnectStruct &st)
{
    INFOMSG("IEC104: connect");
    m_working = false;
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
    return true;
}

void IEC104::reqStartup(quint32 sigAdr, quint32 sigCount)
{
    // adr & count are used in modbus only, 104 has special group STARTUPGROUP for these parameters
    Q_UNUSED(sigAdr)
    Q_UNUSED(sigCount)
    Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, STARTUPGROUP, 0, {} };
    DataManager::addToInQueue(inp);
}

void IEC104::reqBSI()
{
    Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, BSIGROUP, 0, {} };
    DataManager::addToInQueue(inp);
}

void IEC104::reqAlarms(quint32 sigAdr, quint32 sigCount)
{
    Q_UNUSED(sigAdr)
    Q_UNUSED(sigCount)
    Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, ALARMGROUP, 0, {} };
    DataManager::addToInQueue(inp);
}

void IEC104::reqFile(quint32 filenum)
{
    Commands104::CommandStruct inp { Commands104::CM104_REQFILE, filenum, 0, {} };
    DataManager::addToInQueue(inp);
}

void IEC104::writeFile(quint32 filenum, const QByteArray &file)
{
    Commands104::CommandStruct inp { Commands104::CM104_WRITEFILE, filenum, 0, file };
    DataManager::addToInQueue(inp);
}

void IEC104::writeConfigFile(S2ConfigType *s2config)
{
    QByteArray ba;
    S2::StoreDataMem(&ba.data()[0], s2config, Files::Config);
    Commands104::CommandStruct inp { Commands104::CM104_WRITEFILE, Files::Config, 0, ba };
    DataManager::addToInQueue(inp);
}

void IEC104::reqTime()
{
    Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, TIMEGROUP, 0, {} };
    DataManager::addToInQueue(inp);
}

void IEC104::writeTime(quint32 time)
{
    Commands104::CommandStruct inp { Commands104::CM104_COM51, time, 0, {} };
    DataManager::addToInQueue(inp);
}

void IEC104::writeCommand(Queries::Commands cmd, QList<DataTypes::SignalsStruct> &list)
{
    Commands104::CommandStruct inp;
    if (cmd == Queries::QC_WriteUserValues)
    {
        // for each signal in list form the 50 command and set it into the input queue
        foreach (DataTypes::SignalsStruct str, list)
        {
            QVariant var = str.data;
            if (var.canConvert<DataTypes::FloatStruct>())
            {
                DataTypes::FloatStruct flstr = var.value<DataTypes::FloatStruct>();
                inp = { Commands104::CM104_COM50, flstr.sigAdr, flstr.sigVal, {} };
                DataManager::addToInQueue(inp);
            }
        }
    }
    else
    {
        inp = Commands104::CommandsTranslateMap().value(cmd);
        DataManager::addToInQueue(inp);
    }
}

// void IEC104::CorReadRequest()
//{
//    Commands104::CommandStruct104 inp { Commands104104::CM104_CORREADREQUEST, 0, 0, {} };
//    DataManager::addToInQueue(inp);
//    //    IEC104Thread::InputStruct inp;
//    //    inp.cmd = IEC104Thread::CM104_CORREADREQUEST;
//    //    IEC104Thread::s_ParseWriteMutex.lock();
//    //    m_inputQueue.enqueue(inp);
//    //    IEC104Thread::s_ParseWriteMutex.unlock();
//}

// void IEC104::SelectFile(char numFile)
//{
//    Commands104::CommandStruct inp { Commands104::CM104_SELECTFILE, static_cast<quint32>(numFile), 0, {} };
//    DataManager::addToInQueue(inp);
//    //    IEC104Thread::InputStruct inp;
//    //    inp.cmd = IEC104Thread::CM104_SELECTFILE;
//    //    inp.args.uintarg = numFile;
//    //    IEC104Thread::s_ParseWriteMutex.lock();
//    //    m_inputQueue.enqueue(inp);
//    //    IEC104Thread::s_ParseWriteMutex.unlock();
//}

// void IEC104::FileReady(S2ConfigType *s2config)
//{
//    QByteArray ba;
//    S2::StoreDataMem(&ba.data()[0], s2config, Files::Config);
//    Commands104::CommandStruct inp { Commands104::WRITEFILE, Files::Config, 0, ba };
//    DataManager::addToInQueue(inp);
//    //    IEC104Thread::InputStruct inp;
//    //    inp.cmd = IEC104Thread::CM104_FILEREADY;
//    //    inp.args.ptrarg = File;
//    //    IEC104Thread::s_ParseWriteMutex.lock();
//    //    m_inputQueue.enqueue(inp);
//    //    IEC104Thread::s_ParseWriteMutex.unlock();
//}

// void IEC104::Com45(quint32 com)
//{
//    Commands104::CommandStruct inp { Commands104::CM104_COM45, com, 0, {} };
//    DataManager::addToInQueue(inp);
//    //    IEC104Thread::InputStruct inp;
//    //    inp.cmd = IEC104Thread::CM104_COM45;
//    //    inp.args.uintarg = com;
//    //    IEC104Thread::s_ParseWriteMutex.lock();
//    //    m_inputQueue.enqueue(inp);
//    //    IEC104Thread::s_ParseWriteMutex.unlock();
//}

// void IEC104::Com50(quint32 adr, float param)
//{
//    Commands104::CommandStruct inp { Commands104::CM104_COM50, adr, param, {} };
//    DataManager::addToInQueue(inp);
//    //    IEC104Thread::InputStruct inp;
//    //    inp.cmd = IEC104Thread::CM104_COM50;
//    //    inp.args.uintarg = adr;
//    //    inp.args.flarg = param;
//    //    IEC104Thread::s_ParseWriteMutex.lock();
//    //    m_inputQueue.enqueue(inp);
//    //    IEC104Thread::s_ParseWriteMutex.unlock();
//}

// void IEC104::getTime()
//{
//    Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, TIMEGROUP, 0, {} };
//    DataManager::addToInQueue(inp);
//    //    IEC104Thread::InputStruct inp;
//    //    inp.cmd = IEC104Thread::CM104_INTERROGATETIMEGR15;
//    //    IEC104Thread::s_ParseWriteMutex.lock();
//    //    m_inputQueue.enqueue(inp);
//    //    IEC104Thread::s_ParseWriteMutex.unlock();
//}

// void IEC104::com51WriteTime(uint time)
//{
//    Commands104::CommandStruct inp { Commands104::CM104_COM51, time, 0, {} };
//    DataManager::addToInQueue(inp);
//    //    IEC104Thread::InputStruct inp;
//    //    inp.cmd = IEC104Thread::CM104_COM51WRITETIME;
//    //    inp.args.uintarg = time;
//    //    IEC104Thread::s_ParseWriteMutex.lock();
//    //    m_inputQueue.enqueue(inp);
//    //    IEC104Thread::s_ParseWriteMutex.unlock();
//}

void IEC104::EthThreadStarted() { m_working = EthThreadWorking = true; }

void IEC104::EthThreadFinished()
{
    EthThreadWorking = false;
    if (!ParseThreadWorking)
    {
        m_working = false;
        emit Finished();
    }
}

void IEC104::ParseThreadStarted() { m_working = ParseThreadWorking = true; }

void IEC104::ParseThreadFinished()
{
    ParseThreadWorking = false;
    if (!EthThreadWorking)
    {
        m_working = false;
        emit Finished();
    }
}

void IEC104::EmitReconnectSignal()
{
    if (!AboutToFinish)
        emit ReconnectSignal();
}

void IEC104::stop()
{
    AboutToFinish = true;
    emit StopAll();
}
