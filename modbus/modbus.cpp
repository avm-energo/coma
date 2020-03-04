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
#include "../gen/s2.h"
#include "modbus.h"
#include "../gen/timefunc.h"


ModBus::ModBus(ModBus_Settings Settings, QObject *parent) : QObject(parent)
{
    //modbusDevice = new QModbusRtuSerialMaster(this);
    //Reply = new  QModbusReply;
    //state = *new QModbusDevice::State;
    //modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
    //                ui->portEdit->text());
    //QThread *thr = new QThread;
    //thr->setPriority(QThread::HighestPriority);
    //ModThr* modthr = new ModThr;
    //ModThr->moveToThread(thr);
   // connect(thr,SIGNAL(started()),this,SLOT(Run()));
   // connect(thr,SIGNAL(finished()),thr,SLOT(deleteLater()));
   // connect(thr,SIGNAL(finished()),this,SLOT(deleteLater()));

    /*modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, Settings.parity.toInt());
    modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, Settings.baud.toInt());
    modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, Settings.stop.toInt());
    modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, 8);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, Settings.port);
    */
    //modbusDevice->setTimeout(m_settingsDialog->settings().responseTime);  // для слэйва не надо
    //modbusDevice->setNumberOfRetries(m_settingsDialog->settings().numberOfRetries);

    //connect(modbusDevice,  SIGNAL(QModbusDevice::stateChanged(QModbusDevice::State state)), parent, SLOT(onModbusStateChanged(state)));
    deviceAdr = Settings.adr.toInt();
    Groups = 0;
    int i;

    for(i=0;i<6;i++)
    SignalGroups[0].signaltype=0x04;

    SignalGroups[0].firstbyteadr = 0;
    SignalGroups[0].secondbyteadr = 0x65;
    SignalGroups[0].firstbytequantity = 0;
    SignalGroups[0].secondbytequantity = 0x02;
    SignalGroups[1].firstbyteadr = 0x03;
    SignalGroups[1].secondbyteadr = 0xE8;
    SignalGroups[1].firstbytequantity = 0;
    SignalGroups[1].secondbytequantity = 0x10;
    SignalGroups[2].firstbyteadr = 0x04;
    SignalGroups[2].secondbyteadr = 0x4C;
    SignalGroups[2].firstbytequantity = 0;
    SignalGroups[2].secondbytequantity = 0x10;
    SignalGroups[3].firstbyteadr = 0x09;
    SignalGroups[3].secondbyteadr = 0x60;
    SignalGroups[3].firstbytequantity = 0;
    SignalGroups[3].secondbytequantity = 0x07;
    SignalGroups[4].firstbyteadr = 0x09;
    SignalGroups[4].secondbyteadr = 0x74;
    SignalGroups[4].firstbytequantity = 0;
    SignalGroups[4].secondbytequantity = 0x0E;
    SignalGroups[5].firstbyteadr = 0x11;
    SignalGroups[5].secondbyteadr = 0x95;
    SignalGroups[5].firstbytequantity = 0;
    SignalGroups[5].secondbytequantity = 0x02;





    connect(this,  SIGNAL(ModBusState(QModbusDevice::State)), parent, SLOT(onModbusStateChanged(QModbusDevice::State)));
    //connect(modbusDevice,  SIGNAL(stateChanged(QModbusDevice::State)), parent, SLOT(onModbusStateChanged(QModbusDevice::State)));
    //thr->start();

    ModBusInterrogateTimer = new QTimer;
    ModBusInterrogateTimer->setInterval(2000);


    serialPort = new QSerialPort(Settings.port);
    serialPort->setBaudRate(Settings.baud.toInt());
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    //connect(&m_responseTimer, &Timer::timeout, q, [this](int timerId) {
    //            onResponseTimeout(timerId);
     //       });
    //connect(serialPort, SIGNAL(readChannelFinished()), this, SLOT(reading()));
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(ReadPort()));
    connect(serialPort, SIGNAL(aboutToClose()), this, SLOT(onAboutToClose()));
    //connect(serialPort, SIGNAL(WriteToPort(bytes, len))), this, SLOT(WriteToPort(bytes, len);
    connect(ModBusInterrogateTimer,SIGNAL(timeout()),this,SLOT(WriteToPort()));
    connect(this,SIGNAL(signalsreceived(ModBusSignal*)), parent,SLOT(UpdateModBusData(ModBusSignal*)));
    /*        QObject::connect(m_serialPort, QOverload<QSerialPort::SerialPortError>::of(&QSerialPort::error),
                    q, [this](QSerialPort::SerialPortError error) {
                onError(error);
            });*/

    //if(modbusDevice->connectDevice() == true)
    if(serialPort->open(QIODevice::ReadWrite) == true)
    {
       state = QModbusDevice::ConnectedState;
       emit ModBusState(state);      
       ModBusInterrogateTimer->start();
    }
    else
    {
       state = QModbusDevice::ClosingState;
       emit ModBusState(state);
    }

}

void ModBus::Run()
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
}


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
    serialPort->close();
    Q_ASSERT(modbusDevice->state() == QModbusDevice::ClosingState);
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
  quint16 MYKSS = '\0', crc = '\0';
  quint8 size;
  Reading = 1;
  responseBuffer = new QByteArray;
  responseBuffer->clear();


  if(serialPort->bytesAvailable())
  {
      *responseBuffer = serialPort->read(serialPort->bytesAvailable());
      size = responseBuffer->size();
      size -= 2;

      MYKSS=CalcCRC((quint8*)(responseBuffer->data()), size);
      crc = ((responseBuffer->data()[7]<<8) | responseBuffer->data()[8]);
      if(MYKSS == crc)
      {
        MYKSS = crc;
      }
      else
      {
        crc=0;
      }
  }
}

void ModBus::WriteToPort()
{
    QByteArray* bytes = new QByteArray;
    char zero = 0;
    //quint32 crc=0;
    quint16 KSS = '\0';
    int i = 0;
    bytes->clear();

    for(i = 0; i<6; i++)
    {

        TimeFunc::Wait(10);
        bytes->append((char)deviceAdr); // адрес устройства
        bytes->append(SignalGroups[i].signaltype);         //аналоговый выход
        bytes->append(SignalGroups[i].firstbyteadr);
        bytes->append(SignalGroups[i].secondbyteadr);
        bytes->append(SignalGroups[i].firstbytequantity);
        bytes->append(SignalGroups[i].secondbytequantity);

        //for(i=0;i<bytes->size();i++)
        KSS=CalcCRC((quint8*)(bytes->data()), bytes->size());
        //KSS += static_cast<quint8>(bytes->data()[i]);
        //S2::updCRC32(bytes->data()[i],&crc);
        bytes->append(static_cast<char>(KSS>>8));
        bytes->append(static_cast<char>(KSS));

        serialPort->write(bytes->data(), bytes->size());
    }
        //qCDebug << "(RTU client) Response buffer:" << m_responseBuffer.toHex();
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
