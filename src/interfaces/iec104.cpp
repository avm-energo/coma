#include "iec104.h"

#include "ethernet.h"
#include "iec104thread.h"
#include "settingstypes.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QThread>
#include <gen/error.h>
#include <gen/files.h>
#include <gen/timefunc.h>

namespace Interface
{

IEC104::IEC104(QObject *parent)
    : BaseInterface(parent), EthThreadWorking(false), ParseThreadWorking(false), sock(new QTcpSocket(this))
{
    Log = new LogClass;
    Log->Init("ethernet.log");
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

    auto parser = new IEC104Thread();
    auto parserThread = new QThread;
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
    connect(sock, &QAbstractSocket::errorOccurred, this, [=](QAbstractSocket::SocketError error) {
        Log->info("Error: " + QVariant::fromValue(error).toString());
        sock->disconnectFromHost();
    });
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
            QMetaObject::invokeMethod(parser, [=] { parser->processReadBytes(ba); });
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

/*void IEC104::reqStartup(quint32 sigAdr, quint32 sigCount)
{
    // adr & count are used in modbus only, 104 has special group STARTUPGROUP for these parameters
    Q_UNUSED(sigAdr)
    Q_UNUSED(sigCount)
    Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, STARTUPGROUP, 0, {} };
    DataManager::GetInstance().addToInQueue(inp);
}

void IEC104::reqBSI()
{
    Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, BSIGROUP, 0, {} };
    DataManager::GetInstance().addToInQueue(inp);
}

void IEC104::reqBSIExt()
{
    // No request, bsiExt is in the same group with bsi, so request bsi also requests bsi ext
}

void IEC104::reqFile(quint32 filenum, Datatypes::FileFormat format)
{
    auto cmd104 = (format) ? Commands104::CM104_REQCONFIGFILE : Commands104::CM104_REQFILE;
    Commands104::CommandStruct inp { cmd104, filenum, 0, {} };
    DataManager::GetInstance().addToInQueue(inp);
}

void IEC104::writeFile(quint32 filenum, const QByteArray &file)
{
    Commands104::CommandStruct inp { Commands104::CM104_WRITEFILE, filenum, 0, true, file };
    DataManager::GetInstance().addToInQueue(inp);
}

void IEC104::reqTime()
{
    Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, TIMEGROUP, 0, {} };
    DataManager::GetInstance().addToInQueue(inp);
}

void IEC104::writeTime(quint32 time)
{
    Commands104::CommandStruct inp { Commands104::CM104_COM51, time, 0, {} };
    DataManager::GetInstance().addToInQueue(inp);
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
        DataManager::GetInstance().addToInQueue(inp);
        break;
    }
    case Queries::QC_ReqAlarms:
    {
        inp = { Commands104::CM104_REQGROUP, ALARMGROUP, 0, {} };
        DataManager::GetInstance().addToInQueue(inp);
        break;
    }
    default:
    {
        auto iec104cmd = get104Command(cmd);
        inp = { Commands104::CM104_COM45, iec104cmd, 0, true, {} };
        DataManager::GetInstance().addToInQueue(inp);
    }
    }
}

Commands104::CommandRegisters IEC104::get104Command(Commands cmd)
{
    using namespace Commands104;
    static const QMap<Commands, Commands104::CommandRegisters> iec104CommandMap {
        { C_SetNewConfiguration, SetNewConfigurationReg },   //
        { C_SetStartupValues, SetStartupValuesReg },         //
        { C_SetStartupPhaseA, SetStartupPhaseA },            //
        { C_SetStartupPhaseB, SetStartupPhaseB },            //
        { C_SetStartupPhaseC, SetStartupPhaseC },            //
        { C_SetStartupUnbounced, SetStartupUnbounced },      //
        { C_ClearStartupUnbounced, ClearStartupUnbounced },  //
        { C_SetTransOff, SetTransOff },                      //
        { C_ClearStartupValues, ClearStartupValuesReg },     //
        { C_ClearStartupError, ClearStartupSetError },       //
        { C_StartFirmwareUpgrade, StartFirmwareUpgradeReg }, //
        { C_StartWorkingChannel, StartWorkingChannelReg }    //
    };
    return iec104CommandMap.value(cmd);
}

void IEC104::reqFloats(quint32 sigAdr, quint32 sigCount)
{
    Q_UNUSED(sigAdr)
    Q_UNUSED(sigCount)
    Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, MAINFLOATGROUP, 0, {} };
    DataManager::GetInstance().addToInQueue(inp);
}

void IEC104::reqBitStrings(quint32 sigAdr, quint32 sigCount)
{
    Q_UNUSED(sigAdr)
    Q_UNUSED(sigCount)
    Commands104::CommandStruct inp { Commands104::CM104_REQGROUP, MAINBITSTRINGGROUP, 0, {} };
    DataManager::GetInstance().addToInQueue(inp);
}

*/
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

void IEC104::disconnect()
{
    setState(State::Wait);
    emit StopAll();
}

}
