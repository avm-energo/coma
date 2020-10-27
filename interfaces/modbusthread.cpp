#include "modbusthread.h"
#include "../gen/datamanager.h"
#include <QCoreApplication>
#include <QElapsedTimer>

ModbusThread::ModbusThread(QObject *parent) : QObject(parent)
{
    Log = new LogClass;
    Log->Init("modbus.log");
    Log->info("=== Log started ===");
    AboutToFinish = false;
}

ModbusThread::~ModbusThread() { }

void ModbusThread::setDeviceAddress(quint8 adr) { deviceAddress = adr; }

// void ModbusThread::Init(QQueue<ModBus::InOutStruct> *inq, QList<ModBus::InOutStruct> *outl)
//{
//    InQueue = inq;
//    OutList = outl;
//}

void ModbusThread::Run()
{
    QByteArray ba;
    while (!AboutToFinish)
    {
        //        if ()
        CommandsMBS::CommandStruct inp;
        if (DataManager::deQueue(inp) == Error::Msg::NoError)
        {
            switch (inp.cmd)
            {
            case CommandsMBS::MBS_READINPUTREGISTER:
                readRegisters(inp); // send command to the port
                break;
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
            case CommandsMBS::MBS_READHOLDINGREGISTERS:
                readRegisters(inp);
                break;
            case CommandsMBS::MBS_WRITEMULTIPLEREGISTERS:
                writeMultipleRegisters(inp);
                break;
            default:
                break;
            }
        }
        QCoreApplication::processEvents();
    }
    emit Finished();
}
// void ModbusThread::SendAndGetResult(ModBus::InOutStruct &inp)
//{
//    Inp = inp;
//    Send();
//    Outp.TaskNum = Inp.TaskNum;
//    AddToOutQueue(Outp);
//}

// void ModbusThread::AddToOutQueue(ModBus::InOutStruct &outp)
//{
//    OutMutex.lock();
//    OutList->append(outp);
//    OutMutex.unlock();
//    OutWC.wakeAll();
//}

// void ModbusThread::Send()
//{
//    // data to send is in Inp.Ba
//    quint16 KSS = CalcCRC(Inp.Ba);
//    Inp.Ba.append(static_cast<char>(KSS >> 8));
//    Inp.Ba.append(static_cast<char>(KSS));
//    Outp.Ba.clear();
//    Inp.Checked = false;
//    Busy = true;
//    Log->info("-> " + Inp.Ba.toHex());
//    //    qint64 st = SerialPort->write(Inp.Ba.data(), Inp.Ba.size());
//    emit Write(Inp.Ba);
//    /*    if (st < Inp.Ba.size())
//        {
//            Outp.Res = RESEMPTY;
//            return;
//        } */
//    QElapsedTimer tme;
//    tme.start();
//    while ((Busy) && (tme.elapsed() < RECONNECTTIME)) // ждём, пока либо сервер не отработает,
//        // либо не наступит таймаут
//        QCoreApplication::processEvents();
//    if (Busy)
//    {
//        Busy = false;
//        Log->error("Timeout");
//        Outp.Res = Error::Msg::GeneralError;
//    }
//}

void ModbusThread::ParseReply(QByteArray ba)
{
    m_readData.append(ba);
    if (m_readData.size() >= 2)
    {
        quint8 receivedCommand = m_readData.at(1);
        if ((receivedCommand & 0x80) || (receivedCommand != m_commandSent.cmd))
        {
            Log->error("Modbus error response: " + m_readData.toHex());
            ERMSG("Modbus error response");
            m_readData.clear();
            Busy = false;
            return;
        }
    }
    if (m_readData.size() >= m_bytesToReceive)
    {
        Log->info("<- " + m_readData.toHex());
        int rdsize = m_readData.size();

        quint16 crcfinal = (static_cast<quint8>(m_readData.data()[rdsize - 2]) << 8)
            | (static_cast<quint8>(m_readData.data()[rdsize - 1]));
        m_readData.chop(2);
        quint16 MYKSS = CalcCRC(m_readData);

        if (MYKSS != crcfinal)
        {
            Log->error("Crc error");
            ERMSG("Modbus crc error");
            m_readData.clear();
        }
        // add to out list
        parseAndSetToOutList(m_readData);
        Busy = false;
    }
    //    Outp.Ba.append(ba);
    //    //    Log->info("<- " + Outp.Ba.toHex());
    //    if ((!Inp.Checked) && (Outp.Ba.size() >= 2))
    //    {
    //        Inp.Checked = true;
    //        if (Outp.Ba.at(1) == (Inp.Command & 0x80)) // error
    //        {
    //            Log->error("Error in module");
    //            ERMSG("modbus error response: " + Outp.Ba.toHex());
    //        }
    //        if (Outp.Ba.at(1) != Inp.Command) // wrong response
    //        {
    //            Log->error("Wrong response: " + Outp.Ba.toHex());
    //            ERMSG("modbus wrong command");
    //        }
    //    }

    //    if (Outp.Ba.size() >= Inp.ReadSize)
    //    {
    //        Log->info("<- " + Outp.Ba.toHex());
    //        int rdsize = Outp.Ba.size();

    //        quint16 crcfinal = (static_cast<quint8>(Outp.Ba.data()[rdsize - 2]) << 8)
    //            | (static_cast<quint8>(Outp.Ba.data()[rdsize - 1]));
    //        Outp.Ba.chop(2);
    //        quint16 MYKSS = CalcCRC(Outp.Ba);

    //        if (MYKSS != crcfinal)
    //        {
    //            Log->error("Crc error");
    //            ERMSG("modbus crc error");
    //            //            emit ErrorCrc();
    //            Outp.Res = Error::Msg::GeneralError;
    //            return;
    //        }
    //        Outp.Res = Error::Msg::NoError;
    //        Busy = false;
    //    }
}

void ModbusThread::send(QByteArray &ba)
{
    quint16 KSS = CalcCRC(ba);
    ba.append(static_cast<char>(KSS >> 8));
    ba.append(static_cast<char>(KSS));
    m_readData.clear();
    Busy = true;
    Log->info("-> " + ba.toHex());
    emit Write(ba);
    QElapsedTimer tme;
    tme.start();
    while ((Busy) && (tme.elapsed() < RECONNECTTIME)) // ждём, пока либо сервер не отработает,
        // либо не наступит таймаут
        QCoreApplication::processEvents();
    if (Busy)
    {
        Busy = false;
        Log->error("Timeout");
    }
}

void ModbusThread::parseAndSetToOutList(QByteArray &ba)
{
    switch (m_commandSent.cmd)
    {
    case CommandsMBS::MBS_READINPUTREGISTER:
    {
        getFloatSignals(ba);
        break;
    }
    case CommandsMBS::MBS_READHOLDINGREGISTERS:
    {
        getIntegerSignals(ba);
        break;
    }
    case CommandsMBS::MBS_WRITEMULTIPLEREGISTERS:
    {
        getCommandResponse(ba);
        break;
    }
    default:
        break;
    }
}

void ModbusThread::getFloatSignals(QByteArray &bain)
{
    DataTypes::FloatStruct signal;

    if (bain.size() < 3)
    {
        Log->error("Wrong inbuf size");
        return;
    }
    int byteSize = bain.data()[2];
    QByteArray ba = bain.mid(3);
    if (byteSize > ba.size())
    {
        ERMSG("Wrong byte size in response");
        return;
    }
    int signalsSize = byteSize / 4; // количество байт float или u32
    for (int i = 0; i < signalsSize; ++i)
    {
        quint32 ival = ((ba.data()[2 + 4 * i] << 24) & 0xFF000000) + ((ba.data()[3 + 4 * i] << 16) & 0x00FF0000)
            + ((ba.data()[4 * i] << 8) & 0x0000FF00) + ((ba.data()[1 + 4 * i] & 0x000000FF));
        memcpy(&signal.sigVal, &ival, sizeof(float));
        signal.sigAdr = m_commandSent.adr + i;
        DataManager::addSignalToOutList(DataTypes::SignalTypes::Float, signal);
    }
}

void ModbusThread::getIntegerSignals(QByteArray &bain)
{
    DataTypes::BitStringStruct signal;

    if (bain.size() < 3)
    {
        Log->error("Wrong inbuf size");
        return;
    }
    int byteSize = bain.data()[2];
    QByteArray ba = bain.mid(3);
    if (byteSize > ba.size())
    {
        ERMSG("Wrong byte size in response");
        return;
    }
    int signalsSize = byteSize / 4; // количество байт float или u32
    for (int i = 0; i < signalsSize; ++i)
    {
        quint32 ival = ((ba.data()[2 + 4 * i] << 24) & 0xFF000000) + ((ba.data()[3 + 4 * i] << 16) & 0x00FF0000)
            + ((ba.data()[4 * i] << 8) & 0x0000FF00) + ((ba.data()[1 + 4 * i] & 0x000000FF));
        memcpy(&signal.sigVal, &ival, sizeof(quint32));
        signal.sigAdr = m_commandSent.adr + i;
        DataManager::addSignalToOutList(DataTypes::SignalTypes::BitString, signal);
    }
}

void ModbusThread::getCommandResponse(QByteArray &bain)
{
    if (bain.size() < 3)
    {
        Log->error("Wrong inbuf size");
        return;
    }
    int byteSize = bain.data()[2];
    QByteArray ba = bain.mid(3);
    if (byteSize > ba.size())
    {
        ERMSG("Wrong byte size in response");
        return;
    }
    DataTypes::GeneralResponseStruct grs;
    grs.type = DataTypes::GeneralResponseTypes::Ok;
    DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, grs);
}

void ModbusThread::FinishThread() { AboutToFinish = true; }

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

void ModbusThread::readRegisters(CommandsMBS::CommandStruct &cms)
{
    QByteArray ba;

    setQueryStartBytes(cms, ba);
    if (cms.data.size())
        ba.append(cms.data);
    Log->info("Send bytes: " + ba.toHex());
    m_bytesToReceive = cms.quantity * 2 + 5; // address, function code, bytes count, crc (2)
}

void ModbusThread::writeMultipleRegisters(CommandsMBS::CommandStruct &cms)
{
    QByteArray ba;
    setQueryStartBytes(cms, ba);
    ba.append(cms.quantity * 2);
    if (cms.data.size())
        ba.append(cms.data);
    Log->info("Send bytes: " + ba.toHex());
    m_bytesToReceive = 8; // address, function code, address (2), quantity (2), crc (2)
}

void ModbusThread::setQueryStartBytes(CommandsMBS::CommandStruct &cms, QByteArray &ba)
{
    m_commandSent = cms;
    ba.append(deviceAddress); // адрес устройства
    ba.append(cms.cmd); //аналоговый выход
    ba.append(static_cast<char>((cms.adr & 0xFF00) >> 8));
    ba.append(static_cast<char>(cms.adr & 0x00FF));
    ba.append(static_cast<char>((cms.quantity & 0xFF00) >> 8));
    ba.append(static_cast<char>(cms.quantity & 0x00FF));
}

// void ModbusThread::readHoldingRegisters(CommandsMBS::CommandStruct &cms)
//{
//    QByteArray ba;

//    setQueryStartBytes(cms, ba);

//}
