#include "modbus.h"

#include "../gen/board.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"

#include <QStandardPaths>
#include <QThread>

QMutex RunMutex, InMutex, OutMutex, OutWaitMutex;
QWaitCondition RunWC, OutWC;

ModBus::ModBus(QObject *parent) : QObject(parent)
{
    Log = new LogClass;
    Log->Init("modbus.log");
    CycleGroup = 0;
    MainPollEnabled = true;
    TimePollEnabled = AboutToFinish = false;

    PollingTimer = new QTimer;
    PollingTimer->setInterval(POLLINGINTERVAL);
    connect(PollingTimer, &QTimer::timeout, this, &ModBus::Polling);

    SignalGroups[0] = QByteArrayLiteral("\x04\x00\x65\x00\x04");
    SignalGroups[1] = QByteArrayLiteral("\x04\x03\xE8\x00\x20");
    SignalGroups[2] = QByteArrayLiteral("\x04\x04\x4c\x00\x20");
    SignalGroups[3] = QByteArrayLiteral("\x04\x09\x60\x00\x0E");
    SignalGroups[4] = QByteArrayLiteral("\x04\x09\x74\x00\x1c");
    SignalGroups[5] = QByteArrayLiteral("\x04\x11\x95\x00\x04");
    SignalGroups[6] = QByteArrayLiteral("\x01\x0b\xc3\x00\x19");
    Log->info("=== Log started ===");
}

ModBus::~ModBus()
{
}

Error::Msg ModBus::Connect(SerialPort::Settings &settings)
{
    INFOMSG("Modbus: connect");
    Settings = settings;
    SerialPort *port = new SerialPort();
    ModbusThread *cthr = new ModbusThread();
    //    cthr->Init(&InQueue, &OutList);
    QThread *thr = new QThread;
    cthr->moveToThread(thr);
    connect(thr, &QThread::started, cthr, &ModbusThread::Run);
    connect(cthr, &ModbusThread::Finished, thr, &QThread::quit);
    connect(thr, &QThread::finished, thr, &QObject::deleteLater);
    connect(cthr, &ModbusThread::Finished, cthr, &QObject::deleteLater);
    connect(cthr, &ModbusThread::Finished, this, &ModBus::Finished);
    connect(this, &ModBus::FinishModbusThread, cthr, &ModbusThread::FinishThread);
    connect(this, &ModBus::FinishModbusThread, port, &SerialPort::Disconnect);
    connect(port, &SerialPort::Read, cthr, &ModbusThread::ParseReply);
    connect(cthr, &ModbusThread::Write, port, &SerialPort::WriteBytes);
    connect(port, &SerialPort::Reconnect, this, &ModBus::SendReconnectSignal);
    if (port->Init(settings) != Error::Msg::NoError)
        return Error::Msg::GeneralError;
    thr->start();
    StdFunc::Wait(1000);
    //    StartPolling();
    AboutToFinish = false;
    Log->info("Polling started, thread initiated");
    return Error::Msg::NoError;
}

Error::Msg ModBus::SendAndGetResult(Queries::CommandMBS &request, InOutStruct &outp)
{
    QByteArray bytes {};

    bytes.append(static_cast<char>(Settings.Address)); // адрес устройства
    bytes.append(request.command);                     //аналоговый выход
    bytes.append(static_cast<char>((request.address & 0xFF00) >> 8));
    bytes.append(static_cast<char>(request.address & 0x00FF));
    bytes.append(static_cast<char>((request.quantity & 0xFF00) >> 8));
    bytes.append(static_cast<char>(request.quantity & 0x00FF));
    if (request.command == 0x10)
        bytes.append(static_cast<char>(request.sizeBytes));
    if (request.data.size())
        bytes.append(request.data);

    Log->info("Send bytes: " + bytes.toHex());

    InOutStruct inp {
        request.command,     // Command
        bytes,               // Ba
        0,                   // TaskNum
        Error::Msg::NoError, // Res
        0,                   // ReadSize
        0                    // Checked
    };
    if (request.command == Queries::CommandsMBS::MBS_WRITEMULTIPLEREGISTERS)
        inp.ReadSize = 8;
    else
        inp.ReadSize = 5 + 2 * request.quantity;
    // wait for an answer or timeout and return result
    if (SendAndGet(inp, outp) != Error::Msg::NoError)
    {
        Log->warning("Error, bytes: " + outp.Ba.toHex());
        return Error::Msg::GeneralError;
    }
    Log->info("Rcv bytes: " + outp.Ba.toHex());
    return Error::Msg::NoError;
}

void ModBus::Polling()
{
    QByteArray bytes {};
    InOutStruct inp {}, outp {};

    if (MainPollEnabled)
    {
        inp.Command = SignalGroups[CycleGroup][0];
        bytes.append(static_cast<char>(Settings.Address)); // адрес устройства
        bytes.append(SignalGroups[CycleGroup]);
        if (CycleGroup == 6)
            inp.ReadSize = 9;
        else
            inp.ReadSize = 5 + 2 * SignalGroups[CycleGroup][SECONDBYTEQ];

        inp.Ba = bytes;
        // wait for an answer or timeout and return result
        SendAndGet(inp, outp);

        if ((CycleGroup == 6) && (outp.Ba.size() > 3))
        {
            Coils coil;
            coil.countBytes = outp.Ba.data()[2];
            coil.Bytes = outp.Ba.mid(3);
            emit CoilSignalsReady(coil);
        }
        else
        {
            QList<SignalStruct> Sig;
            int sigsize;
            int startadr = (static_cast<quint8>(SignalGroups[CycleGroup][FIRSTBYTEADR]) << 8)
                | (static_cast<quint8>(SignalGroups[CycleGroup][SECONDBYTEADR]));
            GetFloatSignalsFromByteArray(outp.Ba, startadr, Sig, sigsize);
            emit SignalsReceived(Sig);
        }
        CycleGroup++;
        if (CycleGroup > 6)
            CycleGroup = 0;
    }
    if (TimePollEnabled)
    {
        ReadTime();
    }
}

void ModBus::SendReconnectSignal()
{
    if (!AboutToFinish)
        emit ReconnectSignal();
}

void ModBus::Stop()
{
    Log->info("Stop()");
    AboutToFinish = true;
    StopPolling();
    emit FinishModbusThread();
}

Error::Msg ModBus::SendAndGet(InOutStruct &inp, ModBus::InOutStruct &outp)
{
    Error::Msg msg = Error::Msg::NoError;
    QElapsedTimer tmetimeout;

    if (AboutToFinish)
    {
        ERMSG("Command while about to finish");
        return msg;
    }
    inp.TaskNum = _taskCounter++;
    if (_taskCounter >= INT_MAX)
        _taskCounter = 0; // to prevent negative numbers
    InMutex.lock();
    InQueue.enqueue(inp);
    InMutex.unlock();
    RunWC.wakeAll();

    bool Finished = false;
    tmetimeout.start();

    while (!Finished)
    {
        if (GetResultFromOutQueue(inp.TaskNum, outp))
            return msg;
        OutWaitMutex.lock();
        OutWC.wait(&OutWaitMutex, 20);
        OutWaitMutex.unlock();
        if (tmetimeout.elapsed() > RECONNECTTIME)
        {
            ERMSG("Timeout error");
            // outp.Res = Error::Msg::GENERALERROR;
            msg = Error::Msg::GeneralError;
            Finished = true;
        }
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    return msg;
}

bool ModBus::GetResultFromOutQueue(int index, ModBus::InOutStruct &outp)
{
    OutMutex.lock();
    if (!OutList.isEmpty())
    {
        for (int i = 0; i < OutList.size(); ++i)
        {
            if (OutList.at(i).TaskNum == index)
            {
                outp = OutList.takeAt(i);
                OutMutex.unlock();
                return true;
            }
        }
    }
    OutMutex.unlock();
    return false;
}

void ModBus::BSIrequest()
{
    Queries::CommandMBS request {
        Queries::CommandsMBS::MBS_READINPUTREGISTER, // Command
        BSIREG,                                      // Address
        30,                                          // Quantity
        60,                                          // SizeBytes
        {}                                           // Data
    };
    InOutStruct outp {};

    Log->info("BSIRequest()");

    Error::Msg res = SendAndGetResult(request, outp);
    if (res != Error::Msg::NoError)
        emit TimeReadError();

    QList<BSISignalStruct> BSIsig; // = nullptr;
    unsigned int sigsize;
    if (GetSignalsFromByteArray(outp.Ba, BSIREG, BSIsig, sigsize) != Error::Msg::NoError)
    {
        ERMSG("Ошибка взятия сигнала из очереди по modbus");
        return;
    }

    emit BsiFromModbus(BSIsig, sigsize);
}

void ModBus::ModWriteCor(ModBus::Information info, float *data) //, int* size)
{
    Queries::CommandMBS request {
        Queries::CommandsMBS::MBS_WRITEMULTIPLEREGISTERS, // Command
        info.adr,                                         // Address
        0,                                                // Quantity
        0,                                                // SizeBytes
        {}                                                // Data
    };
    InOutStruct outp {};

    Log->info("ModWriteCor()");

    if ((info.adr == MBS_SETINITREG)
        || (info.adr == MBS_CLEARREG)) // set initial values or clear initial values commands
    {
        request.quantity = 1;
        request.sizeBytes = 2;
        request.data = QByteArrayLiteral("\x01\x01");
    }
    else
    {
        request.quantity = static_cast<quint8>((info.size) * 2);
        request.sizeBytes = static_cast<quint8>((info.size) * 4);

        for (int i = 0; i < info.size; i++)
        {
            quint32 fl = *(quint32 *)(data + i);
            request.data.append(static_cast<char>(fl >> 8));
            request.data.append(static_cast<char>(fl));
            request.data.append(static_cast<char>(fl >> 24));
            request.data.append(static_cast<char>(fl >> 16));
        }
    }
    SendAndGetResult(request, outp);
    emit CorSignalsWritten();
}

void ModBus::ModReadCor(ModBus::Information info)
{
    Queries::CommandMBS request {
        Queries::CommandsMBS::MBS_READINPUTREGISTER, // Command
        info.adr,                                    // Address
        static_cast<quint8>(info.size * 2),          // Quantity
        static_cast<quint8>(info.size * 4),          // SizeBytes
        {}                                           // Data
    };
    InOutStruct outp {};

    Log->info("ReadCor()");

    SendAndGetResult(request, outp);

    QList<SignalStruct> Sig;
    int sigsize;
    GetFloatSignalsFromByteArray(outp.Ba, info.adr, Sig, sigsize);
    emit CorSignalsReceived(Sig);
}

void ModBus::ReadTime()
{
    Queries::CommandMBS request {
        Queries::CommandsMBS::MBS_READHOLDINGREGISTERS, // Command
        MBS_TIMEREG,                                    // Address
        2,                                              // Quantity
        4,                                              // SizeBytes
        {}                                              // Data
    };
    InOutStruct outp {};

    Log->info("ReadTime()");

    Error::Msg res = SendAndGetResult(request, outp);
    if (res != Error::Msg::NoError)
        emit TimeReadError();
    QList<BSISignalStruct> BSIsig;
    unsigned int sigsize;
    if (GetSignalsFromByteArray(outp.Ba, MBS_TIMEREG, BSIsig, sigsize) != Error::Msg::NoError)
    {
        ERMSG("Ошибка взятия сигнала из очереди по modbus");
        return;
    }
    emit TimeSignalsReceived(BSIsig);
}

Error::Msg ModBus::GetSignalsFromByteArray(
    QByteArray &bain, int startadr, QList<BSISignalStruct> &BSIsig, unsigned int &size)
{
    if (bain.size() < 3)
    {
        Log->error("Wrong inbuf size");
        return Error::Msg::GeneralError;
    }
    unsigned int byteSize = bain.data()[2];
    QByteArray ba = bain.mid(3);
    if (byteSize > static_cast<unsigned int>(ba.size()))
    {
        ERMSG("wrong byte size in response");
        return Error::Msg::GeneralError;
    }
    unsigned int signalsSize = byteSize / 4; // количество байт float или u32
    BSISignalStruct bsi;
    for (unsigned int i = 0; i < signalsSize; ++i)
    {
        quint32 ival = ((ba.data()[2 + 4 * i] << 24) & 0xFF000000) + ((ba.data()[3 + 4 * i] << 16) & 0x00FF0000)
            + ((ba.data()[4 * i] << 8) & 0x0000FF00) + ((ba.data()[1 + 4 * i] & 0x000000FF));
        bsi.Val = *(reinterpret_cast<quint32 *>(&ival));
        bsi.SigAdr = i + startadr;
        BSIsig.append(bsi);
    }
    size = signalsSize;
    return Error::Msg::NoError;
}

Error::Msg ModBus::GetFloatSignalsFromByteArray(QByteArray &bain, int startadr, QList<SignalStruct> &Sig, int &size)
{
    if (bain.size() < 3)
    {
        Log->error("Wrong inbuf size");
        return Error::Msg::GeneralError;
    }
    int byteSize = bain.data()[2];
    QByteArray ba = bain.mid(3);
    if (byteSize > ba.size())
    {
        ERMSG("wrong byte size in response");
        return Error::Msg::GeneralError;
    }
    int signalsSize = byteSize / 4; // количество байт float или u32
    SignalStruct sig;
    for (int i = 0; i < signalsSize; ++i)
    {
        quint32 ival = ((ba.data()[2 + 4 * i] << 24) & 0xFF000000) + ((ba.data()[3 + 4 * i] << 16) & 0x00FF0000)
            + ((ba.data()[4 * i] << 8) & 0x0000FF00) + ((ba.data()[1 + 4 * i] & 0x000000FF));
        sig.flVal = *(reinterpret_cast<float *>(&ival));
        sig.SigAdr = i + startadr;
        Sig.append(sig);
    }
    size = signalsSize;
    return Error::Msg::NoError;
}

void ModBus::WriteTime(uint time)
{

    InOutStruct outp {};
    Log->info("WriteTime()");
    QByteArray timeArray;
    timeArray.append(static_cast<char>(time >> 8));
    timeArray.append(static_cast<char>(time));
    timeArray.append(static_cast<char>(time >> 24));
    timeArray.append(static_cast<char>(time >> 16));
    Queries::CommandMBS request {
        Queries::CommandsMBS::MBS_WRITEMULTIPLEREGISTERS, // Command
        MBS_TIMEREG,                                      // Address
        2,                                                // Quantity
        4,                                                // SizeBytes
        timeArray                                         // Data
    };
    Error::Msg res = SendAndGetResult(request, outp);
    if (res != Error::Msg::NoError)
        emit TimeReadError();
    if (outp.Ba.size() < 5)
    {
        ERMSG("response length incorrect");
        return;
    }
    quint16 startadr = (outp.Ba.data()[1] << 8) + outp.Ba.data()[2];
    if (startadr != MBS_TIMEREG)
    {
        ERMSG("wrong response");
        return;
    }
    emit TimeWritten();
}

void ModBus::Tabs(int index)
{
    /*    if(!TheEnd)
        { */
    if (index == TimeIndex)
    {
        TimePollEnabled = true;
        MainPollEnabled = false;
    }
    else if (index == CorIndex)
    {
        TimePollEnabled = false;
        MainPollEnabled = false;
        Information info;
        info.adr = MBS_INITREG;
        info.size = 11;
        ModReadCor(info);
    }
    else if (index == CheckIndex)
    {
        TimePollEnabled = false;
        MainPollEnabled = true;
    }
    //    }
}

void ModBus::StartPolling()
{
    PollingTimer->start();
}

void ModBus::StopPolling()
{
    PollingTimer->stop();
}

ModbusThread::ModbusThread(QObject *parent) : QObject(parent)
{
    Log = new LogClass;
    Log->Init("modbusprot.log");
    Log->info("=== Log started ===");
    AboutToFinish = false;
}

ModbusThread::~ModbusThread()
{
}

// void ModbusThread::Init(QQueue<ModBus::InOutStruct> *inq, QList<ModBus::InOutStruct> *outl)
//{
//    InQueue = inq;
//    OutList = outl;
//}

void ModbusThread::Run()
{
    while (!AboutToFinish)
    {
        RunMutex.lock();
        RunWC.wait(&RunMutex, 20);
        RunMutex.unlock();
        Queries::Command inp;
        if (DataManager::deQueue(inp) == Error::Msg::NoError)
        //        while (!InQueue->isEmpty())
        //        {
        //            InMutex.lock();
        //            //            ModBus::InOutStruct inp = InQueue->dequeue();
        //            Queries::CommandMBS inp;
        //            if (DataManager::deQueue(inp) == Error::Msg::NoError)
        //                InMutex.unlock();
        //            SendAndGetResult(inp);
        {
            switch (inp.cmd)
            {
            case Queries::CommandsMBS::MBS_READINPUTREGISTER:
            }
        }
        QCoreApplication::processEvents();
    }
    emit Finished();
}

void ModbusThread::SendAndGetResult(ModBus::InOutStruct &inp)
{
    Inp = inp;
    Send();
    Outp.TaskNum = Inp.TaskNum;
    AddToOutQueue(Outp);
}

void ModbusThread::AddToOutQueue(ModBus::InOutStruct &outp)
{
    OutMutex.lock();
    OutList->append(outp);
    OutMutex.unlock();
    OutWC.wakeAll();
}

void ModbusThread::Send()
{
    // data to send is in Inp.Ba
    quint16 KSS = CalcCRC(Inp.Ba);
    Inp.Ba.append(static_cast<char>(KSS >> 8));
    Inp.Ba.append(static_cast<char>(KSS));
    Outp.Ba.clear();
    Inp.Checked = false;
    Busy = true;
    Log->info("-> " + Inp.Ba.toHex());
    //    qint64 st = SerialPort->write(Inp.Ba.data(), Inp.Ba.size());
    emit Write(Inp.Ba);
    /*    if (st < Inp.Ba.size())
        {
            Outp.Res = RESEMPTY;
            return;
        } */
    QElapsedTimer tme;
    tme.start();
    while ((Busy) && (tme.elapsed() < RECONNECTTIME)) // ждём, пока либо сервер не отработает,
                                                      // либо не наступит таймаут
        QCoreApplication::processEvents();
    if (Busy)
    {
        Busy = false;
        Log->error("Timeout");
        Outp.Res = Error::Msg::GeneralError;
    }
}

void ModbusThread::ParseReply(QByteArray ba)
{
    Outp.Ba.append(ba);
    //    Log->info("<- " + Outp.Ba.toHex());
    if ((!Inp.Checked) && (Outp.Ba.size() >= 2))
    {
        Inp.Checked = true;
        if (Outp.Ba.at(1) == (Inp.Command & 0x80)) // error
        {
            Log->error("Error in module");
            ERMSG("modbus error response: " + Outp.Ba.toHex());
        }
        if (Outp.Ba.at(1) != Inp.Command) // wrong response
        {
            Log->error("Wrong response: " + Outp.Ba.toHex());
            ERMSG("modbus wrong command");
        }
    }

    if (Outp.Ba.size() >= Inp.ReadSize)
    {
        Log->info("<- " + Outp.Ba.toHex());
        int rdsize = Outp.Ba.size();

        quint16 crcfinal = (static_cast<quint8>(Outp.Ba.data()[rdsize - 2]) << 8)
            | (static_cast<quint8>(Outp.Ba.data()[rdsize - 1]));
        Outp.Ba.chop(2);
        quint16 MYKSS = CalcCRC(Outp.Ba);

        if (MYKSS != crcfinal)
        {
            Log->error("Crc error");
            ERMSG("modbus crc error");
            //            emit ErrorCrc();
            Outp.Res = Error::Msg::GeneralError;
            return;
        }
        Outp.Res = Error::Msg::NoError;
        Busy = false;
    }
}

void ModbusThread::FinishThread()
{
    AboutToFinish = true;
}

quint16 ModbusThread::CalcCRC(QByteArray &ba)
{
    quint8 CRChi = 0xFF;
    quint8 CRClo = 0xFF;
    quint8 Ind;
    quint16 crc;
    int count = 0;

    while (count < ba.size())
    {
        Ind = CRChi ^ ba.at(count);
        count++;
        CRChi = CRClo ^ TabCRChi[Ind];
        CRClo = TabCRClo[Ind];
    }
    crc = ((CRChi << 8) | CRClo);
    return crc;
}
