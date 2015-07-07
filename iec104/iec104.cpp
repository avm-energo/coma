#include <QThread>
#include <QTime>
#include <QCoreApplication>

#include "iec104.h"
#include "ethernet.h"
#include "../publicclass.h"

iec104::iec104(QObject *parent) : QObject(parent)
{
    QThread *thr = new QThread;
    ethernet *eth = new ethernet;
    eth->moveToThread(thr);
    connect(thr, SIGNAL(started()), eth, SLOT(run()));
    connect(eth,SIGNAL(finished()),thr,SLOT(quit()));
    connect(this,SIGNAL(stopall()),eth,SLOT(stop()));
    connect(eth,SIGNAL(finished()),eth,SLOT(deleteLater()));
    connect(thr,SIGNAL(finished()),thr,SLOT(deleteLater()));
    connect(eth,SIGNAL(connected()),this,SIGNAL(ethconnected()));
    connect(eth,SIGNAL(connected()),this,SLOT(Start()));
    connect(eth,SIGNAL(error(int)),this,SIGNAL(error(int)));
    connect(eth,SIGNAL(newdataarrived(QByteArray)),this,SLOT(GetSomeData(QByteArray)));
    connect(this,SIGNAL(writedatatoeth(QByteArray)),eth,SLOT(InitiateWriteDataToPort(QByteArray)));

    QThread *ParseThread = new QThread;
    Parse = new Parse104;
    Parse->moveToThread(ParseThread);
    connect(ParseThread,SIGNAL(started()),Parse,SLOT(ParseIncomeData()));
    connect(Parse,SIGNAL(finished()),ParseThread,SLOT(quit()));
    connect(this,SIGNAL(stopall()),Parse,SLOT(stop()));
    connect(Parse,SIGNAL(finished()),Parse,SLOT(deleteLater()));
    connect(ParseThread,SIGNAL(finished()),ParseThread,SLOT(deleteLater()));

    connect(Parse,SIGNAL(signalsreceived()),this,SLOT(SignalsGot()));
    connect(Parse,SIGNAL(sendS()),this,SLOT(SendS()));
    ParseThread->start();
    thr->start();
}

iec104::~iec104()
{
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
    emit readdatafrometh(ba);
    if (Parse->ReceiverBusy) // выкидываем посылки свыше w
        return;
    Parse->ReadDataMutex.lock();
    Parse->ReadData.append(ba);
    Parse->ReadDataSize = Parse->ReadData.size();
    Parse->ReadDataMutex.unlock();
}

void iec104::SignalsGot()
{
    Parse->SignalsMutex.lock();
    Signals.SigNum=Parse->Signals.SigNum;
    Signals.SigQuality=Parse->Signals.SigQuality;
    Signals.SigVal=Parse->Signals.SigVal;
    Signals.CP56Time=Parse->Signals.CP56Time;
    Parse->Signals.SigNum.clear();
    Parse->Signals.SigVal.clear();
    Parse->Signals.SigQuality.clear();
    Parse->Signals.CP56Time.clear();
    Parse->SignalsMutex.unlock();
    emit signalsready();
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
    ReadData.clear();
    ThreadMustBeFinished = false;
    V_S = V_R = 0;
    AckVR = I104_W;
    APDUFormat = I104_WRONG;
    ReceiverBusy = false;
    GetNewVR = false;
}

Parse104::~Parse104()
{
}

void Parse104::ParseIncomeData()
{
    int stpos=0;
    while (!ThreadMustBeFinished)
    {
        ReadDataMutex.lock();
        if (ReadData.size()) // если в буфере есть необработанные данные
        {
            if (V_R > AckVR) // принято w и более посылок
            {
                ReceiverBusy = true;
            }
        }
        else
        {
            if (ReceiverBusy)
            {
                ReceiverBusy = false;
                emit sendS();
                GetNewVR = true;
            }
        }
        if ((stpos = ReadData.indexOf(0x68,stpos)) != -1)
        {
            ReadData.remove(0, stpos); // оставляем только нужные для обработки байты
            stpos=1; // раз отбросили всё, что было слева до 0х68, то начинать надо будет со следующего после 0х68 байта
            int tmpi = isIncomeDataValid(ReadData);
            if (tmpi == I104_RCVNORM) // если поймали правильное начало данных, переходим к их обработке
            {
                stpos=0;
                ReadData.remove(0,6); // отбрасываем начальные байты вместе с APCI, ибо уже не нужны
                if (APDUFormat == I104_I)
                    ParseIFormat(ReadData);
            }
            else if (tmpi == I104_RCVSMSIZE) // данных недостаточно, надо подождать и набрать ещё
                stpos = 0; // возвращаем указатель поиска, чтобы не пропустить посылку
            // в противном случае переход к следующему вхождению 0х68
        }
        else
        {
            ReadData.clear();
            stpos = 0;
        }
        ReadDataMutex.unlock();
        QTime tmr;
        tmr.start();
        while (tmr.elapsed() < 100)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    emit finished();
}

int Parse104::isIncomeDataValid(QByteArray ba)
{
    if (ba.size()<6) // нет APCI, рано принимать решение по правильности блока
        return I104_RCVSMSIZE;
    APDULength = ba.at(1); // в 1-м байте лежит длина
    if ((APDULength<4) || (APDULength>253))
    {
        emit error(M104_LENGTHER);
        return I104_RCVWRONG;
    }
    if (!(ba.at(2)&0x01)) // I
    {
        APDUFormat = I104_I;
    }
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
        quint16 V_Rrcv = static_cast<quint16>(ba.at(3))*256+static_cast<quint16>(ba.at(2)&0xFE);
        V_Rrcv >>= 1;
        if (GetNewVR)
        {
            V_R = V_Rrcv;
            GetNewVR = false;
            AckVR = V_R + I104_W;
        }
        if (V_Rrcv != V_R)
        {
            emit error(M104_NUMER);
            return I104_RCVWRONG;
        }
        quint16 V_Srcv = static_cast<quint16>(ba.at(5))*256+static_cast<quint16>(ba.at(4)&0xFE);
        V_Srcv >>= 1;
        if (V_Srcv != V_S)
        {
            emit error(M104_NUMER);
            return I104_RCVWRONG;
        }
        V_R++;
        return I104_RCVNORM;
        break;
    }
    case I104_S:
    {
        quint16 V_Srcv = static_cast<quint16>(ba.at(5))*256+static_cast<quint16>(ba.at(4)&0xFE);
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

void Parse104::ParseIFormat(QByteArray ba) // основной разборщик
{
    SignalsMutex.lock();
    DUI.typeIdent = ba.at(0);
    DUI.qualifier.Number = ba.at(1)&0x7f;
    DUI.qualifier.SQ = ba.at(1)>>7;
    DUI.cause.cause = ba.at(2)&0x3F;
    DUI.cause.confirm = (ba.at(2)>>6)&0x01;
    DUI.cause.test = ba.at(2)>>7;
    DUI.cause.initiator = ba.at(3);
    DUI.commonAdrASDU = ba.at(4) + ba.at(5)*256;
    quint32 ObjectAdr;
    quint32 index = 6;
    int i;
    for (i = 0; i < DUI.qualifier.Number; i++)
    {
        if ((i==0) || (DUI.qualifier.SQ==0))
        {
            ObjectAdr = ba.at(index++);
            ObjectAdr += ba.at(index++)*0x100;
            ObjectAdr += ba.at(index++)*0x10000;
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
            if (ObjectAdr > 17) // для регулировки Э значения сигналов МИПа с индексом более 17 не требуются
            {
                index += 12;
                break;
            }
            Signals.SigNum.append(ObjectAdr);
            float value;
            memcpy(&value,&ba.data()[index],4);
            index += 4;
            Signals.SigVal.append(QString::number(value,'g',6));
            quint8 quality;
            memcpy(&quality,&ba.data()[index],1);
            index++;
            Signals.SigQuality.append(quality);
            quint64 time;
            memcpy(&time,&ba.data()[index],7);
            index += 7;
            Signals.CP56Time.append(time);
            break;
        }
        default:
            break;
        }
    }
    SignalsMutex.unlock();
    emit signalsreceived();
}

void Parse104::stop()
{
    ThreadMustBeFinished = true;
}
