#include "canal.h"
#include "config.h"

canal::canal(QObject *parent) : QObject(parent)
{
    ReadData = new QByteArray;
}

canal::~canal()
{

}

void canal::Send(int command, void *ptr, quint32 ptrsize, int filenum, publicclass::DataRec *DRptr)
{
    QByteArray *tmpba;
    outdata = static_cast<unsigned char *>(ptr);
    if (outdata == NULL)
        ErrorDetected();
    if (DRptr == NULL)
        ErrorDetected();
    if (!ptrsize)
        ErrorDetected();
    outdatasize = ptrsize; // размер области данных, в которую производить запись
    cmd = command;
    fnum = filenum;
    switch (cmd)
    {
    case GBsi:
    {
        tmpba = new QByteArray(3,0x00);
        tmpba->data()[0] = Start;
        tmpba->data()[1] = GBsi;
        tmpba->data()[2] = ~GBsi;
        break;
    }
    case GF: // запрос файла
    {
        tmpba = new QByteArray(4,0x00);
        tmpba->data()[0] = Start;
        tmpba->data()[1] = GF;
        tmpba->data()[2] = ~GF;
        tmpba->data()[3] = fnum;
        break;
    }
    case WF: // запись файла
    {
    }
    default:
        break;
    }
    ReadData->clear();
    bStep = 0;
    RDLength = 0;
    connect(pc.SThread,SIGNAL(newdataarrived(QByteArray)),this,SLOT(GetSomeData(QByteArray)));
    connect(pc.SThread,SIGNAL(timeout()),this,SLOT(Timeout()));
    pc.SThread->InitiateWriteDataToPort(tmpba);
}

void canal::GetSomeData(QByteArray ba)
{
    ReadData->resize(ReadData->size()+ba.size()); // расширяем приёмный буфер
    ReadData->append(ba);
    if (ReadData->size()<3) // ждём, пока принятый буфер не будет хотя бы длиной 3 байта или не произойдёт таймаут
        return;
    switch (bStep)
    {
    case 0: // первая порция
    {
        switch (cmd)
        {
        case GBsi:
        {
            if (ReadData->size()<4) // для приёма ответа на GBsi требуется минимум четыре байта, чтобы перейти к следующему шагу
                return;
            if (ReadData->at(0) != MStart)
                ErrorDetected();
            SetRDLength(1);
            bStep++;
            break;
        }
        case GF:
        {
            if (ReadData->size()<5) // для приёма файла требуется минимум пять байт, чтобы перейти к следующему шагу
                return;
            if ((ReadData->at(0) != MStart) || (ReadData->at(1) != fnum)) // если первая не < и вторая - не необходимый нам номер файла
                ErrorDetected();
            SetRDLength(2);
            bStep++;
            break;
        }
        default:
        {
            ErrorDetected();
            break;
        }
        }
    }
    case 1:
    {
        if ((ReadData->size() >= SegEnd) && (SegLeft)) // если достигли конца текущего сегмента, и ещё есть, что принимать
        {
            SegEnd += 512; // устанавливаем границу следующего сегмента
            SegLeft--;
            QByteArray *tmpba = new QByteArray(3, 0x00);
            tmpba->data()[0] = Start;
            tmpba->data()[1] = SegOk;
            tmpba->data()[2] = ~SegOk;
            pc.SThread->InitiateWriteDataToPort(tmpba); // отправляем "ОК" и переходим к следующему сегменту
            return;
        }
        switch (cmd)
        {
        case GBsi:
        {
            if (ReadData->size() < RDLength)
                return; // пока не набрали целый буфер соответственно присланной длине или не произошёл таймаут
            NoErrorDetected();
            break;
        }
        case GF:
        {
            if (ReadData->size() < RDLength)
                return; // пока не набрали целый буфер соответственно присланной длине или не произошёл таймаут
            NoErrorDetected();
            break;
        }
        default:
        {
            ErrorDetected();
            break;
        }
        }
    }
    }
}

void canal::SetRDLength(int startpos)
{
    RDLength = ReadData->at(startpos)*65536+ReadData->at(startpos+1)*256+ReadData->at(startpos+2); // посчитали длину посылки
    if (outdatasize<RDLength) // выход за границу допустимой области
        ErrorDetected();
    if (RDLength > 512)
    {
        SegLeft = RDLength / 512;
        SegEnd = 512 + startpos + 3; // 512 - длина сегмента, startpos - позиция начала длины, 3 - длина
    }
    RDLength += startpos + 3;
}

void canal::Timeout()
{
    ErrorDetected();
}

void canal::ErrorDetected()
{
    result = -1;
    disconnect(pc.SThread,SIGNAL(newdataarrived(QByteArray)),this,SLOT(GetSomeData(QByteArray)));
    emit DataReady();
}

void canal::NoErrorDetected()
{
    result = 0;
    for (int i = 0; i < ReadData->size(); i++)
    {
        *outdata = ReadData->at(i);
        outdata++;
    }
    QByteArray *tmpba = new QByteArray(3, 0x00);
    tmpba->data()[0] = Start;
    tmpba->data()[1] = ResOk;
    tmpba->data()[2] = ~ResOk;
    pc.SThread->InitiateWriteDataToPort(tmpba); // отправляем "ОК" и переходим к следующему сегменту
    disconnect(pc.SThread,SIGNAL(newdataarrived(QByteArray)),this,SLOT(GetSomeData(QByteArray)));
    emit DataReady();
}
