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
#include "../gen/s2.h"
#include "../gen/timefunc.h"
#include "../widgets/emessagebox.h"
#include "../gen/mainwindow.h"
#include "modbus.h"

bool ModBus::Reading;

ModBus::ModBus(ModBus_Settings Settings, QObject *parent) : QObject(parent)
{
    //modbusDevice = new QModbusRtuSerialMaster(this);
    //Reply = new  QModbusReply;
    //state = *new QModbusDevice::State;
    //modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
    //                ui->portEdit->text());

    /*modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, Settings.parity.toInt());
    modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, Settings.baud.toInt());
    modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, Settings.stop.toInt());
    modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, 8);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, Settings.port);
    */
    //modbusDevice->setTimeout(m_settingsDialog->settings().responseTime);  // для слэйва не надо
    //modbusDevice->setNumberOfRetries(m_settingsDialog->settings().numberOfRetries);

    //connect(modbusDevice,  SIGNAL(QModbusDevice::stateChanged(QModbusDevice::State state)), parent, SLOT(onModbusStateChanged(state)));
    DeviceAdr = Settings.adr;
    Group = 0;
    ReadSize = 0;
    Reading = false;
    CloseThr = false;
    int i;
    Commands = false;
    First = 1;
    Count = 0;
    Write = 0;
    AboutToFinish = false;
    ReconnectTimer = new QTimer;
    ReconnectTimer->setInterval(RECONNECTIME);

    connect(ReconnectTimer, SIGNAL(timeout()), this, SLOT(Reconnect()));

    for(i=0;i<6;i++)
        SignalGroups[i].data[SIGNALTYPE]=0x04;
//        SignalGroups[i].signaltype=0x04;

    SignalGroups[0] = {0x04, 0x00, 0x65, 0x00, 0x04};
/*    SignalGroups[0].firstbyteadr = 0;
    SignalGroups[0].secondbyteadr = 0x65;
    SignalGroups[0].firstbytequantity = 0;
    SignalGroups[0].secondbytequantity = 0x04; */
    SignalGroups[1] = {0x04, 0x03, 0xE8, 0x00, 0x20};
/*    SignalGroups[1].firstbyteadr = 0x03;
    SignalGroups[1].secondbyteadr = 0xE8;
    SignalGroups[1].firstbytequantity = 0;
    SignalGroups[1].secondbytequantity = 0x20; */
    /*SignalGroups[2].firstbyteadr = 0x03;
    SignalGroups[2].secondbyteadr = 0xFC;
    SignalGroups[2].firstbytequantity = 0;
    SignalGroups[2].secondbytequantity = 0x0A;*/
    SignalGroups[2] = {0x04, 0x04, 0x4C, 0x00, 0x20};
/*    SignalGroups[2].firstbyteadr = 0x04;
    SignalGroups[2].secondbyteadr = 0x4C;
    SignalGroups[2].firstbytequantity = 0;
    SignalGroups[2].secondbytequantity = 0x20; */
    SignalGroups[3] = {0x04, 0x09, 0x60, 0x00, 0x0E};
/*    SignalGroups[3].firstbyteadr = 0x09;
    SignalGroups[3].secondbyteadr = 0x60;
    SignalGroups[3].firstbytequantity = 0;
    SignalGroups[3].secondbytequantity = 0x0E; */
    SignalGroups[4] = {0x04, 0x09, 0x74, 0x00, 0x1C};
/*    SignalGroups[4].firstbyteadr = 0x09;
    SignalGroups[4].secondbyteadr = 0x74;
    SignalGroups[4].firstbytequantity = 0;
    SignalGroups[4].secondbytequantity = 0x1C; */
    SignalGroups[5] = {0x04, 0x11, 0x95, 0x00, 0x04};
/*    SignalGroups[5].firstbyteadr = 0x11;
    SignalGroups[5].secondbyteadr = 0x95;
    SignalGroups[5].firstbytequantity = 0;
    SignalGroups[5].secondbytequantity = 0x04; */
    /*SignalGroups[7].firstbyteadr = 0x04;
    SignalGroups[7].secondbyteadr = 0x60;
    SignalGroups[7].firstbytequantity = 0;
    SignalGroups[7].secondbytequantity = 0x0A;*/
    SignalGroups[6] = {0x01, 0x0B, 0xC3, 0x00, 0x19};
/*    SignalGroups[6].signaltype=0x01;
    SignalGroups[6].firstbyteadr = 0x0B;
    SignalGroups[6].secondbyteadr = 0xC3;
    SignalGroups[6].firstbytequantity = 0;
    SignalGroups[6].secondbytequantity = 0x19; */

    //connect(this,  SIGNAL(ModBusState(QModbusDevice::State)), parent, SLOT(onModbusStateChanged(QModbusDevice::State)));
    //connect(modbusDevice,  SIGNAL(stateChanged(QModbusDevice::State)), parent, SLOT(onModbusStateChanged(QModbusDevice::State)));
    //thr->start();

    //ModBusInterrogateTimer = new QTimer;
    //ModBusInterrogateTimer->setInterval(1000);


    SerialPort = new QSerialPort(Settings.port);
    SerialPort->setBaudRate(Settings.baud.toInt());
    SerialPort->setDataBits(QSerialPort::Data8);
    SerialPort->setParity(QSerialPort::NoParity);
    SerialPort->setStopBits(QSerialPort::OneStop);
    SerialPort->setFlowControl(QSerialPort::NoFlowControl);
    SerialPort->setReadBufferSize(1024);
    connect(SerialPort, SIGNAL(readyRead()), this, SLOT(ReadPort()));

    //if(modbusDevice->connectDevice() == true)
    if(SerialPort->open(QIODevice::ReadWrite) == true)
    {
       State = QModbusDevice::ConnectedState;
       emit ModbusState(State);
       //serialPort->isOpen();
       //ModBusInterrogateTimer->start();
    }
    else
    {
       State = QModbusDevice::ClosingState;
       emit ModbusState(State);
    }

}

void ModBus::Run()
{
    while(!AboutToFinish)
    {
        // 1. Set in mutex
        // 2. If input queue is not empty and not reading send data
        // 3. Clear in mutex
        // 4. Set out mutex
        // 5. If reading and output is ready set out data to out queue
        // 6. Clear out mutex
        // 7. Wait a bit
        QThread::msleep(10);
        qApp->processEvents();
    }
    if (SerialPort->isOpen())
        SerialPort->close();
}


ModBus::~ModBus()
{
    if (SerialPort->isOpen())
        SerialPort->close();
    //if(modbusDevice)
    //modbusDevice->disconnectDevice();

   // delete modbusDevice;

}

/*void ModBus::ResponseTimeout(int timerId)
{
    Q_UNUSED(timerId)
        m_responseTimer.stop();
        if (m_state != State::WaitingForReplay || m_queue.isEmpty())
            return;
        const auto current = m_queue.first();
        if (current.m_timerId != timerId)
            return;
        qCDebug(QT_MODBUS) << "(RTU client) Receive timeout:" << current.requestPdu;
        if (current.numberOfRetries <= 0) {
            auto item = m_queue.dequeue();
            if (item.reply) {
                item.reply->setError(QModbusDevice::TimeoutError,
                    QModbusClient::tr("Request timeout."));
            }
        }
        m_state = Idle;
        scheduleNextRequest(m_interFrameDelayMilliseconds);
}

void ModBus::onBytesWritten(qint64 bytes)
{
        //if (m_queue.isEmpty())
        //    return;
        //auto &current = m_queue.first();
        current.bytesWritten += bytes;
        if (current.bytesWritten != current.adu.size())
            return;
        qCDebug(QT_MODBUS) << "(RTU client) Send successful:" << current.requestPdu;
        if (!current.reply.isNull() && current.reply->type() == QModbusReply::Broadcast) {
            m_state = ProcessReply;
            processQueueElement({}, m_queue.dequeue());
            m_state = Idle;
            scheduleNextRequest(m_turnaroundDelay);
        } else {
            current.m_timerId = m_responseTimer.start(m_responseTimeoutDuration);
        }
}*/

void ModBus::ReadPort()
{
  quint16 MYKSS = '\0', crcfinal = '\0';
  QString crc = nullptr;
  quint8 size;
  qint64 cursize;
  int i = 0, startadr, signalsSize;
  int ival;
  Coils *Coil = new Coils;
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
        if((Group == 6) && (Commands == false))
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
        BSISig = new ModBusBSISignalStruct[signalsSize];
        else
        Sig = new ModBusSignalStruct[signalsSize];

        startadr = (static_cast<quint8>(SignalGroups[Group].data[FIRSTBYTEADR]) << 8) | (static_cast<quint8>(SignalGroups[Group].data[SECONDBYTEADR]));
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
                 BSISig[i].Val = *(quint32*)&ival;
                 BSISig[i].SigAdr = ComData.adr+i;
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
             emit BsiFromModbus(BSISig, &signalsSize);
             Commands = false;
             ReconnectTimer->start();
           }
           else if(ComData.adr == 4600)
           emit TimeSignalsReceived(BSISig);
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

           if(Group == 6)
           emit CoilSignalsReady(Coil);
           else
           emit SignalsReceived(Sig, &signalsSize);

           Group++;

           if(Group == 7)
           Group= 0;

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
          Group++;

          if(Group == 7)
          Group= 0;

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

void ModBus::WriteToPort()
{
    QByteArray* bytes = new QByteArray[1024];
    //quint32 crc=0;
    quint16 KSS = '\0';
    int i = 0;
    qint64 st;
    //QWaitCondition * cond = new QWaitCondition;
    //QMutex *mutex = new QMutex;

    while(1)
    {

        /*if(!serialPort->isOpen())
        {
           Reading = 1;
        }*/

        if(!Reading)
        {
            bytes->clear();

            //if(Reading == 0)
            //emit errorRead();

            //for(i = 0; i<6; i++)
            //{

            SerialPort->clear();

            TimeFunc::Wait(10);
            bytes->append(DeviceAdr.toInt()); // адрес устройства

            if(Commands)
            {
                bytes->append(ComData.ModCom);         //аналоговый выход
                bytes->append(static_cast<char>((ComData.adr&0xFF00)>>8));
                bytes->append(static_cast<char>(ComData.adr&0x00FF));
                bytes->append(static_cast<char>((ComData.quantity&0xFF00)>>8));
                bytes->append(static_cast<char>(ComData.quantity&0x00FF));

                if(ComData.ModCom == 0x10)
                bytes->append(static_cast<char>(ComData.sizebytes));

                if(ComData.data->size())
                {
                    for(i = 0; i<ComData.data->size(); i++)
                    {
                      bytes->append(ComData.data->data()[i]);
                    }
                }


                //for(i=0;i<bytes->size();i++)
                KSS=CalcCRC((quint8*)(bytes->data()), bytes->size());
                //KSS += static_cast<quint8>(bytes->data()[i]);
                //S2::updCRC32(bytes->data()[i],&crc);
                bytes->append(static_cast<char>(KSS>>8));
                bytes->append(static_cast<char>(KSS));

                ReadSize = 5+2*ComData.quantity;

                if(ComData.ModCom == 0x10)
                ReadSize = 8;

                ComData.data->clear();

               // commands = false;
            }
            else
            {
                for (int i=0; i<5; ++i)
                    bytes->append(SignalGroups[Group].data[i]);
/*                bytes->append(SignalGroups[Group].signaltype);         //аналоговый выход
                bytes->append(SignalGroups[Group].firstbyteadr);
                bytes->append(SignalGroups[Group].secondbyteadr);
                bytes->append(SignalGroups[Group].firstbytequantity);
                bytes->append(SignalGroups[Group].secondbytequantity); */

                //for(i=0;i<bytes->size();i++)
                KSS=CalcCRC((quint8*)(bytes->data()), bytes->size());
                //KSS += static_cast<quint8>(bytes->data()[i]);
                //S2::updCRC32(bytes->data()[i],&crc);
                bytes->append(static_cast<char>(KSS>>8));
                bytes->append(static_cast<char>(KSS));

                ReadSize = 5+2*SignalGroups[Group].data[SECONDBYTEQ];

                if(Group == 6)
                ReadSize = 9;


            }


            st = SerialPort->write(bytes->data(), bytes->size());

            if(ComData.adr == 4600)
            Write=0;
            //st=0;
            Reading = 1;

                //qCDebug << "(RTU client) Response buffer:" << m_responseBuffer.toHex();
        }

        if(CloseThr) //&& (!haveFinished))
        {
         //haveFinished = true;
         emit Finished();
         break;
        }
        //cond->wait(mutex,10);
        QThread::msleep(100);
        qApp->processEvents();
    }
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

    bytes->append(Settings.adr.toInt()); // адрес устройства

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

quint16 ModBus::CalcCRC(quint8* Dat, quint8 len)
{
  quint8 CRChi=0xFF;
  quint8 CRClo=0xFF;
  quint8 Ind;
  quint16 crc;

  while(len--)
  {
    Ind = CRChi ^ *Dat;
    Dat++;
    CRChi = CRClo ^ TabCRChi[Ind];
    CRClo = TabCRClo[Ind];
  }
  crc = ((CRChi<<8)|CRClo);
  return crc;
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

void ModBus::FinishThread()
{
    AboutToFinish = true;
}

void ModBus::Reconnect()
{
   ReconnectTimer->stop();
   ReconnectTimer->deleteLater();
   emit ReconnectSignal(1);
}

/*QModbusReply *enqueueRequest(const QModbusRequest &request, int serverAddress,
       const QModbusDataUnit &unit, QModbusReply::ReplyType type)
   {
       Q_Q(QModbusRtuSerialMaster);
       auto reply = new QModbusReply(serverAddress == 0 ? QModbusReply::Broadcast : type,
           serverAddress, q);
       QueueElement element(reply, request, unit, m_numberOfRetries + 1);
       element.adu = QModbusSerialAdu::create(QModbusSerialAdu::Rtu, serverAddress, request);
       m_queue.enqueue(element);
       scheduleNextRequest(m_interFrameDelayMilliseconds);
       return reply;
   }*/
