#include <QThread>
#include <QTime>
#include <QCoreApplication>

#include "iec104.h"
#include "ethernet.h"
#include "../publicclass.h"

iec104::iec104(QObject *parent) : QObject(parent)
{
    ReadData.clear();
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
    connect(eth,SIGNAL(newdataarrived(QByteArray)),this,SLOT(ParseIncomeData(QByteArray)));
    connect(this,SIGNAL(writedatatoeth(QByteArray)),eth,SLOT(InitiateWriteDataToPort(QByteArray)));
    thr->start();
    Ack = V_S = V_R = 0;
    APDUFormat = I104_WRONG;
    Started = false;
    IncomingDisabled = false;
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
    cmd = I104_STARTDT_ACT;
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
    V_S++;
    emit writedatatoeth(ba);
}

void iec104::ParseIncomeData(QByteArray ba)
{
    if (!IncomingDisabled)
    {
        IncomingDisabled = true;
        emit readdatafrometh(ba);
        ReadData.append(ba);
        int stpos=0;
        while ((stpos = ReadData.indexOf(0x68,stpos)) != -1)
        {
    /*        if (ReadData.size() > stpos) // если после 0х68 есть ещё хоть один символ
                from = stpos+1; // следующий поиск начинаем со следующего символа */
            QByteArray tmpba = ReadData.right(ReadData.size()-stpos); // оставляем только нужные для обработки байты
            stpos++;
            int tmpi = isIncomeDataValid(tmpba);
            if (tmpi == I104_RCVNORM) // если поймали правильное начало данных, переходим к их обработке
            {
                ReadData = tmpba.right(tmpba.size()-6); // отбрасываем начальные байты вместе с APCI, ибо уже не нужны
                if (APDUFormat == I104_I)
                    ParseIFormat(ReadData);
            }
            else if (tmpi == I104_RCVSMSIZE) // данных недостаточно, надо подождать и набрать ещё
            {
                IncomingDisabled = false;
                return;
            }
            // в противном случае переход к следующему вхождению 0х68
        }
        QTime tmr;
        tmr.start();
        while (tmr.elapsed() < 100)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        IncomingDisabled = false;
    }
}

int iec104::isIncomeDataValid(QByteArray ba)
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
        if (V_Rrcv != V_R)
        {
            emit error(M104_NUMER);
            return I104_RCVWRONG;
        }
        quint16 V_Srcv = static_cast<quint16>(ba.at(5))*256+static_cast<quint16>(ba.at(4)&0xFE);
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
        {
            cmd = I104_STARTDT_CON;
            Started = true;
            emit startack();
        }
        if ((ba.at(2) == I104_STOPDT_CON) && (cmd == I104_STOPDT_ACT)) // если пришло подтверждение стопа и перед этим мы стоп запрашивали
        {
            cmd = I104_STOPDT_CON;
            Started = false;
        }
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

void iec104::ParseIFormat(QByteArray ba) // основной разборщик
{
    DUI.typeIdent = ba.at(0);
    DUI.qualifier.Number = ba.at(1)&0x7f;
    DUI.qualifier.SQ = ba.at(1)>>7;
    DUI.cause.cause = ba.at(2)&0x3F;
    DUI.cause.confirm = (ba.at(2)>>6)&0x01;
    DUI.cause.test = ba.at(2)>>7;
//    DUI.cause.initiator = ba.at(3);
//    DUI.commonAdrASDU = ba.at(4) + ba.at(5)*256;
    DUI.commonAdrASDU = ba.at(3);
    quint32 ObjectAdr;
    quint32 index = 4;
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
            Signals.SigNum.append(ObjectAdr);
            float value;
            memcpy(&value,&ba.data()[index],4);
            index += 4;
            Signals.SigVal.append(QString::number(value));
            quint8 quality;
            memcpy(&quality,&ba.data()[index],1);
            index++;
            Signals.SigQuality.append(quality);
            quint64 time;
            memcpy(&time,&ba.data()[index],7);
            value += 7;
            Signals.CP56Time.append(time);
            break;
        }
        default:
            break;
        }
    }
}

void iec104::SignalsGot()
{
    Signals.SigNum.clear();
    Signals.SigVal.clear();
}
