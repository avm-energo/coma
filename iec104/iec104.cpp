#include <QThread>

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
    connect(eth,SIGNAL(error(int)),this,SLOT(CanalError(int)));
    connect(eth,SIGNAL(connected()),this,SLOT(EthConnected()));
    connect(eth,SIGNAL(newdataarrived(QByteArray)),this,SLOT(ParseIncomeData(QByteArray)));
    connect(this,SIGNAL(writedatatoeth(QByteArray)),eth,SLOT(InitiateWriteDataToPort(QByteArray)));
    thr->start();
    cmd = CMD_LISTEN;
    Ack = V_S = V_R = 0;
}

iec104::~iec104()
{

}

void iec104::Start()
{
    cmd = CMD_START;
    APCI StartDT;
    StartDT.start = I104_START;
    StartDT.APDUlength = 4;
    StartDT.contrfield[0] = I104_STARTDT_ACT;
    StartDT.contrfield[1] = StartDT.contrfield[2] = StartDT.contrfield[3] = 0;
//    void *tempp = &StartDT;
    Send(StartDT);
}

void iec104::Send(APCI apci, ASDU asdu)
{
    QByteArray ba;
    ba.resize(sizeof(apci)+sizeof(asdu));
    void *tempp = &apci;
    ba.append(static_cast<char *>(tempp),sizeof(apci));
    tempp = &asdu;
    ba.append(static_cast<char *>(tempp),sizeof(asdu));
    emit writedatatoeth(ba);
}

void iec104::ParseIncomeData(QByteArray ba)
{
    ReadData.append(ba);
    switch (cmd)
    {
    case CMD_LISTEN:
    {
        int stpos;
        int from = 0;
        while ((stpos = ReadData.indexOf(0x68,from)) != -1)
        {
            if (ReadData.size() > (stpos+1)) // если после 0х68 есть ещё хоть один символ
                from = stpos++; // следующий поиск начинаем со следующего символа
            QByteArray tmpba = ReadData.right(ReadData.size()-stpos); // оставляем только нужные для обработки байты
            int tmpi = isIncomeDataValid(tmpba);
            if (tmpi == I104_RCVNORM) // если поймали правильное начало данных, переходим к их обработке
            {
                ReadData = tmpba.right(tmpba.size()-6); // отбрасываем начальные байты вместе с APCI, ибо уже не нужны
                ParseIncomeData(tmpba); // вызываем рекурсивно обработчик с командой, установленной в isIncomeDataValid
                break;
            }
            else if (tmpi == I104_RCVSMSIZE) // данных недостаточно, надо подождать и набрать ещё
                return;
            // в противном случае переход к следующему вхождению 0х68
        }
        break;
    }
    case CMD_START:
    {
        break;
    }
    default:
        break;
    }
}

int iec104::isIncomeDataValid(QByteArray ba)
{
    if (ba.size()<6) // нет APCI, рано принимать решение по правильности блока
        return I104_RCVSMSIZE;
    APDULength = ba.at(1); // в 1-м байте лежит длина
    if ((APDULength<6) || (APDULength>253))
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
        V_R++;
        quint16 V_Srcv = static_cast<quint16>(ba.at(5))*256+static_cast<quint16>(ba.at(4)&0xFE);
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
        break;
    }
    case I104_U:
    {
        break;
    }
    default:
        break;
    }
}
