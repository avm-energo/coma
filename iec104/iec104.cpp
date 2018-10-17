#include <QTime>
#include <QThread>
#include <QCoreApplication>

//#include "../gen/stdfunc.h"
#include "../gen/error.h"
#include "iec104.h"
#include "ethernet.h"

iec104::iec104(QObject *parent) : QObject(parent)
{
    ParseStarted = false;
    GSD = true;
    QThread *thr = new QThread;
    ethernet *eth = new ethernet;
    eth->moveToThread(thr);
    connect(thr,SIGNAL(finished()),eth,SLOT(deleteLater()));
    connect(thr,SIGNAL(finished()),thr,SLOT(deleteLater()));
    connect(thr,SIGNAL(started()),eth,SLOT(Run()));
    connect(this,SIGNAL(stopall()),eth,SLOT(Stop()));
    connect(eth,SIGNAL(connected()),this,SIGNAL(ethconnected()));
    connect(eth,SIGNAL(disconnected()),this,SIGNAL(ethdisconnected()));
    connect(eth,SIGNAL(connected()),this,SLOT(Start()));
    connect(eth,SIGNAL(newdataarrived(QByteArray)),this,SLOT(GetSomeData(QByteArray)));
    connect(this,SIGNAL(writedatatoeth(QByteArray)),eth,SLOT(InitiateWriteDataToPort(QByteArray)));

    Parse = new Parse104;
    QThread *thr2 = new QThread;
    Parse->moveToThread(thr2);
    connect(this,SIGNAL(stopall()),Parse,SLOT(Stop()));
    connect(thr2,SIGNAL(finished()),Parse,SLOT(deleteLater()));
    connect(thr2,SIGNAL(finished()),thr2,SLOT(deleteLater()));
    connect(thr2,SIGNAL(started()),Parse,SLOT(Run()));
    connect(Parse,SIGNAL(signalsreceived(Parse104::Signals104&)),\
            this,SIGNAL(signalsready(Parse104::Signals104&)),Qt::BlockingQueuedConnection);
    connect(Parse,SIGNAL(sendS()),this,SLOT(SendS()));
    connect(Parse,SIGNAL(parsestarted()),this,SLOT(StartParse()));
    thr->start();
    thr2->start();
}

iec104::~iec104()
{
}

void iec104::StartParse()
{
    ParseStarted = true;
}

void iec104::Start()
{
    APCI StartDT;
    StartDT.start = I104_START;
    StartDT.APDUlength = 4;
    StartDT.contrfield[0] = I104_STARTDT_ACT;
    StartDT.contrfield[1] = StartDT.contrfield[2] = StartDT.contrfield[3] = 0;
    Parse->cmd = I104_STARTDT_ACT;
    Send(StartDT); // ASDU = QByteArray()
}

void iec104::Stop()
{
    APCI StopDT;
    StopDT.start = I104_START;
    StopDT.APDUlength = 4;
    StopDT.contrfield[0] = I104_STOPDT_ACT;
    StopDT.contrfield[1] = StopDT.contrfield[2] = StopDT.contrfield[3] = 0;
    Parse->cmd = I104_STOPDT_ACT;
    Send(StopDT); // ASDU = QByteArray()
    QTime tmr;
    tmr.start();
    while (tmr.elapsed() < 1000)
        qApp->processEvents();
    emit stopall();
}

void iec104::Send(APCI apci, ASDU asdu)
{
    QByteArray ba;
    ba.clear();
    void *tempp = &apci;
    ba.append(static_cast<char *>(tempp),sizeof(apci));
    if (!asdu.isEmpty())
    {
        tempp = &asdu;
        ba.append(static_cast<char *>(tempp),sizeof(asdu));
    }
    emit writedatatoeth(ba);
}

void iec104::GetSomeData(QByteArray ba)
{
    Parse->ParseMutex.lock();
    ParseSomeData(ba, true);
    Parse->ParseMutex.unlock();
}

void iec104::ParseSomeData(QByteArray ba, bool GSD)
{
    quint32 basize = static_cast<quint32>(ba.size());
    if (GSD)
    {
        if (cutpckt.size()>1)
        {
            quint32 cutpcktlen = static_cast<quint8>(cutpckt.at(1));
            cutpcktlen += 2;
            quint32 cutpcktsize = static_cast<quint32>(cutpckt.size());
            quint32 missing_num = cutpcktlen-cutpcktsize; // взяли длину остатка от предыдущего пакета
            if (missing_num>basize)
            {
                cutpckt.append(ba);
                return; // если так и не достигли конца пакета, надо брать следующий пакет в cutpckt
            }
            cutpckt.append(ba.left(missing_num)); // взяли из текущего пакета сами байты
            ba.remove(0,missing_num);
            Parse->ParseData.append(cutpckt);
            cutpckt.clear();
            basize = static_cast<quint32>(ba.size());
        }
    }
    if (basize<1)
        return;
    quint32 BlockLength = static_cast<quint8>(ba.at(1))+2;
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
    }
}

void iec104::SendS()
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
    Send(Confirm); // ASDU = QByteArray()
}

// Класс PARSE104

Parse104::Parse104(QObject *parent) : QObject(parent)
{
    ParseMutex.lock();
    ParseData.clear();
    ParseMutex.unlock();
    ThreadMustBeFinished = false;
    V_S = V_R = 0;
    AckVR = I104_W;
    APDUFormat = I104_WRONG;
    GetNewVR = false;
    NewDataArrived = false;
}

Parse104::~Parse104()
{
}

void Parse104::Run()
{
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
                    ParseIFormat(&(tmpba.data()[6])); // без APCI
            }
            if (V_R>AckVR)
            {
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
}

int Parse104::isIncomeDataValid(QByteArray ba)
{
    if (ba.at(0) != 0x68)
        return I104_RCVWRONG;
    APDULength = static_cast<quint8>(ba.at(1)); // в 1-м байте лежит длина
    if ((APDULength<4) || (APDULength>253))
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
/*        if (GetNewVR)
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
            emit error(M104_NUMER);
            return I104_RCVWRONG;
        }
        return I104_RCVNORM;
        break;
    }
    case I104_S:
    {
        quint16 V_Srcv = static_cast<quint8>(ba.at(5))*256+static_cast<quint8>(ba.at(4)&0xFE);
        V_Srcv >>= 1;
        if (V_Srcv != V_S)
        {
            emit error(M104_NUMER);
            return I104_RCVWRONG;
        }
//        V_R++;
        return I104_RCVNORM;
        break;
    }
    case I104_U:
    {
        if ((ba.at(2) == I104_STARTDT_CON) && (cmd == I104_STARTDT_ACT)) // если пришло подтверждение старта и перед этим мы старт запрашивали
            cmd = I104_STARTDT_CON;
        if ((ba.at(2) == I104_STOPDT_CON) && (cmd == I104_STOPDT_ACT)) // если пришло подтверждение стопа и перед этим мы стоп запрашивали
            cmd = I104_STOPDT_CON;
        if ((ba.at(2) == I104_TESTFR_CON) && (cmd == I104_TESTFR_ACT)) // если пришло подтверждение теста и перед этим мы тест запрашивали
            cmd = I104_TESTFR_CON;
        return I104_RCVNORM;
        break;
    }
    default:
        break;
    }
    return I104_RCVWRONG;
}

void Parse104::ParseIFormat(const char *ba) // основной разборщик
{
    DUI.typeIdent = ba[0];
    DUI.qualifier.Number = ba[1]&0x7f;
    DUI.qualifier.SQ = ba[1]>>7;
    DUI.cause.cause = ba[2]&0x3F;
    DUI.cause.confirm = (ba[2]>>6)&0x01;
    DUI.cause.test = ba[2]>>7;
    DUI.cause.initiator = ba[3];
    DUI.commonAdrASDU = ba[4] + ba[5]*256;
    quint32 ObjectAdr;
    quint32 index = 6;
    int i;
    for (i = 0; i < DUI.qualifier.Number; i++)
    {
        if ((i==0) || (DUI.qualifier.SQ==0))
        {
            ObjectAdr = ba[index++];
            ObjectAdr += ba[index++]*0x100;
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
            if (ObjectAdr > 45) // для регулировки Э значения сигналов МИПа с индексом более 45 не требуются
            {
                index += 12;
                break;
            }
            Signals104 Signal;
            Signal.SigNum=ObjectAdr;
            float value;
            memcpy(&value,&ba[index],4);
            index += 4;
            Signal.SigVal=value;
            quint8 quality;
            memcpy(&quality,&ba[index],1);
            index++;
            Signal.SigQuality=quality;
            quint64 time;
            memcpy(&time,&ba[index],7);
            index += 7;
            Signal.CP56Time=time;
            emit signalsreceived(Signal);
            break;
        }
        default:
            break;
        }
    }
}

void Parse104::Stop()
{
    ThreadMustBeFinished = true;
}
