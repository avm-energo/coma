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
#include <QWaitCondition>
#include <QLineEdit>
#include <QMessageBox>
#include "../gen/error.h"
#include "../gen/s2.h"
#include "../gen/timefunc.h"
#include "../widgets/emessagebox.h"
#include "../gen/mainwindow.h"
#include "modbus.h"

bool ModBus::Reading;
QMutex RunMutex, InMutex, OutMutex, OutWaitMutex;
QWaitCondition RunWC, OutWC;

ModBus::ModBus(ModBus_Settings settings, QObject *parent) : QObject(parent)
{
    Settings = settings;
    CycleGroup = 0;
    Reading = false;
//    CloseThr = false;
//    Commands = false;
    First = 1;
    Count = 0;
    Write = 0;
    PollingEnabled = false;

    TimeoutTimer = new QTimer;
    TimeoutTimer->setInterval(RECONNECTTIME);
    connect(TimeoutTimer, SIGNAL(timeout()), this, SLOT(Reconnect()));

    PollingTimer = new QTimer;
    PollingTimer->setInterval(POLLINGINTERVAL);
    connect(PollingTimer, SIGNAL(timeout()), this, SLOT(SendNextCmdAndGetResult()));

    SignalGroups[0] = QByteArrayLiteral("\x04\x00\x65\x00\x04");
    SignalGroups[1] = QByteArrayLiteral("\x04\x03\xE8\x00\x20");
    SignalGroups[2] = QByteArrayLiteral("\x04\x04\x4c\x00\x20");
    SignalGroups[3] = QByteArrayLiteral("\x04\x09\x60\x00\x0E");
    SignalGroups[4] = QByteArrayLiteral("\x04\x09\x74\x00\x1c");
    SignalGroups[5] = QByteArrayLiteral("\x04\x11\x95\x00\x04");
    SignalGroups[6] = QByteArrayLiteral("\x01\x0b\xc3\x00\x19");

    //ModBusInterrogateTimer = new QTimer;
    //ModBusInterrogateTimer->setInterval(1000);
}

ModBus::~ModBus()
{
}

int ModBus::Connect()
{
    ModbusThread *cthr = new ModbusThread(Settings);
    cthr->Init(&InQueue, &OutList);
    QThread *thr = new QThread;
    cthr->moveToThread(thr);
    connect(thr,SIGNAL(started()),cthr,SLOT(Run()));
    connect(cthr,SIGNAL(Finished()),thr,SLOT(quit()));
    connect(thr,SIGNAL(finished()),thr,SLOT(deleteLater()));
    connect(cthr,SIGNAL(Finished()),cthr,SLOT(deleteLater()));
    connect(cthr, SIGNAL(ModbusState(ModBus::ModbusDeviceState)), this, SIGNAL(ModbusState(ModBus::ModbusDeviceState)));
    thr->start();
    return cthr->State();
}

int ModBus::SendAndGetResult(ModBus::ComInfo &request)
{
    InOutStruct inp, outp;
    QByteArray bytes;

    if(request.Command== 0x10)
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

    if(request.Address == 4600)
        Write=0;
    return NOERROR;
}

void ModBus::SendNextCmdAndGetResult()
{
    QByteArray bytes;
    InOutStruct inp, outp;

    bytes.append(Settings.Address); // адрес устройства
    bytes.append(SignalGroups[CycleGroup]);
    if(CycleGroup == 6)
        inp.ReadSize = 9;
    else
        inp.ReadSize = 5+2*SignalGroups[CycleGroup][SECONDBYTEQ];

    inp.Ba = bytes;
    // wait for an answer or timeout and return result
    SendAndGet(inp, outp);

    CycleGroup++;
    if (CycleGroup > 6)
        CycleGroup = 0;
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

void ModBus::BSIrequest(ModBus_Settings Settings)
{
    QByteArray* bytes = new QByteArray[100];
    quint16 KSS = '\0';
    qint64 st;

    ComData.ModCom = 0x04;
    ComData.adr = 0x01;
    ComData.quantity = 30;
    ComData.sizebytes = 60;
    //if( ComData.data.size())
    //ComData.data.clear();

    Commands = true;
    bytes->clear();
    SerialPort->clear();

    bytes->append(Settings.Address.toInt()); // адрес устройства

    bytes->append(ComData.ModCom);         //аналоговый выход
    bytes->append(static_cast<char>(ComData.adr>>8));
    bytes->append(static_cast<char>(ComData.adr));
    bytes->append(static_cast<char>(ComData.quantity>>8));
    bytes->append(static_cast<char>(ComData.quantity));

    /*        if(ComData.data.size())
                {
                    for(i = 0; i<ComData.data.size(); i++)
                    {
                      bytes->append(ComData.data.data()[i]);
                    }
                }
*/

                //for(i=0;i<bytes->size();i++)
     KSS=CalcCRC((quint8*)(bytes->data()), bytes->size());
                //KSS += static_cast<quint8>(bytes->data()[i]);
                //S2::updCRC32(bytes->data()[i],&crc);
     bytes->append(static_cast<char>(KSS>>8));
     bytes->append(static_cast<char>(KSS));

     ReadSize = 5+2*ComData.quantity;

     st = SerialPort->write(bytes->data(), bytes->size());
            //st=0;

     Reading = true;

}

void ModBus::StopModSlot()
{
    if(SerialPort != nullptr)
    SerialPort->close();

    Reading = true;
    CloseThr = true;
}

void ModBus::ModWriteCor(Information* info, float* data)//, int* size)
{
    int i;
    quint32 fl;
    //bool readingState = Reading;

    while(Reading == true)
    {
        TimeFunc::Wait(10);
        Count++;
        if(Count == 20)
        {
          Reading = false;
          Count = 0;
        }
    }

    ComData.ModCom = 0x10;
    ComData.adr = info->adr;

    if((info->adr == 900) || (info->adr == 905))
    {
        ComData.quantity = 1;
        ComData.sizebytes = 2;
        ComData.data->append(0x01);
        ComData.data->append(0x01);
    }
    else
    {
        ComData.quantity = (quint8)((info->size)*2);
        ComData.sizebytes = (quint8)((info->size)*4);

        for(i = 0; i<info->size; i++)
        {
         fl =*(quint32*)(data+i);
         ComData.data->append(static_cast<char>(fl>>8));
         ComData.data->append(static_cast<char>(fl));
         ComData.data->append(static_cast<char>(fl>>24));
         ComData.data->append(static_cast<char>(fl>>16));
        }
    }
    //TimeFunc::Wait(100);
    Commands = true;
    Reading = false;

}

void ModBus::ModReadCor(Information* info)
{

    while(Reading == true)
    {
        TimeFunc::Wait(10);
        Count++;
        if(Count == 20)
        {
          Reading = false;
          Count = 0;
        }
    }

    ComData.ModCom = 0x04;
    ComData.adr = info->adr;
    ComData.quantity = (quint8)((info->size)*2);
    ComData.sizebytes = (quint8)((info->size)*4);

    ComData.data = new QByteArray[100];
    ComData.data->clear();

    //TimeFunc::Wait(100);
    Commands = true;
    Reading = false;

}

void ModBus::InterrogateTime()
{
    if(First)
    {
      First = 0;
      Reading = false;
    }


    while(Reading == true)
    {
        TimeFunc::Wait(10);
        Count++;
        if(Count == 20)
        {
          Reading = false;
          Count = 0;
        }
    }

    if(!Write)
    {
      ComData.ModCom = 0x03;
      ComData.adr = 4600;
      ComData.quantity = 2;
      ComData.sizebytes = 4;
      ComData.data = new QByteArray[100];
      ComData.data->clear();
      //TimeFunc::Wait(100);
      Commands = true;
      Reading = false;
    }


}

void ModBus::WriteTime(uint* time)
{
    Write = 1;
    quint32 T;
    //ComData = *new ComInfo;

    while(Reading == true)
    {
        TimeFunc::Wait(10);
        Count++;
        if(Count == 20)
        {
          Reading = false;
          Count = 0;
        }
    }

    ComData.ModCom = 0x10;
    ComData.adr = 4600;

    ComData.quantity = 2;
    ComData.sizebytes = 4;

    ComData.data = new QByteArray[100];
    ComData.data->clear();

    T =*time;
    ComData.data->append(static_cast<char>(T>>8));
    ComData.data->append(static_cast<char>(T));
    ComData.data->append(static_cast<char>(T>>24));
    ComData.data->append(static_cast<char>(T>>16));

    //TimeFunc::Wait(100);
    Commands = true;
    Reading = false;

}

void ModBus::Tabs(int index)
{

  if(!MainWindow::TheEnd)
  {
    while(Reading == true)
    {
        TimeFunc::Wait(10);
        Count++;
        if(Count == 200)
        {
          Reading = false;
          Count = 0;
        }
    }

   if(index == TimeIndex)
   {
       TimeFunc::Wait(1000);
       ComData.ModCom = 0x03;
       ComData.adr = 4600;
       ComData.quantity = 2;
       ComData.sizebytes = 4;
       ComData.data = new QByteArray[100];
       ComData.data->clear();
         //TimeFunc::Wait(100);
       Commands = true;
       //Reading = false;
   }
   else if(index == CorIndex)
   {
       ComData.ModCom = 0x04;
       ComData.adr = 4000;
       ComData.quantity = 22;
       ComData.sizebytes = 44;

       ComData.data = new QByteArray[100];
       ComData.data->clear();

       //TimeFunc::Wait(100);
       Commands = true;
       //Reading = false;
   }
   else if(index == CheckIndex)
   {
       ComData.adr = 0;
       Commands = false;
   }

   Reading = false;
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
        emit ModbusState(ModBus::ModbusDeviceState::ConnectedState);
    else
        emit ModbusState(ModBus::ModbusDeviceState::ClosingState);

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

int ModbusThread::SendAndGetResult(ModBus::InOutStruct &inp)
{
    Inp = inp;
    Send();
    Outp.TaskNum = Inp.TaskNum;
    AddToOutQueue(Outp);
}

void ModbusThread::Send()
{
    // data to send is in Inp.Ba
    quint16 KSS = CalcCRC(Inp.Ba);
    Inp.Ba.append(static_cast<unsigned char>(KSS>>8));
    Inp.Ba.append(static_cast<unsigned char>(KSS));
    if (!SerialPort->isOpen())
        SerialPort->open(QIODevice::ReadWrite);
    ReadData.clear();
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
    else
        Outp.Res = NOERROR;
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

void ModbusThread::AddToOutQueue(ModBus::InOutStruct &outp)
{
    OutMutex.lock();
    OutList->append(outp);
    OutMutex.unlock();
    OutWC.wakeAll();
}

void ModbusThread::ParseReply()
{
    quint16 MYKSS = '\0', crcfinal = '\0';
    QString crc = nullptr;
    int i = 0, startadr, signalsSize;
    int ival;
    Coils *Coil = new Coils;
    ModBusBSISignalStruct *BSIsig = new ModBusBSISignalStruct;

    qint64 cursize = SerialPort->bytesAvailable();
    ReadData.append(SerialPort->read(cursize));
    if (ReadData.size() >= 5)
    {

    }

    if(cursize >= Inp.ReadSize)
    {
        int rdsize = ReadData.size();

        MYKSS = CalcCRC(ReadData);
        crcfinal = (static_cast<quint8>(ReadData.data()[rdsize-2]) << 8) | (static_cast<quint8>(ReadData.data()[rdsize-1]));

        if(MYKSS == crcfinal)
        {

            if((CycleGroup == 6) && (Commands == false))
        {
           Coil = new Coils;
           Coil->countBytes = ResponseBuffer.data()[2];
           signalsSize = 0;
           for(i=0; i<Coil->countBytes; i++)
           {
             Coil->Bytes[i] = ResponseBuffer.data()[3+i];
           }
        }
        else
        signalsSize = ResponseBuffer.data()[2]/4; // количество байт float или u32

        if(ComData.adr == 1 || ComData.adr == 4600)
        BSIsig = new ModBusBSISignalStruct[signalsSize];
        else
        Sig = new ModBusSignalStruct[signalsSize];

        startadr = (static_cast<quint8>(SignalGroups[CycleGroup].data[FIRSTBYTEADR]) << 8) | (static_cast<quint8>(SignalGroups[CycleGroup].data[SECONDBYTEADR]));
        for(i=0; i<signalsSize; i++)
        {
         if(ComData.adr == 1  || ComData.adr == 4600)  // bsi
         ival = (((static_cast<quint8>(ResponseBuffer.data()[5+4*i])<<24)&0xFF000000)+((static_cast<quint8>(ResponseBuffer.data()[6+4*i])<<16)&0x00FF0000)+((static_cast<quint8>(ResponseBuffer.data()[3+4*i])<<8)&0x0000FF00)+(static_cast<quint8>(ResponseBuffer.data()[4+4*i])&0x000000FF));
         else
         {
           /*bool ok;
           QString fl = *new QString[1024];
           QStringList Listfl = *new QStringList[4];
           fl.clear();
           float tmpf;
           fl.append(responseBuffer.data()[5+4*i] & 0x00FF);
           fl.append(responseBuffer.data()[6+4*i] & 0x00FF);
           fl.append(responseBuffer.data()[3+4*i] & 0x00FF);
           fl.append(responseBuffer.data()[4+4*i] & 0x00FF);
           Listfl.append(fl.at(0));
           Listfl.append(fl.at(1));
           Listfl.append(fl.at(2));
           Listfl.append(fl.at(3));
           fl = Listfl.join("");
           tmpf = fl.toFloat(&ok);
           Sig[i].flVal = tmpf;*/
           ival = ((ResponseBuffer.data()[5+4*i]<<24)&0xFF000000)+((ResponseBuffer.data()[6+4*i]<<16)&0x00FF0000)+((ResponseBuffer.data()[3+4*i]<<8)&0x0000FF00)+((ResponseBuffer.data()[4+4*i]&0x000000FF));
           Sig[i].flVal = *(float*)&ival;
         }

         if(Commands)
         {
             if(ComData.adr == 1 || ComData.adr == 4600)  // bsi и time
             {
                 BSIsig[i].Val = *(quint32*)&ival;
                 BSIsig[i].SigAdr = ComData.adr+i;
             }
             else
             Sig[i].SigAdr = ComData.adr+i;

         }
         else
         Sig[i].SigAdr = startadr+i;
        }

        if(Commands)
        {
           if(ComData.adr>=900 && ComData.adr<=910)
           {
               int recordSize = (static_cast<quint8>(ResponseBuffer.data()[4]) << 8) | (static_cast<quint8>(ResponseBuffer.data()[5]));
               if(recordSize == ComData.quantity)
               signalsSize = 1;
               TimeFunc::Wait(100);

           }

           if(ComData.adr == 1)
           {
             ComData.adr = 0;
             emit BsiFromModbus(BSIsig, &signalsSize);
             Commands = false;
             TimeoutTimer->start();
           }
           else if(ComData.adr == 4600)
           emit TimeSignalsReceived(BSIsig);
           else
           {
             Commands = false;
             emit CorSignalsReceived(Sig, &signalsSize);
           }

           //commands = false;
           //ComData.adr = 0;
        }
        else
        {

           if(CycleGroup == 6)
           emit CoilSignalsReady(Coil);
           else
           emit SignalsReceived(Sig, &signalsSize);


           //Reading = 0;
        }
      }
      else
      {
        emit ErrorCrc();
      }

  }
  else
  {
      if((cursize == 5) && (ComData.adr != 1))
      {
          //commands = false;
          Reading = false;

          ResponseBuffer = SerialPort->read(cursize);
          if((ResponseBuffer.data()[3] == (char)0xC2) && (ResponseBuffer.data()[4] == (char)0xC1))
          {
             if(ComData.adr == 4600)
             emit TimeReadError();
             else
             emit ErrorRead();
          }
          //ComData.adr = 0;
      }
  }

 // emit nextGroup();

}
