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
#include "modbus.h"
#include "../gen/timefunc.h"
#include "../widgets/emessagebox.h"
#include "../gen/mainwindow.h"

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
    deviceAdr = Settings.adr;
    Group = 0;
    readSize = 0;
    Reading = false;
    closeThr = false;
    int i;
    commands = false;
    First = 1;
    count = 0;
    write = 0;
    reconnectTimer = new QTimer;
    reconnectTimer->setInterval(RECONNECTIME);

    connect(reconnectTimer, SIGNAL(timeout()), this, SLOT(Reconnect()));

    for(i=0;i<6;i++)
    SignalGroups[i].signaltype=0x04;

    SignalGroups[0].firstbyteadr = 0;
    SignalGroups[0].secondbyteadr = 0x65;
    SignalGroups[0].firstbytequantity = 0;
    SignalGroups[0].secondbytequantity = 0x04;
    SignalGroups[1].firstbyteadr = 0x03;
    SignalGroups[1].secondbyteadr = 0xE8;
    SignalGroups[1].firstbytequantity = 0;
    SignalGroups[1].secondbytequantity = 0x20;
    /*SignalGroups[2].firstbyteadr = 0x03;
    SignalGroups[2].secondbyteadr = 0xFC;
    SignalGroups[2].firstbytequantity = 0;
    SignalGroups[2].secondbytequantity = 0x0A;*/
    SignalGroups[2].firstbyteadr = 0x04;
    SignalGroups[2].secondbyteadr = 0x4C;
    SignalGroups[2].firstbytequantity = 0;
    SignalGroups[2].secondbytequantity = 0x20;
    SignalGroups[3].firstbyteadr = 0x09;
    SignalGroups[3].secondbyteadr = 0x60;
    SignalGroups[3].firstbytequantity = 0;
    SignalGroups[3].secondbytequantity = 0x0E;
    SignalGroups[4].firstbyteadr = 0x09;
    SignalGroups[4].secondbyteadr = 0x74;
    SignalGroups[4].firstbytequantity = 0;
    SignalGroups[4].secondbytequantity = 0x1C;
    SignalGroups[5].firstbyteadr = 0x11;
    SignalGroups[5].secondbyteadr = 0x95;
    SignalGroups[5].firstbytequantity = 0;
    SignalGroups[5].secondbytequantity = 0x04;
    /*SignalGroups[7].firstbyteadr = 0x04;
    SignalGroups[7].secondbyteadr = 0x60;
    SignalGroups[7].firstbytequantity = 0;
    SignalGroups[7].secondbytequantity = 0x0A;*/
    SignalGroups[6].signaltype=0x01;
    SignalGroups[6].firstbyteadr = 0x0B;
    SignalGroups[6].secondbyteadr = 0xC3;
    SignalGroups[6].firstbytequantity = 0;
    SignalGroups[6].secondbytequantity = 0x19;

    //connect(this,  SIGNAL(ModBusState(QModbusDevice::State)), parent, SLOT(onModbusStateChanged(QModbusDevice::State)));
    //connect(modbusDevice,  SIGNAL(stateChanged(QModbusDevice::State)), parent, SLOT(onModbusStateChanged(QModbusDevice::State)));
    //thr->start();

    //ModBusInterrogateTimer = new QTimer;
    //ModBusInterrogateTimer->setInterval(1000);


    serialPort = new QSerialPort(Settings.port);
    serialPort->setBaudRate(Settings.baud.toInt());
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    serialPort->setReadBufferSize(1024);


    connect(serialPort, SIGNAL(readyRead()), this, SLOT(ReadPort()));

    //if(modbusDevice->connectDevice() == true)
    if(serialPort->open(QIODevice::ReadWrite) == true)
    {
       state = QModbusDevice::ConnectedState;
       emit ModBusState(state);
       //serialPort->isOpen();
       //ModBusInterrogateTimer->start();
    }
    else
    {
       state = QModbusDevice::ClosingState;
       emit ModBusState(state);
    }

}

/*void ModBus::Run()
{

    while(1)
    {
        if(modbusDevice->state() == QModbusDevice::ConnectedState)
        {
           if(state != QModbusDevice::ConnectedState)
           {
               state = QModbusDevice::ConnectedState;
               emit ModBusState(state);
           }

        }
        else
        {
            if(state != QModbusDevice::ClosingState)
            {
                state = QModbusDevice::ClosingState;
                emit ModBusState(state);
            }
        }

        QThread::msleep(10);
        qApp->processEvents();
    }
}*/


ModBus::~ModBus()
{
    //if(modbusDevice)
    //modbusDevice->disconnectDevice();

   // delete modbusDevice;

}

void ModBus::reading()
{
    Reading = 0;
}

void ModBus::onAboutToClose()
{
    //serialPort->close();
    //Q_ASSERT(modbusDevice->state() == QModbusDevice::ClosingState);
        //m_responseTimer.stop();
}

void ModBus::onResponseTimeout(int timerId)
{
        /*m_responseTimer.stop();
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
        scheduleNextRequest(m_interFrameDelayMilliseconds);*/
}

/*void ModBus::onBytesWritten(qint64 bytes)
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
  responseBuffer = *new QByteArray[1024];
  responseBuffer.clear();
  Reading = 1;

  cursize = serialPort->bytesAvailable();

  if(ComData.adr != 1)
  {
     reconnectTimer->stop();
     reconnectTimer->setInterval(RECONNECTIME);
     reconnectTimer->start();
  }

  if(cursize == readSize)
  {
      responseBuffer = serialPort->read(cursize);//serialPort->bytesAvailable());
      size = responseBuffer.size();
      size -= 2;

      MYKSS=CalcCRC((quint8*)(responseBuffer.data()), size);

      crcfinal = (static_cast<quint8>(responseBuffer.data()[readSize-2]) << 8) | (static_cast<quint8>(responseBuffer.data()[readSize-1]));

      if(MYKSS == crcfinal)
      {
        if((Group == 6) && (commands == false))
        {
           Coil = new Coils;
           Coil->countBytes = responseBuffer.data()[2];
           signalsSize = 0;
           for(i=0; i<Coil->countBytes; i++)
           {
             Coil->Bytes[i] = responseBuffer.data()[3+i];
           }
        }
        else
        signalsSize = responseBuffer.data()[2]/4; // количество байт float или u32

        if(ComData.adr == 1 || ComData.adr == 4600)
        BSISig = new ModBusBSISignal[signalsSize];
        else
        Sig = new ModBusSignal[signalsSize];

        startadr = (static_cast<quint8>(SignalGroups[Group].firstbyteadr) << 8) | (static_cast<quint8>(SignalGroups[Group].secondbyteadr));
        for(i=0; i<signalsSize; i++)
        {
         if(ComData.adr == 1  || ComData.adr == 4600)  // bsi
         ival = (((static_cast<quint8>(responseBuffer.data()[5+4*i])<<24)&0xFF000000)+((static_cast<quint8>(responseBuffer.data()[6+4*i])<<16)&0x00FF0000)+((static_cast<quint8>(responseBuffer.data()[3+4*i])<<8)&0x0000FF00)+(static_cast<quint8>(responseBuffer.data()[4+4*i])&0x000000FF));
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
           ival = ((responseBuffer.data()[5+4*i]<<24)&0xFF000000)+((responseBuffer.data()[6+4*i]<<16)&0x00FF0000)+((responseBuffer.data()[3+4*i]<<8)&0x0000FF00)+((responseBuffer.data()[4+4*i]&0x000000FF));
           Sig[i].flVal = *(float*)&ival;
         }

         if(commands)
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

        if(commands)
        {
           if(ComData.adr>=900 && ComData.adr<=910)
           {
               int recordSize = (static_cast<quint8>(responseBuffer.data()[4]) << 8) | (static_cast<quint8>(responseBuffer.data()[5]));
               if(recordSize == ComData.quantity)
               signalsSize = 1;
               TimeFunc::Wait(100);

           }

           if(ComData.adr == 1)
           {
             ComData.adr = 0;
             emit BsiFromModBus(BSISig, &signalsSize);
             commands = false;
             reconnectTimer->start();
           }
           else if(ComData.adr == 4600)
           emit timeSignalsReceived(BSISig);
           else
           {
             commands = false;
             emit corsignalsreceived(Sig, &signalsSize);
           }

           //commands = false;
           //ComData.adr = 0;
        }
        else
        {

           if(Group == 6)
           emit coilsignalsready(Coil);
           else
           emit signalsreceived(Sig, &signalsSize);

           Group++;

           if(Group == 7)
           Group= 0;

           //Reading = 0;
        }
      }
      else
      {
        emit errorCrc();
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

          responseBuffer = serialPort->read(cursize);
          if((responseBuffer.data()[3] == (char)0xC2) && (responseBuffer.data()[4] == (char)0xC1))
          {
             if(ComData.adr == 4600)
             emit timeReadError();
             else
             emit errorRead();
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

            serialPort->clear();

            TimeFunc::Wait(10);
            bytes->append(deviceAdr.toInt()); // адрес устройства

            if(commands)
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

                readSize = 5+2*ComData.quantity;

                if(ComData.ModCom == 0x10)
                readSize = 8;

                ComData.data->clear();

               // commands = false;
            }
            else
            {
                bytes->append(SignalGroups[Group].signaltype);         //аналоговый выход
                bytes->append(SignalGroups[Group].firstbyteadr);
                bytes->append(SignalGroups[Group].secondbyteadr);
                bytes->append(SignalGroups[Group].firstbytequantity);
                bytes->append(SignalGroups[Group].secondbytequantity);

                //for(i=0;i<bytes->size();i++)
                KSS=CalcCRC((quint8*)(bytes->data()), bytes->size());
                //KSS += static_cast<quint8>(bytes->data()[i]);
                //S2::updCRC32(bytes->data()[i],&crc);
                bytes->append(static_cast<char>(KSS>>8));
                bytes->append(static_cast<char>(KSS));

                readSize = 5+2*SignalGroups[Group].secondbytequantity;

                if(Group == 6)
                readSize = 9;


            }


            st = serialPort->write(bytes->data(), bytes->size());

            if(ComData.adr == 4600)
            write=0;
            //st=0;
            Reading = 1;

                //qCDebug << "(RTU client) Response buffer:" << m_responseBuffer.toHex();
        }

        if(closeThr) //&& (!haveFinished))
        {
         //haveFinished = true;
         emit finished();
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

    commands = true;
    bytes->clear();
    serialPort->clear();

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

     readSize = 5+2*ComData.quantity;

     st = serialPort->write(bytes->data(), bytes->size());
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
    if(serialPort != nullptr)
    serialPort->close();

    Reading = true;
    closeThr = true;
}

void ModBus::ModWriteCor(information* info, float* data)//, int* size)
{
    int i;
    quint32 fl;
    //bool readingState = Reading;

    while(Reading == true)
    {
        TimeFunc::Wait(10);
        count++;
        if(count == 20)
        {
          Reading = false;
          count = 0;
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
    commands = true;
    Reading = false;

}

void ModBus::ModReadCor(information* info)
{

    while(Reading == true)
    {
        TimeFunc::Wait(10);
        count++;
        if(count == 20)
        {
          Reading = false;
          count = 0;
        }
    }

    ComData.ModCom = 0x04;
    ComData.adr = info->adr;
    ComData.quantity = (quint8)((info->size)*2);
    ComData.sizebytes = (quint8)((info->size)*4);

    ComData.data = new QByteArray[100];
    ComData.data->clear();

    //TimeFunc::Wait(100);
    commands = true;
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
        count++;
        if(count == 20)
        {
          Reading = false;
          count = 0;
        }
    }

    if(!write)
    {
      ComData.ModCom = 0x03;
      ComData.adr = 4600;
      ComData.quantity = 2;
      ComData.sizebytes = 4;
      ComData.data = new QByteArray[100];
      ComData.data->clear();
      //TimeFunc::Wait(100);
      commands = true;
      Reading = false;
    }


}

void ModBus::WriteTime(uint* time)
{
    write = 1;
    quint32 T;
    //ComData = *new ComInfo;

    while(Reading == true)
    {
        TimeFunc::Wait(10);
        count++;
        if(count == 20)
        {
          Reading = false;
          count = 0;
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
    commands = true;
    Reading = false;

}

void ModBus::tabs(int index)
{

  if(!MainWindow::TheEnd)
  {
    while(Reading == true)
    {
        TimeFunc::Wait(10);
        count++;
        if(count == 200)
        {
          Reading = false;
          count = 0;
        }
    }

   if(index == timeIndex)
   {
       TimeFunc::Wait(1000);
       ComData.ModCom = 0x03;
       ComData.adr = 4600;
       ComData.quantity = 2;
       ComData.sizebytes = 4;
       ComData.data = new QByteArray[100];
       ComData.data->clear();
         //TimeFunc::Wait(100);
       commands = true;
       //Reading = false;
   }
   else if(index == corIndex)
   {
       ComData.ModCom = 0x04;
       ComData.adr = 4000;
       ComData.quantity = 22;
       ComData.sizebytes = 44;

       ComData.data = new QByteArray[100];
       ComData.data->clear();

       //TimeFunc::Wait(100);
       commands = true;
       //Reading = false;
   }
   else if(index == checkIndex)
   {
       ComData.adr = 0;
       commands = false;
   }

   Reading = false;
  }
}

void ModBus::Reconnect()
{
   reconnectTimer->stop();
   reconnectTimer->deleteLater();
   emit reconnectSignal(1);
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
