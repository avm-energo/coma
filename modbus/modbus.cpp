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

ModBus::ModBus(ModBus_Settings settings, QObject *parent) : QObject(parent)
{
    Settings = settings;
    CycleGroup = 0;
    ReadSize = 0;
    Reading = false;
//    CloseThr = false;
//    Commands = false;
    First = 1;
    Count = 0;
    Write = 0;
    AboutToFinish = false;
    ReconnectTimer = new QTimer;
    ReconnectTimer->setInterval(RECONNECTIME);

    connect(ReconnectTimer, SIGNAL(timeout()), this, SLOT(Reconnect()));

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
    if (SerialPort->isOpen())
        SerialPort->close();
}

int ModBus::Connect()
{
    SerialPort = new QSerialPort(Settings.Port);
    SerialPort->setBaudRate(Settings.Baud);
    SerialPort->setDataBits(QSerialPort::Data8);
    SerialPort->setParity(QSerialPort::NoParity);
    SerialPort->setStopBits(QSerialPort::OneStop);
    SerialPort->setFlowControl(QSerialPort::NoFlowControl);
    SerialPort->setReadBufferSize(1024);
    connect(SerialPort, SIGNAL(readyRead()), this, SLOT(ReadPort()));

    if(SerialPort->open(QIODevice::ReadWrite) == true)
    {
       State = ConnectedState;
       emit ModbusState(State);
    }
    else
    {
       State = ClosingState;
       emit ModbusState(State);
       return GENERALERROR;
    }
    return NOERROR;
}

int ModBus::SendAndGetResult(ModBus::ComInfo &request)
{
    QByteArray bytes;

    if (PrepareWrite() != NOERROR)
        return GENERALERROR;
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
    quint16 KSS = CalcCRC(bytes);
    bytes.append(static_cast<unsigned char>(KSS>>8));
    bytes.append(static_cast<unsigned char>(KSS));
    if(request.Command== 0x10)
        ReadSize = 8;
    else
        ReadSize = 5+2*request.Quantity;
    qint64 st = SerialPort->write(bytes.data(), bytes.size());
    if (st < bytes.size())
        return RESEMPTY;
    if(request.Address == 4600)
        Write=0;
    // wait for an answer ot timeout and return result
    return NOERROR;
}

int ModBus::SendNextCmdAndGetResult()
{
    QByteArray bytes;

    if (PrepareWrite() != NOERROR)
        return GENERALERROR;
    bytes.append(Settings.Address); // адрес устройства
    bytes.append(SignalGroups[CycleGroup]);
    quint16 KSS = CalcCRC(bytes);
    bytes.append(static_cast<unsigned char>(KSS>>8));
    bytes.append(static_cast<unsigned char>(KSS));

    ReadSize = 5+2*SignalGroups[CycleGroup][SECONDBYTEQ];

    if(CycleGroup == 6)
        ReadSize = 9;
    st = SerialPort->write(bytes.data(), bytes.size());
}

void ModBus::ReadPort()
{
  quint16 MYKSS = '\0', crcfinal = '\0';
  QString crc = nullptr;
  quint8 size;
  qint64 cursize;
  int i = 0, startadr, signalsSize;
  int ival;
  Coils *Coil = new Coils;
  ModBusBSISignalStruct *BSIsig = new ModBusBSISignalStruct;
  //ModBusInterrogateTimer->stop();
  ResponseBuffer = *new QByteArray[1024];
  ResponseBuffer.clear();
  Reading = 1;

  cursize = SerialPort->bytesAvailable();

  if(ComData.adr != 1)
  {
     ReconnectTimer->stop();
     ReconnectTimer->setInterval(RECONNECTIME);
     ReconnectTimer->start();
  }

  if(cursize == ReadSize)
  {
      ResponseBuffer = SerialPort->read(cursize);//serialPort->bytesAvailable());
      size = ResponseBuffer.size();
      size -= 2;

      MYKSS=CalcCRC((quint8*)(ResponseBuffer.data()), size);

      crcfinal = (static_cast<quint8>(ResponseBuffer.data()[ReadSize-2]) << 8) | (static_cast<quint8>(ResponseBuffer.data()[ReadSize-1]));

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
             ReconnectTimer->start();
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

           CycleGroup++;

           if(CycleGroup == 7)
           CycleGroup= 0;

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
          CycleGroup++;

          if(CycleGroup == 7)
          CycleGroup= 0;

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

quint16 ModBus::CalcCRC(QByteArray &ba)
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

int ModBus::PrepareWrite()
{
    if(!SerialPort->isOpen())
    {
        if (Connect() != NOERROR)
            return GENERALERROR;
    }

    SerialPort->clear();
    TimeFunc::Wait(10);
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

void ModBus::Reconnect()
{
   ReconnectTimer->stop();
   ReconnectTimer->deleteLater();
   emit ReconnectSignal(1);
}
