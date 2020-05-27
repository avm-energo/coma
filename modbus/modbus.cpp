#include <QThread>
#include <QCoreApplication>
#include <QWidget>
#include <QtTest/QTest>
#include <QtMath>
#include <QTime>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>
#include "../gen/error.h"
#include "../gen/s2.h"
#include "../gen/timefunc.h"
#include "../widgets/emessagebox.h"
#include "../gen/mainwindow.h"
#include "modbus.h"

QMutex RunMutex, InMutex, OutMutex, OutWaitMutex;
QWaitCondition RunWC, OutWC;

ModBus::ModBus(QObject *parent) : QObject(parent)
{
    CycleGroup = 0;
    MainPollEnabled = true;
    TimePollEnabled = InitPollEnabled = false;

    TimeoutTimer = new QTimer;
    TimeoutTimer->setInterval(RECONNECTTIME);
    connect(TimeoutTimer, SIGNAL(timeout()), this, SLOT(Reconnect()));

    PollingTimer = new QTimer;
    PollingTimer->setInterval(POLLINGINTERVAL);
    connect(PollingTimer, SIGNAL(timeout()), this, SLOT(Polling()));

    SignalGroups[0] = QByteArrayLiteral("\x04\x00\x65\x00\x04");
    SignalGroups[1] = QByteArrayLiteral("\x04\x03\xE8\x00\x20");
    SignalGroups[2] = QByteArrayLiteral("\x04\x04\x4c\x00\x20");
    SignalGroups[3] = QByteArrayLiteral("\x04\x09\x60\x00\x0E");
    SignalGroups[4] = QByteArrayLiteral("\x04\x09\x74\x00\x1c");
    SignalGroups[5] = QByteArrayLiteral("\x04\x11\x95\x00\x04");
    SignalGroups[6] = QByteArrayLiteral("\x01\x0b\xc3\x00\x19");
}

ModBus::~ModBus()
{
}

int ModBus::Connect(ModBus_Settings settings)
{
    Settings = settings;
    ModbusThread *cthr = new ModbusThread(Settings);
    cthr->Init(&InQueue, &OutList);
    QThread *thr = new QThread;
    cthr->moveToThread(thr);
    connect(thr,SIGNAL(started()),cthr,SLOT(Run()));
    connect(cthr,SIGNAL(Finished()),thr,SLOT(quit()));
    connect(thr,SIGNAL(finished()),thr,SLOT(deleteLater()));
    connect(cthr,SIGNAL(Finished()),cthr,SLOT(deleteLater()));
    connect(cthr, SIGNAL(ModbusState(ModBus::ModbusDeviceState)), this, SIGNAL(ModbusState(ModBus::ModbusDeviceState)));
    connect(cthr, SIGNAL(Finished()), this, SIGNAL(Finished()));
    thr->start();
    return cthr->State();
    StartPolling();
}

int ModBus::SendAndGetResult(ComInfo &request, InOutStruct &outp)
{
    InOutStruct inp;
    QByteArray bytes;

    inp.Command = request.Command;
    if(request.Command == WRITEMULTIPLEREGISTERS)
        inp.ReadSize = 8;
    else
        inp.ReadSize = 5+2*request.Quantity;
    bytes.append(Settings.Address); // адрес устройства
    bytes.append(request.Command);         //аналоговый выход
    bytes.append(static_cast<char>((request.Address&0xFF00)>>8));
    bytes.append(static_cast<char>(request.Address&0x00FF));
    bytes.append(static_cast<char>((request.Quantity&0xFF00)>>8));
    bytes.append(static_cast<char>(request.Quantity&0x00FF));
    if(request.Command == 0x10)
        bytes.append(static_cast<char>(request.SizeBytes));
    if(request.Data.size())
        bytes.append(request.Data);

    inp.Ba = bytes;
    // wait for an answer or timeout and return result
    SendAndGet(inp, outp);
    if (outp.Res != NOERROR)
        return outp.Res;
    return NOERROR;
}

void ModBus::Polling()
{
    QByteArray bytes;
    InOutStruct inp, outp;

    if (MainPollEnabled)
    {
        inp.Command = SignalGroups[CycleGroup][0];
        bytes.append(Settings.Address); // адрес устройства
        bytes.append(SignalGroups[CycleGroup]);
        if(CycleGroup == 6)
            inp.ReadSize = 9;
        else
            inp.ReadSize = 5+2*SignalGroups[CycleGroup][SECONDBYTEQ];

        inp.Ba = bytes;
        // wait for an answer or timeout and return result
        SendAndGet(inp, outp);

        if(CycleGroup == 6)
        {
            Coils *coil = new Coils;
            coil->countBytes = outp.Ba.data()[2];
            coil->Bytes = outp.Ba.mid(3);
            emit CoilSignalsReady(coil);
        }
        else
        {
            ModBusSignalStruct *Sig = nullptr;
            int sigsize;
            int startadr = (static_cast<quint8>(SignalGroups[CycleGroup][FIRSTBYTEADR]) << 8) | (static_cast<quint8>(SignalGroups[CycleGroup][SECONDBYTEADR]));
            GetFloatSignalsFromByteArray(outp.Ba, startadr, Sig, sigsize);
            emit SignalsReceived(Sig, sigsize);
        }
        CycleGroup++;
        if (CycleGroup > 6)
            CycleGroup = 0;
    }
    if (TimePollEnabled)
    {
        ReadTime();
    }
    if (InitPollEnabled)
    {
        Information info;
        info.adr = INITREG;
        info.size = 22;
        ModReadCor(info);
    }
}

void ModBus::Finish()
{
    StopPolling();
    emit FinishModbusThread();
}

void ModBus::SendAndGet(InOutStruct &inp, ModBus::InOutStruct &outp)
{
    QElapsedTimer tmetimeout;

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
            return;
        OutWaitMutex.lock();
        OutWC.wait(&OutWaitMutex, 20);
        OutWaitMutex.unlock();
        if (tmetimeout.elapsed() > RECONNECTTIME)
        {
            outp.Res = GENERALERROR;
            Finished = true;
        }
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
}

bool ModBus::GetResultFromOutQueue(int index, ModBus::InOutStruct &outp)
{
    OutMutex.lock();
    if (!OutList.isEmpty())
    {
        for (int i=0; i<OutList.size(); ++i)
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
    ComInfo request;
    InOutStruct outp;

    request.Command = READINPUTREGISTER;
    request.Address = BSIREG; // BSI block
    request.Quantity = 30;
    request.SizeBytes = 60;

    int res = SendAndGetResult(request, outp);
    if (res != NOERROR)
        emit TimeReadError();

    ModBusBSISignalStruct *BSIsig = nullptr;
    int sigsize;
    if (GetSignalsFromByteArray(outp.Ba, BSIREG, BSIsig, sigsize) != NOERROR)
        return;
    emit BsiFromModbus(BSIsig, sigsize);
}

void ModBus::ModWriteCor(Information* info, float* data)//, int* size)
{
    ComInfo request;
    InOutStruct outp;

    request.Command = WRITEMULTIPLEREGISTERS;
    request.Address = info->adr;

    if((info->adr == 900) || (info->adr == 901)) // set initial values or clear initial values commands
    {
        request.Quantity = 1;
        request.SizeBytes = 2;
        request.Data = QByteArrayLiteral("\x01\x01");
    }
    else
    {
        request.Quantity = (quint8)((info->size)*2);
        request.SizeBytes = (quint8)((info->size)*4);

        for (int i = 0; i<info->size; i++)
        {
            quint32 fl =*(quint32*)(data+i);
            request.Data.append(static_cast<char>(fl>>8));
            request.Data.append(static_cast<char>(fl));
            request.Data.append(static_cast<char>(fl>>24));
            request.Data.append(static_cast<char>(fl>>16));
        }
    }
    SendAndGetResult(request, outp);
}

void ModBus::ModReadCor(Information &info)
{
    ComInfo request;
    InOutStruct outp;

    request.Command = READINPUTREGISTER;
    request.Address = info.adr;
    request.Quantity = (quint8)((info.size)*2);
    request.SizeBytes = (quint8)((info.size)*4);
    SendAndGetResult(request, outp);

    ModBusSignalStruct *Sig = nullptr;
    int sigsize;
    GetFloatSignalsFromByteArray(outp.Ba, info.adr, Sig, sigsize);
    emit CorSignalsReceived(Sig, sigsize);
}

void ModBus::ReadTime()
{
    ComInfo request;
    InOutStruct outp;

    request.Address = TIMEREG;
    request.Command = READHOLDINGREGISTERS;
    request.Quantity = 2;
    request.SizeBytes = 4;
    int res = SendAndGetResult(request, outp);
    if (res != NOERROR)
        emit TimeReadError();
    ModBusBSISignalStruct *BSIsig = nullptr;
    int sigsize;
    if (GetSignalsFromByteArray(outp.Ba, TIMEREG, BSIsig, sigsize) != NOERROR)
        return;
    emit TimeSignalsReceived(BSIsig);
}

int ModBus::GetSignalsFromByteArray(QByteArray &bain, int startadr, ModBusBSISignalStruct *BSIsig, int &size)
{
    int byteSize = bain.data()[2];
    QByteArray ba = bain.mid(3);
    if (byteSize > ba.size())
    {
        ERMSG("wrong byte size in response");
        return GENERALERROR;
    }
    int signalsSize = byteSize / 4; // количество байт float или u32
    BSIsig = new ModBusBSISignalStruct[signalsSize];
    for (int i=0; i<signalsSize; ++i)
    {
        quint32 ival = ((ba.data()[2+4*i]<<24)&0xFF000000)+((ba.data()[3+4*i]<<16)&0x00FF0000)+\
                ((ba.data()[4*i]<<8)&0x0000FF00)+((ba.data()[1+4*i]&0x000000FF));
        BSIsig[i].Val = *(reinterpret_cast<quint32*>(&ival));
        BSIsig[i].SigAdr = i+startadr;
    }
    size = signalsSize;
    return NOERROR;
}

int ModBus::GetFloatSignalsFromByteArray(QByteArray &bain, int startadr, ModBusSignalStruct *Sig, int &size)
{
    int byteSize = bain.data()[2];
    QByteArray ba = bain.mid(3);
    if (byteSize > ba.size())
    {
        ERMSG("wrong byte size in response");
        return GENERALERROR;
    }
    int signalsSize = byteSize / 4; // количество байт float или u32
    Sig = new ModBusSignalStruct[signalsSize];
    for (int i=0; i<signalsSize; ++i)
    {
        quint32 ival = ((ba.data()[2+4*i]<<24)&0xFF000000)+((ba.data()[3+4*i]<<16)&0x00FF0000)+\
                ((ba.data()[4*i]<<8)&0x0000FF00)+((ba.data()[1+4*i]&0x000000FF));
        Sig[i].flVal = *(reinterpret_cast<float*>(&ival));
        Sig[i].SigAdr = i+startadr;
    }
    size = signalsSize;
    return NOERROR;
}

void ModBus::WriteTime(uint time)
{
    ComInfo request;
    InOutStruct outp;

    request.Address = TIMEREG;
    request.Command = WRITEMULTIPLEREGISTERS;
    request.Quantity = 2;
    request.SizeBytes = 4;
    request.Data.append(static_cast<char>(time >> 8));
    request.Data.append(static_cast<char>(time));
    request.Data.append(static_cast<char>(time >> 24));
    request.Data.append(static_cast<char>(time >> 16));
    int res = SendAndGetResult(request, outp);
    if (res != NOERROR)
        emit TimeReadError();
    if (outp.Ba.size() < 5)
    {
        ERMSG("response length incorrect");
        return;
    }
    quint16 startadr = (outp.Ba.data()[1] << 8) + outp.Ba.data()[2];
    if (startadr != TIMEREG)
    {
        ERMSG("wrong response");
        return;
    }
    emit
}

void ModBus::Tabs(int index)
{
    if(!MainWindow::TheEnd)
    {
        if(index == TimeIndex)
        {
            TimePollEnabled = true;
            MainPollEnabled = false;
            InitPollEnabled = false;
        }
        else if(index == CorIndex)
        {
            TimePollEnabled = false;
            MainPollEnabled = false;
            InitPollEnabled = true;
        }
        else if(index == CheckIndex)
        {
            TimePollEnabled = false;
            MainPollEnabled = true;
            InitPollEnabled = false;
        }
    }
}

void ModBus::StartPolling()
{
    PollingTimer->start();
}

void ModBus::StopPolling()
{
    PollingTimer->stop();
}

void ModBus::Reconnect()
{
   TimeoutTimer->stop();
   emit ReconnectSignal();
}

ModbusThread::ModbusThread(ModBus::ModBus_Settings settings, QObject *parent) : QObject(parent)
{
    AboutToFinish = false;
    SerialPort = new QSerialPort(settings.Port);
    SerialPort->setBaudRate(settings.Baud);
    SerialPort->setDataBits(QSerialPort::Data8);
    SerialPort->setParity(QSerialPort::NoParity);
    SerialPort->setStopBits(QSerialPort::OneStop);
    SerialPort->setFlowControl(QSerialPort::NoFlowControl);
    SerialPort->setReadBufferSize(1024);
    connect(SerialPort, SIGNAL(readyRead()), this, SLOT(ParseReply()));

    if(SerialPort->open(QIODevice::ReadWrite) == true)
    {
        _state = ModBus::ModbusDeviceState::ConnectedState;
        emit ModbusState(ModBus::ModbusDeviceState::ConnectedState);
    }
    else
    {
        _state = ModBus::ModbusDeviceState::ClosingState;
        emit ModbusState(ModBus::ModbusDeviceState::ClosingState);
    }

}

ModbusThread::~ModbusThread()
{
    if (SerialPort->isOpen())
        SerialPort->close();
}

ModBus::ModbusDeviceState ModbusThread::State()
{
    return _state;
}

void ModbusThread::Init(QQueue<ModBus::InOutStruct> *inq, QList<ModBus::InOutStruct> *outl)
{
    InQueue = inq;
    OutList = outl;
}

void ModbusThread::Run()
{
    while (!AboutToFinish)
    {
        RunMutex.lock();
        RunWC.wait(&RunMutex, 20);
        RunMutex.unlock();
        while (!InQueue->isEmpty())
        {
            InMutex.lock();
            ModBus::InOutStruct inp = InQueue->dequeue();
            InMutex.unlock();
            SendAndGetResult(inp);
        }
    }
    if (SerialPort->isOpen())
        SerialPort->close();
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
    Inp.Ba.append(static_cast<unsigned char>(KSS>>8));
    Inp.Ba.append(static_cast<unsigned char>(KSS));
    if (!SerialPort->isOpen())
        SerialPort->open(QIODevice::ReadWrite);
    Outp.Ba.clear();
    Inp.Checked = false;
    Busy = true;
    qint64 st = SerialPort->write(Inp.Ba.data(), Inp.Ba.size());
    if (st < Inp.Ba.size())
    {
        Outp.Res = RESEMPTY;
        return;
    }
    QElapsedTimer tme;
    tme.start();
    while ((Busy) && (tme.elapsed() < RECONNECTTIME)) // ждём, пока либо сервер не отработает, либо не наступит таймаут
        QThread::msleep(MAINSLEEPCYCLETIME);
    if (Busy)
        Outp.Res = GENERALERROR;
}

void ModbusThread::ParseReply()
{
    qint64 cursize = SerialPort->bytesAvailable();
    Outp.Ba.append(SerialPort->read(cursize));
    if ((!Inp.Checked) && (Outp.Ba.size() >= 2))
    {
        Inp.Checked = true;
        if (Outp.Ba.at(1) == (Inp.Command & 0x80)) // error
            ERMSG("modbus error response");
        if (Outp.Ba.at(1) != Inp.Command) // wrong response
            ERMSG("modbus wrong command");
    }

    if(cursize >= Inp.ReadSize)
    {
        int rdsize = Outp.Ba.size();

        quint16 MYKSS = CalcCRC(Outp.Ba);
        quint16 crcfinal = (static_cast<quint8>(Outp.Ba.data()[rdsize-2]) << 8) | (static_cast<quint8>(Outp.Ba.data()[rdsize-1]));

        if(MYKSS != crcfinal)
        {
            ERMSG("modbus crc error");
//            emit ErrorCrc();
            Outp.Res = GENERALERROR;
            return;
        }
        Outp.Res = NOERROR;
        Busy = false;
    }
}

void ModbusThread::FinishThread()
{
    AboutToFinish = true;
}

quint16 ModbusThread::CalcCRC(QByteArray &ba)
{
  quint8 CRChi=0xFF;
  quint8 CRClo=0xFF;
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
