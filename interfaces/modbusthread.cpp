#include "modbusthread.h"

#include "../gen/datamanager.h"

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

void ModbusThread::setDeviceAddress(quint8 adr)
{
    deviceAddress = adr;
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
        CommandsMBS::CommandStruct inp;
        if (DataManager::deQueue(inp) == Error::Msg::NoError)
        {
            switch (inp.cmd)
            {
            case CommandsMBS::MBS_READINPUTREGISTER:
                //                Com45(inp.uintarg);
                //                RunMutex.lock();
                //                RunWC.wait(&RunMutex, 20);
                //                RunMutex.unlock();
                //        while (!InQueue->isEmpty())
                //        {
                //            InMutex.lock();
                //            //            ModBus::InOutStruct inp = InQueue->dequeue();
                //            Queries::CommandMBS inp;
                //            if (DataManager::deQueue(inp) == Error::Msg::NoError)
                //                InMutex.unlock();
                //            SendAndGetResult(inp);
                QCoreApplication::processEvents();
            }
            emit Finished();
        }
    }
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

void ModbusThread::setQueryStartBytes(CommandsMBS::CommandStruct &cms, QByteArray &ba)
{
    QByteArray bytes {};

    bytes.append(deviceAddress); // адрес устройства
    bytes.append(cms.cmd);       //аналоговый выход
    bytes.append(static_cast<char>((cms.adr & 0xFF00) >> 8));
    bytes.append(static_cast<char>(cms.adr & 0x00FF));
    bytes.append(static_cast<char>((cms.quantity & 0xFF00) >> 8));
    bytes.append(static_cast<char>(cms.quantity & 0x00FF));
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

void ModbusThread::readInputRegisters(CommandsMBS::CommandStruct &cms)
{
    QByteArray ba;

    setQueryStartBytes(cms, ba);
    if (cms.data.size())
        ba.append(cms.data);
    Log->info("Send bytes: " + ba.toHex());
}
