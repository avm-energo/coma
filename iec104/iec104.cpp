#include <QThread>
#include <QCoreApplication>
#include <QDialog>
#include <QVBoxLayout>
#include "../gen/timefunc.h"
#include "../gen/error.h"
#include "iec104.h"
#include "ethernet.h"
#include "../widgets/emessagebox.h"

QMutex ParseReadMutex;
QMutex ParseWriteMutex;

IEC104::IEC104(QVector<S2::DataRec> *s2, QObject *parent) : QObject(parent)
{
    EthThreadWorking = false;
    ParseThreadWorking = false;
    AboutToFinish = false;
    S2Config = s2;
    Log = new LogClass;
    Log->Init("iec104.log");
    Log->info("=== Log started ===");
}

IEC104::~IEC104()
{
}

bool IEC104::Working()
{
    return (EthThreadWorking | ParseThreadWorking);
}

void IEC104::Connect(const QString &IP, quint16 baseadr)
{
    INFOMSG("IEC104: connect");
    EthThreadWorking = false;
    ParseThreadWorking = false;
    AboutToFinish = false;
    QThread *thr = new QThread;
    Ethernet *eth = new Ethernet;
    eth->moveToThread(thr);
    eth->IP = IP;
    connect(eth,SIGNAL(Finished()),thr,SLOT(quit()));
    connect(eth,SIGNAL(Finished()),eth,SLOT(deleteLater()));
    connect(thr,SIGNAL(started()),eth,SLOT(Run()));
    connect(thr,SIGNAL(finished()),thr,SLOT(deleteLater()));
    connect(this,SIGNAL(StopAll()),eth,SLOT(Stop()));
    connect(eth,SIGNAL(Connected()),this,SLOT(EthThreadStarted()));
    connect(eth,SIGNAL(Disconnected()),this,SLOT(EthThreadFinished()));

    Parse = new IEC104Thread(Log, InputQueue, S2Config);
    QThread *thr2 = new QThread;
    Parse->moveToThread(thr2);
    connect(this,SIGNAL(StopAll()),Parse,SLOT(Stop()));
    connect(Parse,SIGNAL(Finished()),Parse,SLOT(deleteLater()));
    connect(Parse,SIGNAL(Finished()),thr2,SLOT(quit()));
    connect(Parse,SIGNAL(Finished()),this,SLOT(ParseThreadFinished()));
    connect(thr2,SIGNAL(finished()),thr2,SLOT(deleteLater()));
    connect(thr2,SIGNAL(started()),Parse,SLOT(Run()));
    connect(Parse,SIGNAL(Started()),this,SLOT(ParseThreadStarted()));
    connect(eth,SIGNAL(Connected()),Parse,SLOT(StartDT()));
    connect(eth,SIGNAL(Finished()),this,SLOT(EmitReconnectSignal()));
    connect(Parse,SIGNAL(WriteData(QByteArray)),eth,SLOT(InitiateWriteDataToPort(QByteArray)));
    connect(eth,SIGNAL(NewDataArrived(QByteArray)),Parse,SLOT(GetSomeData(QByteArray)));
    connect(Parse,SIGNAL(ReconnectSignal()),this,SLOT(EmitReconnectSignal()));

    connect(Parse,SIGNAL(Bs104signalsreceived(IEC104Thread::BS104Signals*)),\
            this,SIGNAL(Bs104signalsready(IEC104Thread::BS104Signals*)),Qt::BlockingQueuedConnection);
    connect(Parse,SIGNAL(Floatsignalsreceived(IEC104Thread::FlSignals104*)),\
            this,SIGNAL(Floatsignalsready(IEC104Thread::FlSignals104*)),Qt::BlockingQueuedConnection);
    connect(Parse,SIGNAL(Sponsignalsreceived(IEC104Thread::SponSignals*)),\
            this,SIGNAL(Sponsignalsready(IEC104Thread::SponSignals*)),Qt::BlockingQueuedConnection);
    connect(Parse,SIGNAL(SendS2fromParse(QVector<S2::DataRec>*)),this,SIGNAL(SendS2fromiec104(QVector<S2::DataRec>*)));
    connect(Parse,SIGNAL(SendJourSysfromParse(QByteArray)),this,SIGNAL(SendJourSysfromiec104(QByteArray)));
    connect(Parse,SIGNAL(SendJourWorkfromParse(QByteArray)),this,SIGNAL(SendJourWorkfromiec104(QByteArray)));
    connect(Parse,SIGNAL(SendJourMeasfromParse(QByteArray)),this,SIGNAL(SendJourMeasfromiec104(QByteArray)));

    connect(Parse,SIGNAL(SendMessageOk()), this,SIGNAL(SendMessageOk()));

    connect(Parse,SIGNAL(SetDataSize(int)),this,SIGNAL(SetDataSize(int)));
    connect(Parse,SIGNAL(SetDataCount(int)),this,SIGNAL(SetDataCount(int)));
    connect(Parse,SIGNAL(SendMessagefromParse()),this,SIGNAL(SendConfMessageOk()));

    Parse->SetBaseAdr(baseadr);
    Parse->incLS = 0;
    Parse->count = 0;

    thr->start();
    thr2->start();
}

void IEC104::CorReadRequest()
{
    IEC104Thread::InputStruct inp;
    inp.cmd = IEC104Thread::CM104_CORREADREQUEST;
    ParseWriteMutex.lock();
    InputQueue.enqueue(inp);
    ParseWriteMutex.unlock();
}

void IEC104::SelectFile(char numFile)
{
    IEC104Thread::InputStruct inp;
    inp.cmd = IEC104Thread::CM104_SELECTFILE;
    inp.args.uintarg = numFile;
    ParseWriteMutex.lock();
    InputQueue.enqueue(inp);
    ParseWriteMutex.unlock();
}

void IEC104::FileReady(QVector<S2::DataRec>* File)
{
    IEC104Thread::InputStruct inp;
    inp.cmd = IEC104Thread::CM104_FILEREADY;
    inp.args.ptrarg = File;
    ParseWriteMutex.lock();
    InputQueue.enqueue(inp);
    ParseWriteMutex.unlock();
}

void IEC104::Com45(quint32 com)
{
    IEC104Thread::InputStruct inp;
    inp.cmd = IEC104Thread::CM104_COM45;
    inp.args.uintarg = com;
    ParseWriteMutex.lock();
    InputQueue.enqueue(inp);
    ParseWriteMutex.unlock();
}

void IEC104::Com50(quint32 adr, float param)
{
    IEC104Thread::InputStruct inp;
    inp.cmd = IEC104Thread::CM104_COM50;
    inp.args.uintarg = adr;
    inp.args.flarg = param;
    ParseWriteMutex.lock();
    InputQueue.enqueue(inp);
    ParseWriteMutex.unlock();
}

void IEC104::InterrogateTimeGr15()
{
    IEC104Thread::InputStruct inp;
    inp.cmd = IEC104Thread::CM104_INTERROGATETIMEGR15;
    ParseWriteMutex.lock();
    InputQueue.enqueue(inp);
    ParseWriteMutex.unlock();
}

void IEC104::com51WriteTime(uint time)
{
    IEC104Thread::InputStruct inp;
    inp.cmd = IEC104Thread::CM104_COM51WRITETIME;
    inp.args.uintarg = time;
    ParseWriteMutex.lock();
    InputQueue.enqueue(inp);
    ParseWriteMutex.unlock();
}

void IEC104::EthThreadStarted()
{
    EthThreadWorking = true;
}

void IEC104::EthThreadFinished()
{
    EthThreadWorking = false;
    if (!ParseThreadWorking)
        emit Finished();
}

void IEC104::ParseThreadStarted()
{
    ParseThreadWorking = true;
}

void IEC104::ParseThreadFinished()
{
    ParseThreadWorking = false;
    if (!EthThreadWorking)
        emit Finished();
}

void IEC104::EmitReconnectSignal()
{
    if (!AboutToFinish)
        emit ReconnectSignal();
}

void IEC104::StopAllThreads()
{
    AboutToFinish = true;
    emit StopAll();
}

// Класс PARSE104

IEC104Thread::IEC104Thread(LogClass *log, QQueue<InputStruct> &queue, QVector<S2::DataRec> *s2, QObject *parent) : QObject(parent)
{
    DR = s2;
    FirstParse = true;
    InputQueue = &queue;
    Log = log;
    ThreadMustBeFinished = false;
    V_S = V_R = 0;
    AckVR = I104_W;
    APDUFormat = I104_WRONG;
    FileSending = false;
    Timer104 = new QTimer;
    Timer104->setInterval(15000);
    ConTimer = new QTimer;
    ConTimer->setInterval(5000);
#ifndef DEBUG
    connect(Timer104,SIGNAL(timeout()),this,SLOT(Stop()));
    connect(ConTimer,SIGNAL(timeout()),this,SLOT(SendTestAct()));
    ConTimer->start();
#endif
    NoAnswer = 0;
}

IEC104Thread::~IEC104Thread()
{
}

void IEC104Thread::SetBaseAdr(quint16 adr)
{
    BaseAdr = adr;
}

void IEC104Thread::Run()
{
    emit Started();
    while (!ThreadMustBeFinished)
    {
        if (!ParseData.isEmpty())
        {
            ParseReadMutex.lock();
            QByteArray tmpba = ParseData.takeFirst();
            ParseReadMutex.unlock();
            if (!tmpba.isEmpty())
            {
                int tmpi = isIncomeDataValid(tmpba);
                if (tmpi == I104_RCVNORM) // если поймали правильное начало данных, переходим к их обработке
                {
                    if (APDUFormat == I104_I)
                    {
                        tmpba = tmpba.mid(6);
                        ParseIFormat(tmpba); // без APCI
                    }
                }
            }
        }
        if (!FileSending)
        {
            if (!InputQueue->isEmpty())
            {
                ParseWriteMutex.lock();
                InputStruct inp = InputQueue->dequeue();
                ParseWriteMutex.unlock();
                switch (inp.cmd)
                {
                case CM104_COM45:
                    Com45(inp.args.uintarg);
                    break;
                case CM104_COM50:
                    Com50(inp.args.uintarg, inp.args.flarg);
                    break;
                case CM104_FILEREADY:
                {
                    QVector<S2::DataRec> *ptr = reinterpret_cast<QVector<S2::DataRec> *>(inp.args.ptrarg);
//                    FileReady(ptr);
                    FileReady();
                    break;
                }
                case CM104_SELECTFILE:
                    SelectFile(inp.args.uintarg);
                    break;
                case CM104_COM51WRITETIME:
                    Com51WriteTime(inp.args.uintarg);
                    break;
                case CM104_CORREADREQUEST:
                    CorReadRequest();
                    break;
                case CM104_INTERROGATETIMEGR15:
                    InterrogateTimeGr15();
                    break;
                }
            }
            if (V_R > (AckVR + I104_W))
                SendS();
        }
        QCoreApplication::processEvents();
    }
    emit Finished();
}

void IEC104Thread::GetSomeData(QByteArray ba)
{
    if (FirstParse)
        Log->info("<-- " + ba.toHex());
    FirstParse = false;
    quint32 basize = static_cast<quint32>(ba.size());
    if (CutPckt.size() > 1)
    {
        quint32 cutpcktlen = static_cast<quint8>(CutPckt.at(1));
        cutpcktlen += 2;
        quint32 cutpcktsize = static_cast<quint32>(CutPckt.size());
        quint32 missing_num = cutpcktlen-cutpcktsize; // взяли длину остатка от предыдущего пакета
        if (missing_num>basize)
        {
            CutPckt.append(ba);
            return; // если так и не достигли конца пакета, надо брать следующий пакет в cutpckt
        }
        CutPckt.append(ba.left(missing_num)); // взяли из текущего пакета сами байты
        ba.remove(0,missing_num);
        ParseReadMutex.lock();
        ParseData.append(CutPckt);
        ParseReadMutex.unlock();
        CutPckt.clear();
        basize = static_cast<quint32>(ba.size());
    }
    if (basize < 2) // ba is empty or there's not enough symbols to parse in it
      return;

    CutPckt = ba.left(2);
    ba = ba.mid(2);
    GetSomeData(ba);
    FirstParse = true;
}

int IEC104Thread::isIncomeDataValid(QByteArray ba)
{
    try
    {
        if (ba.at(0) != 0x68)
            return I104_RCVWRONG;
        APDULength = static_cast<quint8>(ba.at(1)); // в 1-м байте лежит длина
        if ((APDULength < 4) || (APDULength > 253))
            return I104_RCVWRONG;
        if (!(ba.at(2)&0x01)) // I
            APDUFormat = I104_I;
        else
        {
            if (!(ba.at(2)&0x02)) // S
                APDUFormat = I104_S;
            else
                APDUFormat = I104_U;
        }
        switch (APDUFormat)
        {
        case I104_I:
        {
            quint16 V_Rrcv = static_cast<quint8>(ba.at(3))*256+static_cast<quint8>(ba.at(2)&0xFE);
            V_Rrcv >>= 1;
            if (V_Rrcv != V_R)
            {
                Log->error("V_Rrcv != V_R");
                return I104_RCVWRONG;
            }
            V_R++;
            quint16 V_Srcv = static_cast<quint8>(ba.at(5))*256+static_cast<quint8>(ba.at(4)&0xFE);
            V_Srcv >>= 1;
            if (V_Srcv != V_S)
                V_S = V_Srcv;           // временно, нужно исправить проблему несовпадения s посылок
            return I104_RCVNORM;
            break;
        }
        case I104_S:
        {
            quint16 V_Srcv = static_cast<quint8>(ba.at(5))*256+static_cast<quint8>(ba.at(4)&0xFE);
            V_Srcv >>= 1;
            if (V_Srcv != V_S)
                V_S = V_Srcv;
            return I104_RCVNORM;
            break;
        }
        case I104_U:
        {
            unsigned char baat2 = ba.at(2);
            if ((baat2 == I104_STARTDT_CON) && (Command == I104_STARTDT_ACT)) // если пришло подтверждение старта и перед этим мы старт запрашивали
            {
                Timer104->stop();
                Command = I104_STARTDT_CON;
                SendGI();
            }
            if ((baat2 == I104_STOPDT_CON) && (Command == I104_STOPDT_ACT)) // если пришло подтверждение стопа и перед этим мы стоп запрашивали
                Command = I104_STOPDT_CON;
            if ((baat2 == I104_TESTFR_CON) && (Command == I104_TESTFR_ACT)) // если пришло подтверждение теста и перед этим мы тест запрашивали
                Command = I104_TESTFR_CON;
            if (baat2 == I104_TESTFR_ACT)
                SendTestCon();

            NoAnswer = 0;

            return I104_RCVNORM;
            break;
        }
        default:
            break;
        }
        return I104_RCVWRONG;
    }
    catch (...)
    {
        ERMSG("Fatal exception");
        return GENERALERROR;
    }
}

void IEC104Thread::ParseIFormat(QByteArray &ba) // основной разборщик
{
    try
    {
        DUI.typeIdent = ba[0];
        DUI.qualifier.Number = ba[1]&0x7f;
        DUI.qualifier.SQ = ba[1]>>7;
        DUI.cause.cause = ba[2]&0x3F;
        DUI.cause.confirm = (ba[2]>>6)&0x01;
        DUI.cause.test = ba[2]>>7;
        DUI.cause.initiator = ba[3];
        DUI.commonAdrASDU = ba[4] + ba[5]*256;
        quint32 objectAdr = 0;
        quint32 index = 6;
        int fileSize;
        int res,i,cntfl = 0,cntflTimestamp = 0,cntspon = 0,cntbs = 0; //, cntsponTime = 0;
        IEC104Thread::FlSignals104* flSignals = new IEC104Thread::FlSignals104[DUI.qualifier.Number];
        IEC104Thread::SponSignals* sponsignals = new IEC104Thread::SponSignals[DUI.qualifier.Number];
        IEC104Thread::BS104Signals* BS104Signals = new IEC104Thread::BS104Signals[DUI.qualifier.Number];
        unsigned char num = ba[9];

        for (i = 0; i < DUI.qualifier.Number; i++)
        {
            if ((i==0) || (DUI.qualifier.SQ==0))
            {

                objectAdr = ba[index++];
                objectAdr &= 0x00FF;
                objectAdr |= ba[index++]<<8;
                objectAdr += ba[index++]*0x10000;
            }
            else
                objectAdr++;
            switch(DUI.typeIdent)
            {
            case M_EI_NA_1: // 70 тип - подтверждение окончания инициализации
            {
                break;
            }
            case M_ME_TF_1: // 36 тип - измеренные данные с плавающей запятой
            {
                if (cntflTimestamp >= DUI.qualifier.Number)
                {
                    ERMSG("out of array flSignals");
                    return;
                }
                (flSignals+cntflTimestamp)->fl.SigAdr=objectAdr;
                float value;
                memcpy(&value,&(ba.data()[index]),4);
                index += 4;
                (flSignals+cntflTimestamp)->fl.SigVal=value;
                quint8 quality;
                memcpy(&quality,&(ba.data()[index]),1);
                index++;
                (flSignals+cntflTimestamp)->fl.SigQuality=quality;
                quint64 time;
                memcpy(&time,&(ba.data()[index]),8);
                index += 8;
                (flSignals+cntflTimestamp)->fl.CP56Time=time;
                cntflTimestamp++;
                break;
            }

            case C_IC_NA_1: // 100
                break;

            case M_ME_NC_1:   // 13 тип - измеренные данные с плавающей запятой
            {
                if (cntfl >= DUI.qualifier.Number)
                {
                    ERMSG("out of array flSignals");
                    return;
                }
                (flSignals+cntfl)->fl.SigAdr=objectAdr;
                float value;
                memcpy(&value,&(ba.data()[index]),4);
                index += 4;
                (flSignals+cntfl)->fl.SigVal=value;
                quint8 quality;
                memcpy(&quality,&(ba.data()[index]),1);
                index++;
                (flSignals+cntfl)->fl.SigQuality=quality;
                cntfl++;
                break;
            }

            case M_SP_NA_1:
            {
                if (cntspon > 255)
                {
                    ERMSG("out of array sponsignals");
                    return;
                }
               sponsignals->Spon[cntspon].SigAdr=objectAdr;
               quint8 value;
               memcpy(&value,&(ba.data()[index]),1);
               index += 1;
               sponsignals->Spon[cntspon].SigVal=value;
               cntspon++;
               break;
            }

            case M_SP_TB_1:
            {
                if (cntspon > 255)
                {
                    ERMSG("out of array sponsignals");
                    return;
                }
                sponsignals->Spon[cntspon].SigAdr=objectAdr;
                quint8 value;
                memcpy(&value,&(ba.data()[index]),1);
                index += 1;
                sponsignals->Spon[cntspon].SigVal=value;
                quint64 time;
                memcpy(&time,&(ba.data()[index]),7);
                index += 7;
                sponsignals->Spon[cntspon].CP56Time=time;
                cntspon++;
                break;
             }

            case M_BO_NA_1:
            {
                if (cntbs >= DUI.qualifier.Number)
                {
                    ERMSG("out of array BS104Signals");
                    return;
                }
                int j;
                for(j=0 ; j<3; j++)
                (BS104Signals+cntbs)->BS.SigAdr[j]=(objectAdr>>8*j);

                quint32 value;
                memcpy(&value,&(ba.data()[index]),4);
                index += 4;
                (BS104Signals+cntbs)->BS.SigVal=value;
                quint8 quality;
                memcpy(&quality,&(ba.data()[index]),1);
                index++;
                (BS104Signals+cntbs)->BS.SigQuality=quality;
                cntbs++;
                break;
            }

            case F_SR_NA_1: // секция готова
            {
                Log->info("Section ready");
                GetSection(ba[9]);
                break;
            }

            case F_FR_NA_1: // файл готов
            {
                Log->info("File ready");
                ReadData.clear();
                RDSize = 0;
                RDLength = 0;
                fileSize = (static_cast<quint8>(ba[13]) << 16) | (static_cast<quint8>(ba[12]) << 8) | (static_cast<quint8>(ba[11]));
                emit SetDataSize(fileSize);
                CallFile(ba[9]);
                break;
            }

            case F_SG_NA_1: // сегмент
            {
                Log->info("Segment ready: RDSize=" + QString::number(ba[12], 16) + ", num=" + QString::number(ba[13]));
                RDSize = static_cast<quint8>(ba[12]);
                RDSize &= 0xFF;
                if(RDSize) //>= RDLength)
                {
                    ReadData.append(&(ba.data()[13]), RDSize);
                    RDLength += RDSize;
                    emit SetDataCount(RDLength);
                }
                break;
            }

            case F_LS_NA_1: // последняя секция, последнй сегмент
            {
                Log->info("Last section, ba[12] = "+QString::number(ba[12]));
                    switch(ba[12])
                    {
                    case 1:
                    {
                        ConfirmFile(num);
                        ConTimer->start();
                        FileSending = false;
                        Log->info("FileSending clear");

                        int filetype = ba.at(9);
                        if(filetype == 0x01)  // если файл конфигурации
                        {
                           res = S2::RestoreDataMem(ReadData.data(), RDLength, DR);
                           if (res == NOERROR)
                               emit SendS2fromParse(DR);
                        }
                        else if(filetype == 0x04)   // если файл системного журнала
                            emit SendJourSysfromParse(ReadData);
                        else if(filetype == 0x05)   // если файл рабочего журнала
                              emit SendJourWorkfromParse(ReadData);
                        else if(filetype == 6)   // если файл журнала измерений
                            emit SendJourMeasfromParse(ReadData);
                        RDLength = 0;
                        RDSize = 0;
                        break;
                     }

                     case 3:
                     {
                        ConfirmSection(num);
                        break;
                     }
                    }
                    break;
            }

            case F_SC_NA_1: // запрос файла, секции
            {
                if(ba.at(12) == 0x02)  //запрос файла
                {
                    Log->info("File query");
                    SectionReady();
                }
                if(ba.at(12) == 0x06)
                {
                    Log->info("Segment query");
                    SendSegments();
                }
                break;
            }

            case F_AF_NA_1: // подтверждение файла, секции
            {
                Log->info("Last section of file " + QString::number(ba[12]) + " confirm");
                if(ba.at(12) == 0x03)  // подтверждение секции
                    LastSection();
                if(ba.at(12) == 0x01)  // подтверждение файла
                {
                    Log->info("FileSending clear");
                    FileSending = false;
                    emit SendMessagefromParse();
                }
                break;
            }

            case C_SC_NA_1:
            {
                if(DUI.cause.cause == 10)
                    emit SendMessageOk();
                break;
            }

            case C_SE_NC_1:
            {
                if(DUI.cause.cause == 10)
                count++;

                emit SetDataCount(count);
                quint32 adr = ba[6] + (ba[7]+1)*256; //+ (ba[8]<<16);

                if((adr == 920) && (DUI.cause.cause == 10))  // если адрес последнего параметра коррекции
                {
                   if(ba.at(13) == 0)
                   {
                       emit SendMessageOk();
                       emit SetDataCount(11);
                   }
                   count=0;
                }
                break;
            }

            default:
                break;
            }
        }

        if(cntflTimestamp != 0)
        {
            flSignals->SigNumber = cntflTimestamp;
            emit Floatsignalsreceived(flSignals);
        }

        if(cntfl != 0)
        {
            flSignals->SigNumber = cntfl;
            emit Floatsignalsreceived(flSignals);
        }

        if(cntspon != 0)
        {
            sponsignals->SigNumber = cntspon;
            emit Sponsignalsreceived(sponsignals);
        }

        if(cntbs != 0)
        {
            BS104Signals->SigNumber = cntbs;
            emit Bs104signalsreceived(BS104Signals);
        }
    }
    catch (...)
    {
        Log->error("Catch exception");
    }
}

void IEC104Thread::StartDT()
{
    Log->info("Start()");
    APCI StartDT;
    ASDU GInter;
    StartDT.append(I104_START);
    StartDT.append(0x04);
    StartDT.append(I104_STARTDT_ACT);
    StartDT.append(QByteArrayLiteral("\x00\x00\x00"));
    Command = I104_STARTDT_ACT;
    Send(0,StartDT);
    Timer104->start();
}

void IEC104Thread::StopDT()
{
    Log->info("Stop()");
    APCI StopDT;
    StopDT.append(I104_START);
    StopDT.append(0x04);
    StopDT.append(I104_STOPDT_ACT);
    StopDT.append(QByteArrayLiteral("\x00\x00\x00"));
    Command = I104_STOPDT_ACT;
    Send(0,StopDT); // ASDU = QByteArray()
    ConTimer->stop();
}

void IEC104Thread::Send(int inc, APCI apci, ASDU asdu)
{
    QByteArray ba = apci;
    ba.append(asdu);
    Log->info("--> " + ba.toHex());
    emit WriteData(ba);
    if(inc)
        V_S++;
}

void IEC104Thread::Stop()
{
    ThreadMustBeFinished = true;
    Timer104->stop();
}

QByteArray IEC104Thread::CreateGI(unsigned char apdulength)
{
    QByteArray GI;
    GI.append(I104_START);
    GI.append(apdulength);
    GI.append((V_S & 0x007F) << 1);
    GI.append((V_S & 0x7F80) >> 7);
    GI.append((V_R & 0x007F) << 1);
    GI.append((V_R & 0x7F80) >> 7);
    Command = I104_S;
    return GI;
}

QByteArray IEC104Thread::ASDUFilePrefix(unsigned char cmd, unsigned char filenum, unsigned char secnum)
{
    QByteArray ba;
    ba.append(cmd);
    ba.append(QByteArrayLiteral("\x01\x0d\x00"));
    ba.append(BaseAdr);
    ba.append(BaseAdr>>8);
    ba.append(QByteArrayLiteral("\x00\x00\x00"));
    ba.append(filenum);
    ba.append('\x00');
    ba.append(secnum);
    return ba;
}

QByteArray IEC104Thread::ASDU6Prefix(unsigned char cmd, quint32 adr)
{
    ASDU ba;
    ba.append(cmd);
    ba.append(QByteArrayLiteral("\x01\x06\x00"));
    ba.append(BaseAdr);
    ba.append(BaseAdr>>8);
    ba.append(adr);
    ba.append(adr>>8);
    ba.append(adr>>16);
    return ba;
}

void IEC104Thread::SendGI()
{
    ASDU GInter = ASDU6Prefix(C_IC_NA_1, 0x00);
    GInter.append('\x14');
    APCI GI = CreateGI(0x0e);
    Send(1, GI, GInter); // ASDU = QByteArray()
    AckVR = V_R;
}

template <typename T> QByteArray IEC104Thread::ToByteArray(T var)
{
    QByteArray ba;
    int sizeofvar = sizeof(var);
    for (int i=0; i<sizeofvar; ++i)
    {
        unsigned char tmpc;
        memcpy(&tmpc, reinterpret_cast<quint8 *>(&var)+i, sizeof(quint8));
        ba.append(tmpc);
    }
    return ba;
}

void IEC104Thread::SendS()
{
    APCI Confirm;

    Confirm.append(I104_START);
    Confirm.append(0x04);
    Confirm.append(I104_S);
    Confirm.append('\x0');
    Confirm.append((V_R & 0x007F) << 1);
    Confirm.append((V_R & 0x7F80) >> 7);
    Command = I104_S;
    Send(0,Confirm); // ASDU = QByteArray()
    AckVR = V_R;
}

void IEC104Thread::SendTestCon()
{
    APCI GI;
    GI.append(I104_START);
    GI.append(0x04);
    GI.append(I104_TESTFR_CON);
    GI.append(QByteArrayLiteral("\x00\x00\x00"));
    Command = I104_TESTFR_CON;
    Send(0,GI); // ASDU = QByteArray()
}

void IEC104Thread::CorReadRequest()
{
    Log->info("CorReadRequest()");
    ASDU GCor = ASDU6Prefix(C_IC_NA_1, 0);
    GCor.append('\x16');
    APCI GI = CreateGI(0x0e);
    Send(1, GI, GCor); // ASDU = QByteArray()
    AckVR = V_R;
}

void IEC104Thread::SendTestAct()
{
    if(NoAnswer)
    {
        emit ReconnectSignal();
        ERMSG("No answer");
        return;
    }
    else
        NoAnswer = 1;

    ConTimer->stop();
    APCI GI;
    GI.append(I104_START);
    GI.append(0x04);
    GI.append(I104_TESTFR_ACT);
    GI.append(QByteArrayLiteral("\x00\x00\x00"));
    Command = I104_TESTFR_ACT;
    Send(0,GI); // ASDU = QByteArray()
}

void IEC104Thread::SelectFile(char numfile)
{
    SecNum = 1;
    FileSending = true;
    Log->info("FileSending set");
    ConTimer->stop();
    Log->info("SelectFile(" + QString::number(numfile) + ")");
    ASDU cmd = ASDUFilePrefix(F_SC_NA_1, numfile, 0x00);
    cmd.append('\x01');
    APCI GI = CreateGI(0x11);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Thread::CallFile(unsigned char numFile)
{
    ASDU cmd = ASDUFilePrefix(F_SC_NA_1, numFile, 0x00);
    cmd.append('\x02');
    APCI GI = CreateGI(0x11);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Thread::GetSection(unsigned char numFile)
{
    ASDU cmd = ASDUFilePrefix(F_SC_NA_1, numFile, SecNum);
    cmd.append('\x06');
    APCI GI = CreateGI(0x11);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Thread::ConfirmSection(unsigned char numFile)
{
    ASDU cmd = ASDUFilePrefix(F_AF_NA_1, numFile, SecNum);
    cmd.append('\x03');
    APCI GI = CreateGI(0x11);
    Send(1, GI, cmd); // ASDU = QByteArray()
    SecNum++;
}

void IEC104Thread::ConfirmFile(unsigned char numFile)
{
    ASDU cmd = ASDUFilePrefix(F_AF_NA_1, numFile, SecNum);
    cmd.append('\x01');
    APCI GI = CreateGI(0x11);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

//void IEC104Thread::FileReady(QVector<S2::DataRec> *file)
void IEC104Thread::FileReady()
{
    FileSending = true;
    Log->info("FileSending set");
//    DR = file;
    SecNum = 1;
    ASDU cmd = ASDUFilePrefix(F_FR_NA_1, 0x01, 0x00);
    cmd.chop(1);
    File.resize(65535);
    S2::StoreDataMem(&(File.data()[0]), DR, 0x0001); // 0x0001 - номер файла конфигурации
    FileLen = static_cast<quint8>(File.data()[4]);
    FileLen += static_cast<quint8>(File.data()[5])*256;
    FileLen += static_cast<quint8>(File.data()[6])*65536;
    FileLen += static_cast<quint8>(File.data()[7])*16777216;
    FileLen += sizeof(S2::FileHeader); // FileHeader
    File.resize(FileLen);
    cmd.append(FileLen&0xFF);
    cmd.append((FileLen&0xFF00)>>8);
    cmd.append((FileLen&0xFF0000)>>16);
    APCI GI = CreateGI(0x12);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Thread::SectionReady()
{
    ASDU cmd = ASDUFilePrefix(F_SR_NA_1, 0x01, SecNum);
    cmd.append(FileLen&0xFF);
    cmd.append((FileLen&0xFF00)>>8);
    cmd.append((FileLen&0xFF0000)>>16);
    APCI GI = CreateGI(0x13);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Thread::SendSegments()
{
    APCI GI;
    KSS = 0;
    KSF = 0;

    unsigned int pos = 0;
/*    if (FileLen > static_cast<quint32>(File.size()))
    {
        ERMSG("FileLen is bigger than file");
        return;
    } */
    emit SetDataSize(FileLen);
    ASDU cmd = ASDUFilePrefix(F_SG_NA_1, 0x01, SecNum);
    cmd.append('\x0');
    unsigned char diff;
    do
    {
        quint32 filesize = File.size();
        if(filesize > SEGMENTSIZE)
            diff = SEGMENTSIZE;
        else
            diff = filesize;
        cmd.append(File.left(diff));
        for(int i = 0; i<diff; ++i)
        {
           KSS += static_cast<quint8>(File.data()[i]);
           KSF += static_cast<quint8>(File.data()[i]);
        }
        File = File.mid(diff);
        pos += diff;
        emit SetDataCount(pos);
        cmd[12] = static_cast<unsigned char>(diff);
        GI = CreateGI(diff+17);
        Send(1, GI, cmd); // ASDU = QByteArray()
        cmd = cmd.left(13);
        QThread::msleep(100);
    } while (!File.isEmpty());

    cmd = ASDUFilePrefix(F_LS_NA_1, 1, SecNum);
    cmd.append('\x03');
    cmd.append(KSS);
    KSS = 0;
    GI = CreateGI(0x12);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Thread::LastSection()
{
    ASDU cmd = ASDUFilePrefix(F_LS_NA_1, 1, SecNum);
    cmd.append('\x01');
    cmd.append(KSF);
    KSF = 0;
    APCI GI = CreateGI(0x12);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Thread::Com45(quint32 com)
{
    ASDU cmd = ASDU6Prefix(C_SC_NA_1, com);
    cmd.append('\x01');
    APCI GI = CreateGI(0x0e);
    Send(1, GI, cmd);
}

void IEC104Thread::Com50(quint32 adr, float param)
{
    if(adr == 910)
        emit SetDataSize(11);

    ASDU cmd = ASDU6Prefix(C_SE_NC_1, adr);
    cmd.append(ToByteArray(param));
    cmd.append('\x00');
    APCI GI = CreateGI(0x12);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Thread::InterrogateTimeGr15()
{
    ASDU GTime = ASDU6Prefix(C_IC_NA_1, 0);
    GTime.append(0x23);
    APCI GI = CreateGI(0x0e);
    Send(1, GI, GTime); // ASDU = QByteArray()
    AckVR = V_R;
}

void IEC104Thread::Com51WriteTime(quint32 time)
{
    ASDU cmd = ASDU6Prefix(C_BO_NA_1, 4601);
    cmd.append(ToByteArray(time));
    APCI GI = CreateGI(0x11);
    Send(1, GI, cmd); // ASDU = QByteArray()
}
