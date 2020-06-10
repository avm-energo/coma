#include <QTime>
#include <QThread>
#include <QTimer>
#include <QCoreApplication>
#include <QDialog>
#include <QVBoxLayout>
#include "../gen/timefunc.h"
#include "../gen/error.h"
#include "iec104.h"
#include "ethernet.h"
#include "../widgets/emessagebox.h"
/*#include <vcruntime_string.h>

#include <vcruntime.h> */

IEC104::IEC104(QObject *parent) : QObject(parent)
{
//    GSD = true;
    EthThreadWorking = false;
    ParseThreadWorking = false;
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

void IEC104::Start()
{
    Log->info("Start()");
    EthThreadWorking = true;
    APCI StartDT;
    ASDU GInter;
    StartDT.start = I104_START;
    StartDT.APDUlength = 4;
    StartDT.contrfield[0] = I104_STARTDT_ACT;
    StartDT.contrfield[1] = StartDT.contrfield[2] = StartDT.contrfield[3] = 0;
    Parse->cmd = I104_STARTDT_ACT;
    Send(0,StartDT);//, GInter); // ASDU = QByteArray()
//    emit ParseTimer104Start();
    Parse->Timer104->start();
}

void IEC104::Stop()
{
    Log->info("Stop()");
    if (ParseThreadWorking)
    {
        APCI StopDT;
        StopDT.start = I104_START;
        StopDT.APDUlength = 4;
        StopDT.contrfield[0] = I104_STOPDT_ACT;
        StopDT.contrfield[1] = StopDT.contrfield[2] = StopDT.contrfield[3] = 0;
        Parse->cmd = I104_STOPDT_ACT;
        Send(0,StopDT); // ASDU = QByteArray()
        if(ConTimer != nullptr)
           ConTimer->stop();
        emit StopAll();
    }
}

void IEC104::Send(int inc, APCI apci, ASDU asdu)
{
    QByteArray ba;
    ba.clear();
    void *tempp = &apci;
    ba.append(static_cast<char *>(tempp),sizeof(apci));
    if(!asdu.isEmpty())
    {
        tempp = asdu.data();
        ba.append(static_cast<char *>(tempp),asdu.size());
    }
    Log->info("--> " + ba.toHex());
    emit writedatatoeth(ba);  
    if(inc)
        Parse->V_S++;
}

void IEC104::Connect(const QString &IP)
{
    INFOMSG("IEC104: connect");
    QThread *thr = new QThread;
    ethernet *eth = new ethernet;
    eth->moveToThread(thr);
    eth->IP = IP;
    connect(eth,SIGNAL(Finished()),thr,SLOT(quit()));
    connect(eth,SIGNAL(Finished()),eth,SLOT(deleteLater()));
    connect(thr,SIGNAL(started()),eth,SLOT(Run()));
    connect(thr,SIGNAL(finished()),thr,SLOT(deleteLater()));
    connect(this,SIGNAL(StopAll()),eth,SLOT(Stop()));
    connect(eth,SIGNAL(connected()),this,SIGNAL(EthConnected()));
    connect(eth,SIGNAL(disconnected()),this,SIGNAL(EthDisconnected()));
    connect(eth,SIGNAL(disconnected()),this,SLOT(EthThreadFinished()));
    connect(eth,SIGNAL(connected()),this,SLOT(Start()));
    connect(eth,SIGNAL(newdataarrived(QByteArray)),this,SLOT(GetSomeData(QByteArray)));
    connect(this,SIGNAL(writedatatoeth(QByteArray)),eth,SLOT(InitiateWriteDataToPort(QByteArray)));
    //connect(eth,SIGNAL(disconnected()), parent, SLOT(DisconnectAndClear()));
    //connect(eth,SIGNAL(ethNoconnection()), parent, SLOT(DisconnectAndClear()));

    Parse = new Parse104;
    QThread *thr2 = new QThread;
    Parse->moveToThread(thr2);
    connect(this,SIGNAL(StopAll()),Parse,SLOT(Stop()));
    connect(Parse,SIGNAL(Finished()),Parse,SLOT(deleteLater()));
    connect(Parse,SIGNAL(Finished()),thr2,SLOT(quit()));
    connect(Parse,SIGNAL(Finished()),this,SLOT(ParseThreadFinished()));
    connect(thr2,SIGNAL(finished()),thr2,SLOT(deleteLater()));
    connect(thr2,SIGNAL(started()),Parse,SLOT(Run()));
    connect(Parse,SIGNAL(Started()),this,SLOT(ParseThreadStarted()));
    connect(eth,SIGNAL(Finished()),Parse,SLOT(Stop()));
    connect(this,SIGNAL(ParseTimer104Start()),Parse,SLOT(Timer104Start()));
    connect(this,SIGNAL(ParseTimer104Stop()),Parse,SLOT(Timer104Stop()));
    connect(Parse,SIGNAL(bs104signalsreceived(Parse104::BS104Signals*)),\
            this,SIGNAL(bs104signalsready(Parse104::BS104Signals*)),Qt::BlockingQueuedConnection);
    connect(Parse,SIGNAL(floatsignalsreceived(Parse104::FlSignals104*)),\
            this,SIGNAL(floatsignalsready(Parse104::FlSignals104*)),Qt::BlockingQueuedConnection);
    connect(Parse,SIGNAL(sponsignalsreceived(Parse104::SponSignals*)),\
            this,SIGNAL(sponsignalsready(Parse104::SponSignals*)),Qt::BlockingQueuedConnection);
/*    connect(Parse,SIGNAL(sponsignalsreceived(Parse104::SponSignals104*)),\
            this,SIGNAL(sponsignalsready(Parse104::SponSignals104*)),Qt::BlockingQueuedConnection); */
/*    connect(Parse,SIGNAL(sponsignalWithTimereceived(Parse104::SponSignalsWithTime*)),\
            this,SIGNAL(sponsignalWithTimereceived(Parse104::SponSignalsWithTime*)),Qt::BlockingQueuedConnection); */
/*    connect(Parse,SIGNAL(UpdateReleWidget(Parse104::SponSignals104*)),\
            this,SIGNAL(relesignalsready(Parse104::SponSignals104*)),Qt::BlockingQueuedConnection); */
    connect(Parse,SIGNAL(sendS()),this,SLOT(SendS()));
    connect(Parse,SIGNAL(GeneralInter()),this,SLOT(SendI()));
    connect(Parse,SIGNAL(sendAct()),this,SLOT(SendTestCon()));
    connect(Parse,SIGNAL(CallFile(unsigned char)),this,SLOT(CallFile(unsigned char)));
    connect(Parse,SIGNAL(CallSection(unsigned char)),this,SLOT(GetSection(unsigned char)));
    connect(Parse,SIGNAL(SendConfirmSection(unsigned char)),this,SLOT(ConfirmSection(unsigned char)));
    //connect(Parse,SIGNAL(sendConfirmFile(unsigned char)),this,SLOT(ConfirmFile(unsigned char)));
    connect(Parse,SIGNAL(sendS2fromParse(QVector<S2::DataRec>*)),this,SIGNAL(sendS2fromiec104(QVector<S2::DataRec>*)));
    connect(Parse,SIGNAL(sendJourSysfromParse(QByteArray)),this,SIGNAL(sendJourSysfromiec104(QByteArray)));
    connect(Parse,SIGNAL(sendJourWorkfromParse(QByteArray)),this,SIGNAL(sendJourWorkfromiec104(QByteArray)));
    connect(Parse,SIGNAL(sendJourMeasfromParse(QByteArray)),this,SIGNAL(sendJourMeasfromiec104(QByteArray)));

    connect(Parse,SIGNAL(sectionReady()),this,SLOT(SectionReady()));
    connect(Parse,SIGNAL(segmentReady()),this,SLOT(SendSegments()));
    connect(this,SIGNAL(LastSeg()),this,SLOT(LastSegment()));
    connect(Parse,SIGNAL(LastSec()),this,SLOT(LastSection()));
    connect(Parse,SIGNAL(sendMessageOk()), this,SIGNAL(sendMessageOk()));

    connect(Parse,SIGNAL(SetDataSizeFromParse(int)),this,SIGNAL(SetDataSize(int)));
    connect(Parse,SIGNAL(SetDataCountFromParse(int)),this,SIGNAL(SetDataCount(int)));
    connect(Parse,SIGNAL(sendMessagefromParse()),this,SIGNAL(sendConfMessageOk()));
//    connect(Parse,SIGNAL(writeCorMesOkParse()),this,SIGNAL(sendCorMesOk()));

    Parse->incLS = 0;
    Parse->count = 0;

    thr->start();
    thr2->start();
    ConTimer = new QTimer;
    ConTimer->setInterval(5000);
#ifndef DEBUG
    connect(ConTimer,SIGNAL(timeout()),this,SLOT(SendTestAct()));
#endif
    ConTimer->start();
}

void IEC104::SendI()
{
    APCI GI;
    ASDU GInter;
    quint16 VR = Parse->V_R;

    GInter[0] = static_cast<char>(C_IC_NA_1);
    GInter[1] = static_cast<char>(1);
    GInter[2] = static_cast<char>(6);
    GInter[3] = static_cast<char>(0);
    GInter[4] = static_cast<char>(BaseAdr);
    GInter[5] = static_cast<char>(BaseAdr>>8);
    GInter[6] = static_cast<char>(0);
    GInter[7] = static_cast<char>(0);
    GInter[8] = static_cast<char>(0);
    GInter[9] = static_cast<char>(20);

    GI.start = I104_START;
    GI.APDUlength = 14;
    GI.contrfield[0] = (Parse->V_S & 0x007F) << 1;
    GI.contrfield[1] = (Parse->V_S & 0x7F80) >> 7;
    GI.contrfield[2] = (VR & 0x007F) << 1;
    GI.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;
    Send(1, GI, GInter); // ASDU = QByteArray()

    Parse->AckVR = Parse->V_R;

    //while(stopincrementing)
    //QThread::usleep(10);

   // Parse->V_S++;

    Parse->InterogateTimer->start();
}

void IEC104::CorReadRequest()
{
    APCI GI;
    ASDU GCor;
    quint16 VR = Parse->V_R;

    Log->info("CorReadRequest()");
    GCor[0] = static_cast<char>(C_IC_NA_1);
    GCor[1] = static_cast<char>(1);
    GCor[2] = static_cast<char>(6);
    GCor[3] = static_cast<char>(0);
    GCor[4] = static_cast<char>(BaseAdr);
    GCor[5] = static_cast<char>(BaseAdr>>8);
    GCor[6] = static_cast<char>(0);
    GCor[7] = static_cast<char>(0);
    GCor[8] = static_cast<char>(0);
    GCor[9] = static_cast<char>(22);

    GI.start = I104_START;
    GI.APDUlength = 14;
    GI.contrfield[0] = (Parse->V_S & 0x007F) << 1;
    GI.contrfield[1] = (Parse->V_S & 0x7F80) >> 7;
    GI.contrfield[2] = (VR & 0x007F) << 1;
    GI.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;
    Send(1, GI, GCor); // ASDU = QByteArray()

    Parse->AckVR = Parse->V_R;

    //while(stopincrementing)
    //QThread::usleep(10);

   // Parse->V_S++;

    Parse->InterogateTimer->start();
}

void IEC104::SendTestCon()
{
    APCI GI;
    ASDU GInter;
    //quint16 VR = Parse->V_R;

    GI.start = I104_START;
    GI.APDUlength = 4;
    GI.contrfield[0] = I104_TESTFR_CON;
    GI.contrfield[1] = 0;
    GI.contrfield[2] = 0;
    GI.contrfield[3] = 0;
    Parse->cmd = I104_TESTFR_CON;
    Send(0,GI); // ASDU = QByteArray()

    //while(stopincrementing)
    //QThread::usleep(10);

    //Parse->V_S++;

}

void IEC104::SendTestAct()
{
    APCI GI;
    //ASDU GInter;
    //quint16 VR = Parse->V_R;

    if(Parse->NoAnswer)
    {
        ConTimer->stop();
//        ConTimer->deleteLater();
        emit ReconnectSignal();
        ERMSG("Нет ответа");
        return;
    }
    else
    Parse->NoAnswer = 1;

    GI.start = I104_START;
    GI.APDUlength = 4;
    GI.contrfield[0] = I104_TESTFR_ACT;
    GI.contrfield[1] = 0;
    GI.contrfield[2] = 0;
    GI.contrfield[3] = 0;
    Parse->cmd = I104_TESTFR_ACT;
    Send(0,GI); // ASDU = QByteArray()

    //while(stopincrementing)
    //QThread::usleep(10);

    //Parse->V_S++;

}

void IEC104::GetSomeData(QByteArray ba)
{
//    Parse->ParseMutex.lock();
    Log->info("<-- " + ba.toHex());
    ParseSomeData(ba);
//    Parse->ParseMutex.unlock();
}

void IEC104::ParseSomeData(QByteArray ba) //, bool GSD)
{
    quint32 basize = static_cast<quint32>(ba.size());
//    if (GSD)
//    {
        if (cutpckt.size() > 1)
        {
            quint32 cutpcktlen = static_cast<quint8>(cutpckt.at(1));
            cutpcktlen += 2;
            quint32 cutpcktsize = static_cast<quint32>(cutpckt.size());
            quint32 missing_num = cutpcktlen-cutpcktsize; // взяли длину остатка от предыдущего пакета
            if (missing_num>basize)
            {
                cutpckt.append(ba);
//                ERMSG("Так и не достигли конца пакета, надо брать следующий пакет в cutpckt");
                return; // если так и не достигли конца пакета, надо брать следующий пакет в cutpckt
            }
            cutpckt.append(ba.left(missing_num)); // взяли из текущего пакета сами байты
            ba.remove(0,missing_num);
            Parse->ParseMutex.lock();
            Parse->ParseData.append(cutpckt);
            Parse->ParseMutex.unlock();
            cutpckt.clear();
            basize = static_cast<quint32>(ba.size());
        }
//    }
    if (basize < 2) // ba is empty or there's not enough symbols to parse in it
      return;

    cutpckt = ba.left(2);
    ba = ba.mid(2);
    ParseSomeData(ba);
/*    quint32 BlockLength = static_cast<quint8>(ba.at(1))+2;
    if (BlockLength == 0)
    {
        while ((!BlockLength) && (ba.size()>3))
        {
            ba.remove(0,2);
            BlockLength = static_cast<quint8>(ba.at(1))+2;
        }
    }
    if (BlockLength == basize)
        Parse->ParseData.append(ba);
    else if (basize < BlockLength)
        cutpckt = ba;
    else
    {
        Parse->ParseData.append(ba.left(BlockLength));
        ParseSomeData(ba.right(basize-BlockLength), false);
    } */
}

void IEC104::SendS()
{
    APCI Confirm;
    Confirm.start = I104_START;
    Confirm.APDUlength = 4;
    Confirm.contrfield[0] = I104_S;
    Confirm.contrfield[1] = 0x00;
    quint16 VR = Parse->V_R;
    Confirm.contrfield[2] = (VR & 0x007F) << 1;
    Confirm.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;
    Send(0,Confirm); // ASDU = QByteArray()
    Parse->AckVR = Parse->V_R;
}

// Класс PARSE104

Parse104::Parse104(QObject *parent) : QObject(parent)
{
    ParseData.clear();
    ThreadMustBeFinished = false;
    V_S = V_R = 0;
    AckVR = I104_W;
    APDUFormat = I104_WRONG;
    GetNewVR = false;
    NewDataArrived = false;
    FileSending = 0;
    Timer104 = new QTimer;
    Timer104->setInterval(15000);
#ifndef DEBUG
    connect(Timer104,SIGNAL(timeout()),this,SLOT(Stop()));
#endif
    InterogateTimer = new QTimer;
    InterogateTimer->setInterval(15000);
    InterogateTimer->setSingleShot(true);
#ifndef DEBUG
    connect(InterogateTimer,SIGNAL(timeout()),this,SLOT(ErrMsg()));
#endif
    NoAnswer = 0;
}

Parse104::~Parse104()
{
}

void Parse104::Run()
{
    emit Started();
    while (!ThreadMustBeFinished)
    {
        // обработка ParseData
        if (ParseData.size())
        {
            ParseMutex.lock();
            QByteArray tmpba = ParseData.at(0);
            ParseData.removeFirst();
            ParseMutex.unlock();
            if (tmpba.isEmpty())
                continue;
//                char *ParseDataOne = new char[ParseData.at(0).size()];
//                memcpy(ParseDataOne,ParseData.at(0).constData(),ParseData.at(0).size());
            int tmpi = isIncomeDataValid(tmpba);
            if (tmpi == I104_RCVNORM) // если поймали правильное начало данных, переходим к их обработке
            {
                if (APDUFormat == I104_I)
//                    ParseIFormat(&(tmpba.data()[6])); // без APCI
                {
                    tmpba = tmpba.mid(6);
                    ParseIFormat(tmpba); // без APCI
                }
            }

            if ((V_R>(AckVR+I104_W)) && !FileSending)
            {
                //AckVR += V_R;
                emit sendS();
                GetNewVR = true;
            }
        }
/*        QTime tmr;
        tmr.start();
        while (tmr.elapsed() < 10)*/
        QThread::msleep(10);
        qApp->processEvents();
    }
    emit Finished();
}

int Parse104::isIncomeDataValid(QByteArray ba)
{
    try
    {
        if (ba.at(0) != 0x68)
            return I104_RCVWRONG;
        APDULength = static_cast<quint8>(ba.at(1)); // в 1-м байте лежит длина
        if ((APDULength < 4) || (APDULength > 253))
        {
            emit error(M104_LENGTHER);
            return I104_RCVWRONG;
        }
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
                emit error(M104_NUMER);
    //            V_R = V_Rrcv+1;
                return I104_RCVWRONG;
            }
            /*if (GetNewVR)
            {
                V_R = V_Rrcv;
                GetNewVR = false;
                AckVR = V_R + I104_W;
            } */
            V_R++;

            quint16 V_Srcv = static_cast<quint8>(ba.at(5))*256+static_cast<quint8>(ba.at(4)&0xFE);
            V_Srcv >>= 1;
            if (V_Srcv != V_S)
            {
                V_S = V_Srcv;           // временно, нужно исправить проблему несовпадения s посылок
                emit error(M104_NUMER);
                //return I104_RCVWRONG;  // временно, нужно исправить проблему несовпадения s посылок
            }

            //if(FileSending)
            //noAnswer = 0;

            return I104_RCVNORM;
            break;
        }
        case I104_S:
        {
            quint16 V_Srcv = static_cast<quint8>(ba.at(5))*256+static_cast<quint8>(ba.at(4)&0xFE);
            V_Srcv >>= 1;
            if (V_Srcv != V_S)
            {
                V_S = V_Srcv;
                emit error(M104_NUMER);
                //return I104_RCVWRONG;
            }
    //        V_R++;
            return I104_RCVNORM;
            break;
        }
        case I104_U:
        {
            unsigned char baat2 = ba.at(2);
            if ((baat2 == I104_STARTDT_CON) && (cmd == I104_STARTDT_ACT)) // если пришло подтверждение старта и перед этим мы старт запрашивали
            {
                Timer104->stop();
                cmd = I104_STARTDT_CON;
                emit GeneralInter();
            }
            if ((baat2 == I104_STOPDT_CON) && (cmd == I104_STOPDT_ACT)) // если пришло подтверждение стопа и перед этим мы стоп запрашивали
                cmd = I104_STOPDT_CON;
            if ((baat2 == I104_TESTFR_CON) && (cmd == I104_TESTFR_ACT)) // если пришло подтверждение теста и перед этим мы тест запрашивали
                cmd = I104_TESTFR_CON;
            if (baat2 == I104_TESTFR_ACT)
                emit sendAct();

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

void Parse104::ParseIFormat(QByteArray &ba) // основной разборщик
{
   // char bacopy[100];
   // memcpy(&bacopy, ba, sizeof(ba));
    DUI.typeIdent = ba[0];
    DUI.qualifier.Number = ba[1]&0x7f;
    DUI.qualifier.SQ = ba[1]>>7;
    DUI.cause.cause = ba[2]&0x3F;
    DUI.cause.confirm = (ba[2]>>6)&0x01;
    DUI.cause.test = ba[2]>>7;
    DUI.cause.initiator = ba[3];
    DUI.commonAdrASDU = ba[4] + ba[5]*256;
    quint32 ObjectAdr = 0;
    quint32 index = 6;
    int FileSize;
    int res,i,cntfl = 0,cntflTimestamp = 0,cntspon = 0,cntbs = 0; //, cntsponTime = 0;
    Parse104::FlSignals104* flSignals = new Parse104::FlSignals104[DUI.qualifier.Number];
    Parse104::SponSignals* sponsignals = new Parse104::SponSignals[DUI.qualifier.Number];
/*    Parse104::SponSignals104* sponSignals = new Parse104::SponSignals104[DUI.qualifier.Number];
    Parse104::SponSignalsWithTime* SponSignalsWithTime = new Parse104::SponSignalsWithTime[DUI.qualifier.Number]; */
    Parse104::BS104Signals* BS104Signals = new Parse104::BS104Signals[DUI.qualifier.Number];
    unsigned char num = ba[9];

    for (i = 0; i < DUI.qualifier.Number; i++)
    {
        if ((i==0) || (DUI.qualifier.SQ==0))
        {

            ObjectAdr = ba[index++];
            ObjectAdr &= 0x00FF;
            ObjectAdr |= ba[index++]<<8;
            ObjectAdr += ba[index++]*0x10000;
        }
        else
            ObjectAdr++;
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
            (flSignals+cntflTimestamp)->fl.SigAdr=ObjectAdr;
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
        {
            if(DUI.cause.cause == 7)
                InterogateTimer->stop(); // !!!
            break;
        }

        case M_ME_NC_1:   // 13 тип - измеренные данные с плавающей запятой
        {
            if (cntfl >= DUI.qualifier.Number)
            {
                ERMSG("out of array flSignals");
                return;
            }
            (flSignals+cntfl)->fl.SigAdr=ObjectAdr;
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
//           (sponSignals)->Spon[cntspon].SigAdr=ObjectAdr;
           sponsignals->Spon[cntspon].SigAdr=ObjectAdr;
           quint8 value;
           memcpy(&value,&(ba.data()[index]),1);
           index += 1;
//           (sponSignals)->Spon[cntspon].SigVal=value;
           sponsignals->Spon[cntspon].SigVal=value;
           /*quint64 time;
           memcpy(&time,&(ba.data()[index]),8);
           index += 8;
           (sponSignals+cntspon)->Spon.CP56Time=time;*/

/*           if(ObjectAdr>=950 && ObjectAdr<953)
           emit UpdateReleWidget(sponSignals+cntspon); */

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
//            (SponSignalsWithTime)->Spon[cntsponTime].SigAdr=ObjectAdr;
            sponsignals->Spon[cntspon].SigAdr=ObjectAdr;
            quint8 value;
            memcpy(&value,&(ba.data()[index]),1);
            index += 1;
            sponsignals->Spon[cntspon].SigVal=value;
//            (SponSignalsWithTime)->Spon[cntsponTime].SigVal=value;
            quint64 time;
            memcpy(&time,&(ba.data()[index]),7);
            index += 7;
            sponsignals->Spon[cntspon].CP56Time=time;
//            (SponSignalsWithTime)->Spon[cntsponTime].CP56Time=time;

/*            if(ObjectAdr>=950 && ObjectAdr<953)
            emit UpdateReleWidget(sponSignals+cntsponTime); */

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
            (BS104Signals+cntbs)->BS.SigAdr[j]=(ObjectAdr>>8*j);

            quint32 value;
            memcpy(&value,&(ba.data()[index]),4);
            index += 4;
            (BS104Signals+cntbs)->BS.SigVal=value;
            quint8 quality;
            memcpy(&quality,&(ba.data()[index]),1);
            index++;
            (BS104Signals+cntbs)->BS.SigQuality=quality;
            //quint64 time;
            //memcpy(&time,&(ba.data()[index]),8);
            //index += 8;
            //(BS104Signals+cntbs)->BS.CP56Time=time;
            cntbs++;
            break;
        }

        case F_SR_NA_1: // секция готова
        {
            emit CallSection(static_cast<quint8>(ba[9]));
            break;
        }

        case F_FR_NA_1: // файл готов
        {
            ReadData.clear();
            RDSize = 0;
            RDLength = 0;
            FileSize = (static_cast<quint8>(ba[13]) << 16) | (static_cast<quint8>(ba[12]) << 8) | (static_cast<quint8>(ba[11]));

            emit CallFile(ba[9]);

            emit SetDataSizeFromParse(FileSize);

            break;
        }

        case F_SG_NA_1: // сегмент
        {
            RDSize = static_cast<quint8>(ba[12]);
            RDSize &= 0xFF;
            if(RDSize) //>= RDLength)
            {
                /*if (DR->isEmpty())
                {
                    break;
                }*/
                ReadData.append(&(ba.data()[13]), RDSize);
                RDLength += RDSize;
                emit SetDataCountFromParse(RDLength);

            }
            break;
        }

        case F_LS_NA_1: // последняя секция, последнй сегмент
        {

                switch(ba[12])
                {
                 case 1:
                 {
                      //emit sendConfirmFile(num);
                    FileSending = 0;
                    emit startConTimer();

                    int filetype = ba.at(9);
                      if(filetype == 0x01)  // если файл конфигурации
                      {
                       res = S2::RestoreDataMem(ReadData.data(), RDLength, DR);
                       if (res == NOERROR)
                       {
                        emit sendS2fromParse(DR);
                       }
                      }
                      else if(filetype == 0x04)   // если файл системного журнала
                      {
/*                       QVector<S2::DataRec> *DRJour = new QVector<S2::DataRec>;
                       DRJour->append({static_cast<quint32>(ReadData.data()[16]),static_cast<quint32>(ReadData.data()[20]),&ReadData.data()[24]});
                       memcpy(&DRJour->data()[0],&ReadData.data()[16],8);

                       res = S2::RestoreDataMem(ReadData.data(), RDLength, DRJour);
                       if (res == NOERROR)
                       {
                        emit sendJourSysfromParse(DRJour);
                       } */
                          emit sendJourSysfromParse(ReadData);
                      }
                      else if(filetype == 0x05)   // если файл рабочего журнала
                      {
/*                       QVector<S2::DataRec> *DRJour = new QVector<S2::DataRec>;
                       DRJour->append({static_cast<quint32>(ReadData.data()[16]),static_cast<quint32>(ReadData.data()[20]),&ReadData.data()[24]});
                       memcpy(&DRJour->data()[0],&ReadData.data()[16],8);

                       res = S2::RestoreDataMem(ReadData.data(), RDLength, DRJour);
                       if (res == NOERROR)
                       { */
//                        emit sendJourWorkfromParse(DRJour);
                          emit sendJourWorkfromParse(ReadData);
//                       }
                      }
                      else if(filetype == 6)   // если файл журнала измерений
                      {
/*                       QVector<S2::DataRec> *DRMJour = new QVector<S2::DataRec>;
                       DRMJour->append({static_cast<quint32>(ReadData.data()[16]),static_cast<quint32>(ReadData.data()[20]),&ReadData.data()[24]});
                       memcpy(&DRMJour->data()[0],&ReadData.data()[16],8);

                       res = S2::RestoreDataMem(ReadData.data(), RDLength, DRMJour);
                       if (res == NOERROR)
                       { */
//                        emit sendJourMeasfromParse(DRMJour);
                          emit sendJourMeasfromParse(ReadData);
//                       }
                      }


                      RDLength = 0;
                      RDSize = 0;

                       break;
                 }

                 case 3:
                 {
                  //incLS++;
                  emit SendConfirmSection(num);
                  break;
                 }

                }
            break;
        }

        case F_SC_NA_1: // запрос файла, секции
        {
            if(ba.at(12) == 0x02)  //запрос файла
            {
                emit sectionReady();
            }

            if(ba.at(12) == 0x06)
            {
                emit segmentReady();
            }
            break;
        }

        case F_AF_NA_1: // подтверждение файла, секции
        {
            //emit sendMessagefromParse();
            if(ba.at(12) == 0x03)  //подтверждение секции
            {
                emit LastSec();
            }

            if(ba.at(12) == 0x01)  //подтверждение секции
            {
                emit sendMessagefromParse();
            }

            break;
        }

        case C_SC_NA_1:
        {
            if(DUI.cause.cause == 10)
            {
                emit sendMessageOk();
            }

            break;


        }

        case C_SE_NC_1:
        {
            if(DUI.cause.cause == 10)
            count++;

            emit SetDataCountFromParse(count);
            quint32 adr = ba[6] + (ba[7]+1)*256; //+ (ba[8]<<16);

            if((adr == 920) && (DUI.cause.cause == 10))  // если адрес последнего параметра коррекции
            {
               if(ba.at(13) == 0)
               {
                   emit sendMessageOk();
                   emit SetDataCountFromParse(11);
               }
               //else
               //emit writeCorMesError()
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
        emit floatsignalsreceived(flSignals);
    }

    if(cntfl != 0)
    {
        flSignals->SigNumber = cntfl;
//        TimeFunc::Wait(100);
        emit floatsignalsreceived(flSignals);
    }

    if(cntspon != 0)
    {
        sponsignals->SigNumber = cntspon;
        emit sponsignalsreceived(sponsignals);
    }

/*    if(cntsponTime != 0)
    {
        SponSignalsWithTime->SigNumber = cntsponTime;
        emit sponsignalWithTimereceived(SponSignalsWithTime);
    } */

    if(cntbs != 0)
    {
        BS104Signals->SigNumber = cntbs;
        emit bs104signalsreceived(BS104Signals);
    }
}

void Parse104::Stop()
{
    ThreadMustBeFinished = true;
    Timer104->stop();
}

void Parse104::ErrMsg()
{
//    InterogateTimer->stop();
    //QDialog *dlg = new QDialog;
    //dlg->deleteLater();
/*    QVBoxLayout *lyout = new QVBoxLayout;
    EMessageBox::information(dlg, "Ошибка", "Таймаут по команде");
    dlg->setLayout(lyout);
    dlg->exec();*/
}

void Parse104::Timer104Start()
{
    Timer104->start();
}

void Parse104::Timer104Stop()
{
    Timer104->stop();
}

void IEC104::SelectFile(char numFile)
{
    APCI GI;
    ASDU Cmd;
    //char *ptr = static_cast<char*>(Cmd.data());
    quint16 VR = Parse->V_R;
    //int i;
    SecNum = 1;
    //void* temp = &SC;
    Parse->FileSending = 1;
    ConTimer->stop();
    /*SC.Ident.typeIdent = F_SC_NA_1;
    SC.Ident.qualifier = static_cast<char>(1);
    SC.Ident.cause = 13;
    SC.Ident.commonAdrASDU = BASEADR104;
    SC.AdrObj = 0;
    SC.FileName = static_cast<char>(1);
    SC.SectionName = 0;
    SC.SCQ = static_cast<char>(2);
    i = sizeof(SC);*/

    Log->info("SelectFile(" + QString::number(numFile) + ")");
    Cmd[0] = F_SC_NA_1;
    Cmd[1] = static_cast<char>(1);
    Cmd[2] = static_cast<char>(13);
    Cmd[3] = 0;
    Cmd[4] = static_cast<char>(BaseAdr);
    Cmd[5] = static_cast<char>(BaseAdr>>8);
    Cmd[6] = 0;
    Cmd[7] = 0;
    Cmd[8] = 0;
    Cmd[9] = numFile;
    Cmd[10] = 0;
    Cmd[11] = 0;
    Cmd[12] = 1;

    //Cmd.append(static_cast<char *>(temp),sizeof(SC));

    //memcpy(Cmd.data(), &SC.Ident.typeIdent, sizeof(SC));

    GI.start = I104_START;
    GI.APDUlength = 17;
    GI.contrfield[0] = (Parse->V_S & 0x007F) << 1;
    GI.contrfield[1] = (Parse->V_S & 0x7F80) >> 7;
    GI.contrfield[2] = (VR & 0x007F) << 1;
    GI.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;
    Send(1, GI, Cmd); // ASDU = QByteArray()

    //while(stopincrementing)
    //QThread::usleep(10);

    //Parse->V_S++;
}

void IEC104::CallFile(unsigned char numFile)
{
    APCI GI;
    ASDU Cmd;
    quint16 VR = Parse->V_R;

    Cmd[0] = F_SC_NA_1;
    Cmd[1] = static_cast<char>(1);
    Cmd[2] = static_cast<char>(13);
    Cmd[3] = 0;
    Cmd[4] = static_cast<char>(BaseAdr);
    Cmd[5] = static_cast<char>(BaseAdr>>8);
    Cmd[6] = 0;
    Cmd[7] = 0;
    Cmd[8] = 0;
    Cmd[9] = static_cast<char>(numFile);
    Cmd[10] = 0;
    Cmd[11] = 0;
    Cmd[12] = 2;

    //Cmd.append(static_cast<char *>(temp),sizeof(SC));

    //memcpy(Cmd.data(), &SC.Ident.typeIdent, sizeof(SC));

    GI.start = I104_START;
    GI.APDUlength = 17;
    GI.contrfield[0] = (Parse->V_S & 0x007F) << 1;
    GI.contrfield[1] = (Parse->V_S & 0x7F80) >> 7;
    GI.contrfield[2] = (VR & 0x007F) << 1;
    GI.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;
    Send(1, GI, Cmd); // ASDU = QByteArray()

    //while(stopincrementing)
    //QThread::usleep(10);

    //Parse->V_S++;
}

void IEC104::GetSection(unsigned char numFile)
{
    APCI GI;
    ASDU Cmd;
    //char *ptr = static_cast<char*>(Cmd.data());
    quint16 VR = Parse->V_R;
    //int i;
    //void* temp = &SC;

    /*SC.Ident.typeIdent = F_SC_NA_1;
    SC.Ident.qualifier = static_cast<char>(1);
    SC.Ident.cause = 13;
    SC.Ident.commonAdrASDU = BASEADR104;
    SC.AdrObj = 0;
    SC.FileName = static_cast<char>(1);
    SC.SectionName = 0;
    SC.SCQ = static_cast<char>(2);
    i = sizeof(SC);*/

    Cmd[0] = F_SC_NA_1;
    Cmd[1] = static_cast<char>(1);
    Cmd[2] = static_cast<char>(13);
    Cmd[3] = 0;
    Cmd[4] = static_cast<char>(BaseAdr);
    Cmd[5] = static_cast<char>(BaseAdr>>8);
    Cmd[6] = 0;
    Cmd[7] = 0;
    Cmd[8] = 0;
    Cmd[9] = static_cast<char>(numFile);
    Cmd[10] = 0;
    Cmd[11] = static_cast<char>(SecNum);
    Cmd[12] = 6;

    //Cmd.append(static_cast<char *>(temp),sizeof(SC));

    //memcpy(Cmd.data(), &SC.Ident.typeIdent, sizeof(SC));

    GI.start = I104_START;
    GI.APDUlength = 17;
    GI.contrfield[0] = (Parse->V_S & 0x007F) << 1;
    GI.contrfield[1] = (Parse->V_S & 0x7F80) >> 7;
    GI.contrfield[2] = (VR & 0x007F) << 1;
    GI.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;
    Send(1, GI, Cmd); // ASDU = QByteArray()

    //while(stopincrementing)
    //QThread::usleep(10);

    //Parse->V_S++;
}

void IEC104::ConfirmSection(unsigned char numFile)
{
    APCI GI;
    ASDU Cmd;
    //char *ptr = static_cast<char*>(Cmd.data());
    quint16 VR = Parse->V_R;
    //inc++;

    Cmd[0] = F_AF_NA_1;
    Cmd[1] = static_cast<char>(1);
    Cmd[2] = static_cast<char>(13);
    Cmd[3] = 0;
    Cmd[4] = static_cast<char>(BaseAdr);
    Cmd[5] = static_cast<char>(BaseAdr>>8);
    Cmd[6] = 0;
    Cmd[7] = 0;
    Cmd[8] = 0;
    Cmd[9] = static_cast<char>(numFile);
    Cmd[10] = 0;
    Cmd[11] = static_cast<char>(SecNum);
    Cmd[12] = static_cast<char>(3);

    GI.start = I104_START;
    GI.APDUlength = 17;
    GI.contrfield[0] = (Parse->V_S & 0x007F) << 1;
    GI.contrfield[1] = (Parse->V_S & 0x7F80) >> 7;
    GI.contrfield[2] = (VR & 0x007F) << 1;
    GI.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;

   // incSend++;

    Send(1, GI, Cmd); // ASDU = QByteArray()

    //while(stopincrementing)
    //QThread::usleep(10);

    //Parse->V_S++;

    SecNum++;
}

void IEC104::ConfirmFile(unsigned char numFile)
{
    APCI GI;
    ASDU Cmd;
    //char *ptr = static_cast<char*>(Cmd.data());
    quint16 VR = Parse->V_R;

    Cmd[0] = F_AF_NA_1;
    Cmd[1] = static_cast<char>(1);
    Cmd[2] = static_cast<char>(13);
    Cmd[3] = 0;
    Cmd[4] = static_cast<char>(BaseAdr);
    Cmd[5] = static_cast<char>(BaseAdr>>8);
    Cmd[6] = 0;
    Cmd[7] = 0;
    Cmd[8] = 0;
    Cmd[9] = static_cast<char>(numFile);
    Cmd[10] = 0;
    Cmd[11] = static_cast<char>(SecNum);
    Cmd[12] = static_cast<char>(1);

    GI.start = I104_START;
    GI.APDUlength = 17;
    GI.contrfield[0] = (Parse->V_S & 0x007F) << 1;
    GI.contrfield[1] = (Parse->V_S & 0x7F80) >> 7;
    GI.contrfield[2] = (VR & 0x007F) << 1;
    GI.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;
    Send(1, GI, Cmd); // ASDU = QByteArray()

    //while(stopincrementing)
    //QThread::usleep(10);

    //Parse->V_S++;
}

void IEC104::FileReady(QVector<S2::DataRec>* File)
{
    APCI GI;
    ASDU Cmd;

    //char *ptr = static_cast<char*>(Cmd.data());
    quint16 VR = Parse->V_R;
    //int i;

    Log->info("FileReady()");
    Parse->DR = File;

    SecNum = 1;

    Cmd[0] = F_FR_NA_1;
    Cmd[1] = static_cast<char>(1);
    Cmd[2] = static_cast<char>(13);
    Cmd[3] = 0;
    Cmd[4] = static_cast<char>(BaseAdr);
    Cmd[5] = static_cast<char>(BaseAdr>>8);
    Cmd[6] = 0;
    Cmd[7] = 0;
    Cmd[8] = 0;
    Cmd[9] = static_cast<char>(1);
    Cmd[10] = 0;
    Parse->File.resize(65535);
    S2::StoreDataMem(&(Parse->File.data()[0]), Parse->DR, 0x0001); // 0x0001 - номер файла конфигурации
    Parse->FileLen = static_cast<quint8>(Parse->File.data()[4]);
    Parse->FileLen += static_cast<quint8>(Parse->File.data()[5])*256;
    Parse->FileLen += static_cast<quint8>(Parse->File.data()[6])*65536;
    Parse->FileLen += static_cast<quint8>(Parse->File.data()[7])*16777216;
    Parse->FileLen += sizeof(S2::FileHeader); // FileHeader
    Cmd[11] = static_cast<char>(Parse->FileLen&0xFF);
    Cmd[12] = static_cast<char>((Parse->FileLen&0xFF00)>>8);
    Cmd[13] = static_cast<char>((Parse->FileLen&0xFF0000)>>16);

    //Cmd.append(static_cast<char *>(temp),sizeof(SC));

    //memcpy(Cmd.data(), &SC.Ident.typeIdent, sizeof(SC));

    GI.start = I104_START;
    GI.APDUlength = 18;
    GI.contrfield[0] = (Parse->V_S & 0x007F) << 1;
    GI.contrfield[1] = (Parse->V_S & 0x7F80) >> 7;
    GI.contrfield[2] = (VR & 0x007F) << 1;
    GI.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;
    Send(1, GI, Cmd); // ASDU = QByteArray()

    //while(stopincrementing)
    //QThread::usleep(10);

    //Parse->V_S++;
}

void IEC104::SectionReady()
{
    APCI GI;
    ASDU Cmd;
    QByteArray ba;
    //char *ptr = static_cast<char*>(Cmd.data());
    quint16 VR = Parse->V_R;
    Parse->firstSegment = 1;

    Cmd[0] = F_SR_NA_1;
    Cmd[1] = static_cast<char>(1);
    Cmd[2] = static_cast<char>(13);
    Cmd[3] = 0;
    Cmd[4] = static_cast<char>(BaseAdr);
    Cmd[5] = static_cast<char>(BaseAdr>>8);
    Cmd[6] = 0;
    Cmd[7] = 0;
    Cmd[8] = 0;
    Cmd[9] = static_cast<char>(1);
    Cmd[10] = 0;
    Cmd[11] = static_cast<char>(SecNum);
    Cmd[12] = static_cast<char>(Parse->FileLen&0xFF);
    Cmd[13] = static_cast<char>((Parse->FileLen&0xFF00)>>8);
    Cmd[14] = static_cast<char>((Parse->FileLen&0xFF0000)>>16);


    //Cmd.append(static_cast<char *>(temp),sizeof(SC));

    //memcpy(Cmd.data(), &SC.Ident.typeIdent, sizeof(SC));

    GI.start = I104_START;
    GI.APDUlength = 19;
    GI.contrfield[0] = (Parse->V_S & 0x007F) << 1;
    GI.contrfield[1] = (Parse->V_S & 0x7F80) >> 7;
    GI.contrfield[2] = (VR & 0x007F) << 1;
    GI.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;
    Send(1, GI, Cmd); // ASDU = QByteArray()

    //while(stopincrementing)
    //QThread::usleep(10);

    //if(Parse->FileLen > SECTIONSIZE)
    //SecNum++;

    //Parse->V_S++;
}

void IEC104::SendSegments()
{
    APCI GI;
    ASDU Cmd;
    QByteArray ba;
    //char *ptr = static_cast<char*>(Cmd.data());
    quint16 VR = Parse->V_R;
    quint32 leftsize = 0;
    quint32 i;
    KSS = 0;
    KSF = 0;
    Cmd.resize(256);

    Cmd[0] = F_SG_NA_1;
    Cmd[1] = static_cast<unsigned char>(1);
    Cmd[2] = static_cast<unsigned char>(13);
    Cmd[3] = 0;
    Cmd[4] = static_cast<unsigned char>(BaseAdr);
    Cmd[5] = static_cast<unsigned char>(BaseAdr>>8);
    Cmd[6] = 0;
    Cmd[7] = 0;
    Cmd[8] = 0;
    Cmd[9] = static_cast<unsigned char>(1);
    Cmd[10] = 0;
    Cmd[11] = static_cast<char>(SecNum);
    //Cmd[12] =  static_cast<char>(SEGMENTSIZE);

    Parse->Pos = OFFSET;
    //memcpy(&Cmd.data()[Parse->Pos], Parse->File, FHSIZE);
    //Parse->Pos += FHSIZE;
    /*Parse->File.data()[12] = 0;
    Parse->File.data()[13] = 0;
    Parse->File.data()[14] = 0;
    Parse->File.data()[15] = 0;*/
    emit SetDataSize(Parse->FileLen);

    if(Parse->FileLen > SEGMENTSIZE)
    {
       memcpy(&Cmd.data()[Parse->Pos], &Parse->File.data()[0], SEGMENTSIZE);
       Cmd.resize(SEGMENTSIZE+OFFSET);
       leftsize = Parse->FileLen - SEGMENTSIZE;
       for(i = 0; i<SEGMENTSIZE; i++)
       {
          KSS += static_cast<quint8>(Parse->File.data()[i]);
          KSF += static_cast<quint8>(Parse->File.data()[i]);
       }
       Parse->Pos += SEGMENTSIZE;
       emit SetDataCount(Parse->Pos-OFFSET);
    }
    else
    {
       memcpy(&Cmd.data()[Parse->Pos], &Parse->File.data()[0], Parse->FileLen);

       Cmd.resize(Parse->Pos);
       leftsize = 0;

       for(i = 0; i<Parse->FileLen; i++)
       {
          KSS += static_cast<quint8>(Parse->File.data()[i]);
          KSF += static_cast<quint8>(Parse->File.data()[i]);
       }

       Parse->Pos += Parse->FileLen;
       emit SetDataCount(Parse->Pos-OFFSET);
    }

    Cmd[12] =  static_cast<unsigned char>(Parse->Pos - OFFSET);
    //Parse->firstSegment = 0;




    //Cmd.append(static_cast<char *>(temp),sizeof(SC));

    //memcpy(Cmd.data(), &SC.Ident.typeIdent, sizeof(SC));

    GI.start = I104_START;
    GI.APDUlength = (Cmd[12]+17);
    GI.contrfield[0] = (Parse->V_S & 0x007F) << 1;
    GI.contrfield[1] = (Parse->V_S & 0x7F80) >> 7;
    GI.contrfield[2] = (VR & 0x007F) << 1;
    GI.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;
    Send(1, GI, Cmd); // ASDU = QByteArray()

    QThread::msleep(20);

    //while(stopincrementing)
    //QThread::usleep(10);

    //Parse->V_S++;

    while(leftsize)
    {
        if(leftsize > SEGMENTSIZE) //Parse->FileLen + OFFSET + FHSIZE - Parse->Pos
        {
            memcpy(&Cmd.data()[OFFSET], &Parse->File.data()[Parse->Pos-OFFSET], SEGMENTSIZE);

            Cmd.resize(SEGMENTSIZE+OFFSET);
            leftsize = leftsize - SEGMENTSIZE;
            Cmd[12] =  static_cast<unsigned char>(SEGMENTSIZE);

            for(i = 0; i<SEGMENTSIZE; i++)
            {
               KSS += static_cast<quint8>(Parse->File.data()[Parse->Pos-OFFSET+i]);
               KSF += static_cast<quint8>(Parse->File.data()[Parse->Pos-OFFSET+i]);
            }

            Parse->Pos += SEGMENTSIZE;
            emit SetDataCount(Parse->Pos-OFFSET);
        }
        else
        {
            memcpy(&Cmd.data()[OFFSET], &Parse->File.data()[Parse->Pos-OFFSET], leftsize);
            //Parse->Pos += leftsize;
            Cmd.resize(leftsize + OFFSET);
            Cmd[12] =  static_cast<char>(leftsize);

            for(i = 0; i<leftsize; i++)
            {
               KSS += static_cast<quint8>(Parse->File.data()[Parse->Pos-OFFSET+i]);
               KSF += static_cast<quint8>(Parse->File.data()[Parse->Pos-OFFSET+i]);
            }

            Parse->Pos += leftsize;
            emit SetDataCount(Parse->Pos-OFFSET);
            leftsize = 0;
        }

        GI.start = I104_START;
        GI.APDUlength = (Cmd[12]+17);
        GI.contrfield[0] = (Parse->V_S & 0x007F) << 1;
        GI.contrfield[1] = (Parse->V_S & 0x7F80) >> 7;
        GI.contrfield[2] = (VR & 0x007F) << 1;
        GI.contrfield[3] = (VR & 0x7F80) >> 7;
        Parse->cmd = I104_S;
        Send(1, GI, Cmd);

        QThread::msleep(500);

        //while(stopincrementing)
        //QThread::usleep(10);

        //Parse->V_S++;

        if(leftsize == 0)
        emit LastSeg();
    }
}

void IEC104::LastSegment()
{
    APCI GI;
    ASDU Cmd;
    QByteArray ba;
    //char *ptr = static_cast<char*>(Cmd.data());
    quint16 VR = Parse->V_R;
    //Parse->firstSegment = 1;

    Cmd[0] = F_LS_NA_1;
    Cmd[1] = static_cast<char>(1);
    Cmd[2] = static_cast<char>(13);
    Cmd[3] = 0;
    Cmd[4] = static_cast<char>(BaseAdr);
    Cmd[5] = static_cast<char>(BaseAdr>>8);
    Cmd[6] = 0;
    Cmd[7] = 0;
    Cmd[8] = 0;
    Cmd[9] = static_cast<char>(1);
    Cmd[10] = 0;
    Cmd[11] = static_cast<char>(SecNum);
    Cmd[12] = static_cast<char>(3);
    Cmd[13] = static_cast<char>(KSS);
    //Cmd[14] = static_cast<char>((Parse->FileLen&0xFF0000)>>16);

    KSS = 0;
    //Cmd.append(static_cast<char *>(temp),sizeof(SC));

    //memcpy(Cmd.data(), &SC.Ident.typeIdent, sizeof(SC));

    GI.start = I104_START;
    GI.APDUlength = 18;
    GI.contrfield[0] = (Parse->V_S & 0x007F) << 1;
    GI.contrfield[1] = (Parse->V_S & 0x7F80) >> 7;
    GI.contrfield[2] = (VR & 0x007F) << 1;
    GI.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;

    QThread::msleep(500);

    Send(1, GI, Cmd); // ASDU = QByteArray()

    //while(stopincrementing)
    //QThread::usleep(10);

    //if(Parse->FileLen > SECTIONSIZE)
    //SecNum++;

    //Parse->V_S++;
}

void IEC104::LastSection()
{
    APCI GI;
    ASDU Cmd;
    QByteArray ba;
    //char *ptr = static_cast<char*>(Cmd.data());
    quint16 VR = Parse->V_R;
    //Parse->firstSegment = 1;

    Cmd[0] = F_LS_NA_1;
    Cmd[1] = static_cast<char>(1);
    Cmd[2] = static_cast<char>(13);
    Cmd[3] = 0;
    Cmd[4] = static_cast<char>(BaseAdr);
    Cmd[5] = static_cast<char>(BaseAdr>>8);
    Cmd[6] = 0;
    Cmd[7] = 0;
    Cmd[8] = 0;
    Cmd[9] = static_cast<char>(1);
    Cmd[10] = 0;
    Cmd[11] = static_cast<char>(SecNum);;
    Cmd[12] = static_cast<char>(1);
    Cmd[13] = static_cast<char>(KSF);

    KSF = 0;

    GI.start = I104_START;
    GI.APDUlength = 18;
    GI.contrfield[0] = (Parse->V_S & 0x007F) << 1;
    GI.contrfield[1] = (Parse->V_S & 0x7F80) >> 7;
    GI.contrfield[2] = (VR & 0x007F) << 1;
    GI.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;
    Send(1, GI, Cmd); // ASDU = QByteArray()

    //while(stopincrementing)
    //QThread::usleep(10);

    //if(Parse->FileLen > SECTIONSIZE)
    //SecNum++;

    //Parse->V_S++;
}

void IEC104::Com45(quint32 com)
{
    APCI GI;
    ASDU Cmd;
    QByteArray ba;
    //char *ptr = static_cast<char*>(Cmd.data());
    quint16 VR = Parse->V_R;
    //Parse->firstSegment = 1;

    Log->info("Com45()");
    Cmd[0] = C_SC_NA_1;
    Cmd[1] = static_cast<char>(1);
    Cmd[2] = static_cast<char>(6);
    Cmd[3] = 0;
    Cmd[4] = static_cast<char>(BaseAdr);
    Cmd[5] = static_cast<char>(BaseAdr>>8);
    Cmd[6] = static_cast<char>(com);
    Cmd[7] = static_cast<char>(com>>8);
    Cmd[8] = static_cast<char>(com>>16);
    Cmd[9] = static_cast<char>(1);

    GI.start = I104_START;
    GI.APDUlength = 14;
    GI.contrfield[0] = (Parse->V_S & 0x007F) << 1;
    GI.contrfield[1] = (Parse->V_S & 0x7F80) >> 7;
    GI.contrfield[2] = (VR & 0x007F) << 1;
    GI.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;
    Send(1, GI, Cmd); // ASDU = QByteArray()

    //while(stopincrementing)
    //QThread::usleep(10);

    //if(Parse->FileLen > SECTIONSIZE)
    //SecNum++;

    //Parse->V_S++;
}

void IEC104::Com50(quint32* adr, float *param)
{
    APCI GI;
    ASDU Cmd;
    QByteArray *ba = new QByteArray;
    ba->resize(4);
    //char *ptr = static_cast<char*>(Cmd.data());
    quint16 VR = Parse->V_R;

    Log->info("Com50()");
    if(*adr == 910)
    emit SetDataSize(11);

    memcpy(&ba->data()[0], (quint8*)param, sizeof(float));
    ba->toHex();

    Cmd[0] = C_SE_NC_1;
    Cmd[1] = static_cast<unsigned char>(1);
    Cmd[2] = static_cast<unsigned char>(6);
    Cmd[3] = 0;
    Cmd[4] = static_cast<unsigned char>(BaseAdr);
    Cmd[5] = static_cast<unsigned char>(BaseAdr>>8);
    Cmd[6] = static_cast<unsigned char>(*adr);
    Cmd[7] = static_cast<unsigned char>(*adr>>8);
    Cmd[8] = static_cast<unsigned char>(*adr>>16);
    //Cmd[9] = static_cast<char>(1);
    Cmd[9] = static_cast<unsigned char>(ba->data()[0]);
    Cmd[10] = static_cast<unsigned char>(ba->data()[1]);
    Cmd[11] = static_cast<unsigned char>(ba->data()[2]);
    Cmd[12] = static_cast<unsigned char>(ba->data()[3]);
    Cmd[13] = 0;

    GI.start = I104_START;
    GI.APDUlength = 18;
    GI.contrfield[0] = (Parse->V_S & 0x007F) << 1;
    GI.contrfield[1] = (Parse->V_S & 0x7F80) >> 7;
    GI.contrfield[2] = (VR & 0x007F) << 1;
    GI.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;
    Send(1, GI, Cmd); // ASDU = QByteArray()

    //while(stopincrementing)
    //QThread::usleep(10);

    //if(Parse->FileLen > SECTIONSIZE)
    //SecNum++;

    //Parse->V_S++;
}

void IEC104::InterrogateTimeGr15()
{
    APCI GI;
    ASDU GTime;
    quint16 VR = Parse->V_R;

    Log->info("InterrogateTimeGr15()");

    GTime[0] = static_cast<char>(C_IC_NA_1);
    GTime[1] = static_cast<char>(1);
    GTime[2] = static_cast<char>(6);
    GTime[3] = static_cast<char>(0);
    GTime[4] = static_cast<char>(BaseAdr);
    GTime[5] = static_cast<char>(BaseAdr>>8);
    GTime[6] = static_cast<char>(0);
    GTime[7] = static_cast<char>(0);
    GTime[8] = static_cast<char>(0);
    GTime[9] = static_cast<char>(35);

    GI.start = I104_START;
    GI.APDUlength = 14;
    GI.contrfield[0] = (Parse->V_S & 0x007F) << 1;
    GI.contrfield[1] = (Parse->V_S & 0x7F80) >> 7;
    GI.contrfield[2] = (VR & 0x007F) << 1;
    GI.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;
    Send(1, GI, GTime); // ASDU = QByteArray()

    Parse->AckVR = Parse->V_R;

    //while(stopincrementing)
    //QThread::usleep(10);

   // Parse->V_S++;

    Parse->InterogateTimer->start();

}

void IEC104::com51WriteTime(uint Time)
{
    APCI GI;
    ASDU Cmd;
    QByteArray *ba = new QByteArray;
    quint32 adr = 4601;
    ba->resize(4);
    //char *ptr = static_cast<char*>(Cmd.data());
    quint16 VR = Parse->V_R;

    Log->info("Com51()");
    memcpy(&ba->data()[0], &Time, sizeof(uint));
    //ba->toHex();

    Cmd[0] = C_BO_NA_1;
    Cmd[1] = static_cast<char>(1);
    Cmd[2] = static_cast<char>(6);
    Cmd[3] = 0;
    Cmd[4] = static_cast<char>(BaseAdr);
    Cmd[5] = static_cast<char>(BaseAdr>>8);
    Cmd[6] = static_cast<char>(adr);
    Cmd[7] = static_cast<char>(adr>>8);
    Cmd[8] = static_cast<char>(adr>>16);
    //Cmd[9] = static_cast<char>(1);
    Cmd[9] = static_cast<char>(ba->data()[0]);
    Cmd[10] = static_cast<char>(ba->data()[1]);
    Cmd[11] = static_cast<char>(ba->data()[2]);
    Cmd[12] = static_cast<char>(ba->data()[3]);
    //Cmd[13] = 0;

    GI.start = I104_START;
    GI.APDUlength = 17;
    GI.contrfield[0] = (Parse->V_S & 0x007F) << 1;
    GI.contrfield[1] = (Parse->V_S & 0x7F80) >> 7;
    GI.contrfield[2] = (VR & 0x007F) << 1;
    GI.contrfield[3] = (VR & 0x7F80) >> 7;
    Parse->cmd = I104_S;
    Send(1, GI, Cmd); // ASDU = QByteArray()

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

void IEC104::StartConTimer()
{
    ConTimer->start();
}

