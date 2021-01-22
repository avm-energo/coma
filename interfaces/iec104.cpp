#include "iec104.h"

#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/timefunc.h"
#include "ethernet.h"
#include "iec104thread.h"
#include "settingstypes.h"

#include <QCoreApplication>
#include <QThread>

IEC104::IEC104(QObject *parent) : BaseInterface(parent), EthThreadWorking(false), ParseThreadWorking(false)
{

    // Log = new LogClass;
    Log->Init("iec104.log");
    Log->info("=== Log started ===");
    qRegisterMetaType<DataTypes::FloatStruct>();
}

IEC104::~IEC104()
{
}

// bool IEC104::Working() { return (EthThreadWorking | ParseThreadWorking); }

bool IEC104::start(const ConnectStruct &st)
{
    Q_ASSERT(std::holds_alternative<IEC104Settings>(st.settings));
    qInfo() << metaObject()->className() << "connect";
    if (!std::holds_alternative<IEC104Settings>(st.settings))
        return false;
    auto settings = std::get<IEC104Settings>(st.settings);

    EthThreadWorking = false;
    ParseThreadWorking = false;
    QThread *portThread = new QThread;
    portThread->setObjectName("portThread");
    Ethernet *port = new Ethernet;

    connect(portThread, &QThread::started, [&] { EthThreadWorking = true; });

    connect(port, &Ethernet::finished, portThread, &QThread::quit);
    connect(portThread, &QThread::finished, [] { qDebug() << "Port thread finished"; });
    connect(portThread, &QThread::finished, port, &QObject::deleteLater);
    connect(portThread, &QThread::started, port, &Ethernet::Run);
    connect(portThread, &QThread::finished, portThread, &QObject::deleteLater);
    connect(this, &IEC104::StopAll, port, &Ethernet::Stop);
    connect(port, &Ethernet::Disconnected, this, &IEC104::EthThreadFinished);

    IEC104Thread *parser = new IEC104Thread(Log);
    QThread *parserThread = new QThread;
    parserThread->setObjectName("parserThread");

    connect(parserThread, &QThread::started, [&] { ParseThreadWorking = true; });
    connect(this, &IEC104::StopAll, parser, &IEC104Thread::Stop);

    connect(parserThread, &QThread::finished, parser, &QObject::deleteLater);
    connect(parserThread, &QThread::finished, parserThread, &QObject::deleteLater);
    connect(parser, &IEC104Thread::finished, parserThread, &QThread::quit);
    connect(parserThread, &QThread::finished, this, &IEC104::ParseThreadFinished);

    connect(parserThread, &QThread::started, parser, &IEC104Thread::Run);
    connect(port, &Ethernet::Connected, parser, &IEC104Thread::StartDT);

    connect(portThread, &QThread::finished, this, &IEC104::EmitReconnectSignal);

    connect(parser, &IEC104Thread::WriteData, port, &Ethernet::InitiateWriteDataToPort);
    connect(port, &Ethernet::NewDataArrived, parser, &IEC104Thread::GetSomeData);
    connect(parser, &IEC104Thread::ReconnectSignal, this, &IEC104::EmitReconnectSignal);

    port->IP = settings.ip;
    parser->SetBaseAdr(settings.baseadr);

    port->moveToThread(portThread);
    parser->moveToThread(parserThread);
    portThread->start();
    parserThread->start();
    QEventLoop ethloop;
    bool ethconnected = false;
    QTimer *ethtimeouttimer = new QTimer(this);
    ethtimeouttimer->setInterval(10000);
    connect(ethtimeouttimer, &QTimer::timeout, [&]() {
        ethconnected = false;
        ethloop.quit();
    });
    connect(port, &Ethernet::Connected, [&]() {
        ethconnected = true;
        setState(State::Run);
        ethloop.quit();
    });
    ethloop.exec();
    return ethconnected;
}

InterfaceSettings IEC104::parseSettings(QDomElement domElement) const
{
    return BaseInterface::parseSettings<Commands104::Iec104Group>(domElement);
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

// void IEC104::reqAlarms(quint32 sigAdr, quint32 sigCount)
//{
//    Q_UNUSED(sigAdr)
//    Q_UNUSED(sigCount)
//    Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, ALARMGROUP, 0, {} };
//    DataManager::addToInQueue(inp);
//}

void IEC104::reqFile(quint32 filenum, bool isConfigFile)
{
    Commands104::CommandStruct inp { (isConfigFile) ? Commands104::CM104_REQCONFIGFILE : Commands104::CM104_REQFILE,
        filenum, 0, {} };
    DataManager::addToInQueue(inp);
}

void IEC104::writeFile(quint32 filenum, const QByteArray &file)
{
    Commands104::CommandStruct inp { Commands104::CM104_WRITEFILE, filenum, 0, file };
    DataManager::addToInQueue(inp);
}

// void IEC104::writeConfigFile(S2DataTypes::S2ConfigType *s2config)
//{
//    QByteArray ba;
//    S2::StoreDataMem(&ba.data()[0], s2config, Files::Config);
//    Commands104::CommandStruct inp { Commands104::CM104_WRITEFILE, Files::Config, 0, ba };
//    DataManager::addToInQueue(inp);
//}

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

void IEC104::writeCommand(Queries::Commands cmd, QVariant item)
{

    Commands104::CommandStruct inp;

    switch (cmd)
    {
    case Queries::QC_WriteUserValues:
    {
        Q_ASSERT(item.canConvert<DataTypes::FloatStruct>());
        if (!item.canConvert<DataTypes::FloatStruct>())
            return;

        DataTypes::FloatStruct flstr = item.value<DataTypes::FloatStruct>();
        inp = { Commands104::CM104_COM50, flstr.sigAdr, flstr.sigVal, {} };
        DataManager::addToInQueue(inp);
        break;
    }
    case Queries::QC_ReqAlarms:
    {
        inp = { Commands104::CM104_REQGROUP, ALARMGROUP, 0, {} };
        DataManager::addToInQueue(inp);
        break;
    }
    default:
    {
        inp = Commands104::CommandsTranslateMap.value(cmd);
        DataManager::addToInQueue(inp);
    }
    }
}

void IEC104::reqFloats(quint32 sigAdr, quint32 sigCount)
{
    Q_UNUSED(sigAdr)
    Q_UNUSED(sigCount)
    Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, MAINFLOATGROUP, 0, {} };
    DataManager::addToInQueue(inp);
}

// void IEC104::reqBitStrings(quint32 sigAdr, quint32 sigCount)
//{
//    Q_UNUSED(sigAdr)
//    Q_UNUSED(sigCount)
//    Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, MAINBITSTRINGGROUP, 0, {} };
//    DataManager::addToInQueue(inp);
//}

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

// void IEC104::EthThreadStarted()
//{
//    EthThreadWorking = true;
//}

void IEC104::EthThreadFinished()
{
    EthThreadWorking = false;
    if (!ParseThreadWorking)
    {
        setState(State::Stop);
        emit Finished();
    }
}

void IEC104::ParseThreadFinished()
{
    ParseThreadWorking = false;
    if (!EthThreadWorking)
    {
        setState(State::Stop);
        emit Finished();
    }
}

void IEC104::EmitReconnectSignal()
{
    qDebug() << __PRETTY_FUNCTION__;
    if (state() != State::Wait)
        emit reconnect();
}

void IEC104::stop()
{
    setState(BaseInterface::State::Wait);
    emit StopAll();
}
