#include "iec104.h"

#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/timefunc.h"
#include "ethernet.h"
#include "iec104thread.h"
#include "settingstypes.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QThread>

IEC104::IEC104(QObject *parent)
    : BaseInterface(parent), EthThreadWorking(false), ParseThreadWorking(false), sock(new QTcpSocket(this))
{
    Log->Init("iec104.log");
    Log->info("=== Log started ===");
}

IEC104::~IEC104()
{
}

bool IEC104::start(const ConnectStruct &st)
{
    Q_ASSERT(std::holds_alternative<IEC104Settings>(st.settings));
    qInfo() << metaObject()->className() << "connect";
    if (!std::holds_alternative<IEC104Settings>(st.settings))
        return false;
    auto settings = std::get<IEC104Settings>(st.settings);

    EthThreadWorking = false;
    ParseThreadWorking = false;

    IEC104Thread *parser = new IEC104Thread(Log);
    QThread *parserThread = new QThread;
    parserThread->setObjectName("parserThread");

    connect(parserThread, &QThread::started, [&] { ParseThreadWorking = true; });
    connect(this, &IEC104::StopAll, parser, &IEC104Thread::Stop);
    connect(this, &IEC104::StopAll, [=] {
        if (sock->isOpen())
        {
            sock->close();
            sock->disconnect();
            sock->deleteLater();
        }
    });
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(sock, qOverload<QAbstractSocket::SocketError>(&QAbstractSocket::error), this,
        [=](QAbstractSocket::SocketError error) {
            Log->info("Error: " + QVariant::fromValue(error).toString());
            sock->disconnectFromHost();
        });
#else
    connect(sock, &QAbstractSocket::errorOccurred, this, [=](QAbstractSocket::SocketError error) {
        Log->info("Error: " + QVariant::fromValue(error).toString());
        sock->disconnectFromHost();
    });
#endif
    connect(sock, &QAbstractSocket::stateChanged, this, &IEC104::EthStateChanged);
    connect(parserThread, &QThread::finished, parser, &QObject::deleteLater);
    connect(parserThread, &QThread::finished, parserThread, &QObject::deleteLater);
    connect(parser, &IEC104Thread::finished, parserThread, &QThread::quit);
    connect(parserThread, &QThread::finished, this, &IEC104::ParseThreadFinished);

    connect(parserThread, &QThread::started, parser, &IEC104Thread::Run);

    connect(parser, &IEC104Thread::WriteData, sock, [=](const QByteArray ba) {
        qint64 res = sock->write(ba);
        Log->info(QString::number(res) + " bytes written");
    });
    connect(sock, &QAbstractSocket::disconnected, this, &IEC104::EthThreadFinished);

    connect(
        sock, &QIODevice::readyRead, parser,
        [=] {
            QByteArray ba = sock->readAll();
            QMetaObject::invokeMethod(parser, [=] { parser->GetSomeData(ba); });
        },
        Qt::QueuedConnection);
    connect(parser, &IEC104Thread::ReconnectSignal, this, &IEC104::EmitReconnectSignal);

    StdFunc::SetDeviceIP(settings.ip);
    parser->SetBaseAdr(settings.baseadr);

    parser->moveToThread(parserThread);
    parserThread->start();

    QEventLoop ethloop;
    bool ethconnected = false;
    QTimer *ethtimeouttimer = new QTimer(this);
    ethtimeouttimer->setSingleShot(true);
    connect(ethtimeouttimer, &QTimer::timeout, [&]() {
        qCritical() << Error::Timeout;
        ethconnected = false;
        ethloop.quit();
    });
    connect(sock, &QAbstractSocket::connected, parser, [&] {
        ethtimeouttimer->stop();
        qInfo() << "Connected";
        ethconnected = true;
        EthThreadWorking = true;
        ethloop.quit();
        QMetaObject::invokeMethod(parser, &IEC104Thread::StartDT);
    });
    connect(sock, &QAbstractSocket::connected, this, [=] { setState(State::Run); });
    ethtimeouttimer->start(INTERVAL::WAIT);

    sock->connectToHost(StdFunc::ForDeviceIP(), PORT104, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
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

void IEC104::reqBSIExt()
{
    // No request, bsiExt is in the same group with bsi, so request bsi also requests bsi ext
}

// void IEC104::reqAlarms(quint32 sigAdr, quint32 sigCount)
//{
//    Q_UNUSED(sigAdr)
//    Q_UNUSED(sigCount)
//    Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, ALARMGROUP, 0, {} };
//    DataManager::addToInQueue(inp);
//}

void IEC104::reqFile(quint32 filenum, FileFormat format)
{
    Commands104::CommandStruct inp { (format) ? Commands104::CM104_REQCONFIGFILE : Commands104::CM104_REQFILE, filenum,
        0, {} };
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

void IEC104::reqBitStrings(quint32 sigAdr, quint32 sigCount)
{
    Q_UNUSED(sigAdr)
    Q_UNUSED(sigCount)
    Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, MAINBITSTRINGGROUP, 0, {} };
    DataManager::addToInQueue(inp);
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

void IEC104::EthStateChanged(QAbstractSocket::SocketState state)
{
    switch (state)
    {
    case QAbstractSocket::UnconnectedState:
        Log->info("Socket unconnected");
        break;
    case QAbstractSocket::HostLookupState:
        Log->info("Socket enters host lookup state");
        break;
    case QAbstractSocket::ConnectingState:
        Log->info("Socket enters connecting state");
        break;
    case QAbstractSocket::ConnectedState:
        Log->info("Socket connected!");
        break;
    case QAbstractSocket::BoundState:
        Log->info("Socket is bound to address and port");
        break;
    case QAbstractSocket::ClosingState:
        Log->info("Socket is in closing state");
        break;
    case QAbstractSocket::ListeningState:
        Log->info("Socket is in listening state");
        break;
    default:
        Log->info("Unprocessed state");
        break;
    }
}

void IEC104::stop()
{
    setState(BaseInterface::State::Wait);
    emit StopAll();
}
