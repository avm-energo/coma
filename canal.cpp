#include "canal.h"
#include "config.h"

canal::canal(QObject *parent) : QObject(parent)
{
    ReadData = new QByteArray;
    SegEnd = 0;
    SegLeft = 0;
}

canal::~canal()
{

}

void canal::Send(int command, void *ptr, quint32 ptrsize, int filenum, publicclass::DataRec *DRptr)
{
    QByteArray *tmpba;
    outdata = static_cast<unsigned char *>(ptr);
    outdatasize = ptrsize; // размер области данных, в которую производить запись
    cmd = command;
    fnum = filenum;
    DR = DRptr;
    switch (cmd)
    {
    case GBsi:
    case Gda:
    case Gac:
    case GBd:
    {
        tmpba = new QByteArray(3,0x00);
        tmpba->data()[0] = Start;
        tmpba->data()[1] = cmd;
        tmpba->data()[2] = ~cmd;
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
        tmpba = new QByteArray(7+10000,0x00); // 10000 - предположительная длина файла конфигурации
        tmpba->data()[0] = Start;
        tmpba->data()[1] = WF;
        tmpba->data()[2] = ~WF;
        tmpba->data()[3] = fnum;
        if (DR == NULL)
            ErrorDetected(CN_NULLDATAERROR);
        pc.StoreDataMem(&(tmpba->data()[7]), DR);
        DLength = tmpba->data()[9]*65536+tmpba->data()[8]*256+tmpba->data()[7]; // DataHeader.size
        DLength += 12; // sizeof(DataHeader)
        int tmpi1 = tmpba->data()[4] = (DLength/65536);
        int tmpi2 = tmpba->data()[5] = (DLength - tmpi1*65536)/256;
        tmpba->data()[6] = DLength - tmpi1*65536 - tmpi2*256;
        tmpba->resize(DLength+7);
        SetWR(tmpba,7); // 7 - длина заголовка
        if (SegLeft)
        {
            tmpba->truncate(512+7);
            WriteData->remove(0,512+7);
        }
    }
    case Wac:
    {
        tmpba = new QByteArray (QByteArray::fromRawData((const char *)outdata, outdatasize)); // 10000 - предположительная длина блока
//        *tmpba = outdata;
        DLength = outdatasize;
        int tmpi1 = (DLength/65536);
        int tmpi2 = (DLength - tmpi1*65536)/256;
        tmpba->insert(0, DLength - tmpi1*65536 - tmpi2*256);
        tmpba->insert(0, tmpi2);
        tmpba->insert(0, tmpi1);
        tmpba->insert(0, ~cmd);
        tmpba->insert(0, cmd);
        tmpba->insert(0, Start);
        SetWR(tmpba,6);
        if (SegLeft)
        {
            tmpba->truncate(512+7);
            WriteData->remove(0,512+7);
        }
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
    int res;
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
        case Gda:
        case Gac:
        case GBd:
        {
            if (ReadData->size()<4) // для приёма ответа требуется минимум четыре байта, чтобы перейти к следующему шагу
                return;
            if (ReadData->at(0) != MStart)
            {
                ErrorDetected(CN_RCVDATAERROR);
                return;
            }
            SetRDLength(1);
            bStep++;
            break;
        }
        case GF:
        {
            if (ReadData->size()<5) // для приёма файла требуется минимум пять байт, чтобы перейти к следующему шагу
                return;
            if ((ReadData->at(0) != MStart) || (ReadData->at(1) != fnum)) // если первая не < и вторая - не необходимый нам номер файла
            {
                ErrorDetected(CN_RCVDATAERROR);
                return;
            }
            SetRDLength(2);
            bStep++;
            break;
        }
        case WF:
        case Wac:
        {
            if (!SegLeft)
            {
                bStep++;
                break;
            }
            if (ReadData->size()<3) // для приема ответа нужно минимум три байта
                return;
            if ((ReadData->at(0) == MStart) && (ReadData->at(1) == SegOk) && (ReadData->at(2) == ~SegOk))
            {
                ReadData->clear();
                WRCheckForNextSegment();
                return;
            }
            else
            {
                ErrorDetected(CN_RCVDATAERROR);
                return;
            }
            break;
        }
        default:
        {
            ErrorDetected(CN_UNKNOWNCMDERROR);
            return;
        }
        }
    }
    case 1:
    {
        switch (cmd)
        {
        case GBsi:
        case Gda:
        case Gac:
        case GBd:
        {
            if (!RDCheckForNextSegment())
                return;
            if (ReadData->size() < RDLength)
                return; // пока не набрали целый буфер соответственно присланной длине или не произошёл таймаут
            if (outdatasize < DLength)
            {
                ErrorDetected(CN_NULLDATAERROR);
                return;
            }
            for (int i = 4; i < ReadData->size(); i++)
            {
                *outdata = ReadData->at(i);
                outdata++;
            }
            result = 0;
            emit DataReady();
            break;
        }
        case GF:
        {
            if (!RDCheckForNextSegment())
                return;
            if (ReadData->size() < RDLength)
                return; // пока не набрали целый буфер соответственно присланной длине или не произошёл таймаут
            if (DR == NULL)
            {
                ErrorDetected(CN_NULLDATAERROR);
                return;
            }
            ReadData->remove(0, 5); // убираем заголовок с <, номером файла и длиной
            res = pc.RestoreDataMem(ReadData->data(), ReadData->size(), DR);
            if (!res)
            {
                result = 0;
                emit DataReady();
            }
            else
            {
                ErrorDetected(res);
                return;
            }
            break;
        }
        case WF:
        case Wac:
        {
            if (ReadData->size()<3) // для приема ответа нужно минимум три байта
                return;
            if ((ReadData->at(0) == MStart) && (ReadData->at(1) == ResOk) && (ReadData->at(2) == ~ResOk))
            {
                result = 0;
                emit DataReady();
                break;
            }
            ErrorDetected(CN_RCVDATAERROR);
            break;
        }
        default:
        {
            ErrorDetected(CN_UNKNOWNCMDERROR);
            break;
        }
        }
    }
    }
    disconnect(pc.SThread,SIGNAL(newdataarrived(QByteArray)),this,SLOT(GetSomeData(QByteArray)));
}

void canal::SetRDLength(int startpos)
{
    DLength = ReadData->at(startpos)*65536+ReadData->at(startpos+1)*256+ReadData->at(startpos+2); // посчитали длину посылки
    if (DLength > 512)
    {
        SegLeft = DLength / 512;
        SegEnd = 512 + startpos + 3; // 512 - длина сегмента, startpos - позиция начала длины, 3 - длина
    }
    RDLength = DLength + startpos + 3;
}

// false - есть ещё сегменты для приёма
// true - последний сегмент

bool canal::RDCheckForNextSegment()
{
    if ((ReadData->size() >= SegEnd) && (SegLeft)) // если достигли конца текущего сегмента, и ещё есть, что принимать
    {
        SegLeft--;
        SegEnd += 512; // устанавливаем границу следующего сегмента
        QByteArray *tmpba = new QByteArray(3, 0x00);
        tmpba->data()[0] = Start;
        tmpba->data()[1] = SegOk;
        tmpba->data()[2] = ~SegOk;
        pc.SThread->InitiateWriteDataToPort(tmpba); // отправляем "ОК" и переходим к следующему сегменту
        return false;
    }
    return true;
}

void canal::SetWR(QByteArray *ba, int startpos)
{
    WriteData = new QByteArray(*ba);
    WRLength = WriteData->size();
    if (WriteData->size()>(512+startpos)) // startpos - заголовок
    {
        SegLeft = (WRLength - startpos) / 512;
        SegEnd = 512 + startpos;
    }
}

void canal::WRCheckForNextSegment()
{
    if (SegLeft)
    {
        SegLeft--;
        QByteArray *tmpba;
        if (SegLeft)
        {
            SegEnd += 512;
            tmpba = new QByteArray(WriteData->left(512));
            WriteData->truncate(512);
        }
        else
        {
            tmpba = new QByteArray(*WriteData);
            WriteData->clear();
        }
        pc.SThread->InitiateWriteDataToPort(tmpba);
    }
}

void canal::Timeout()
{
    ErrorDetected(CN_TIMEOUTERROR);
}

void canal::ErrorDetected(int ernum)
{
    result = ernum;
    disconnect(pc.SThread,SIGNAL(newdataarrived(QByteArray)),this,SLOT(GetSomeData(QByteArray)));
    emit DataReady();
}

void canal::NoErrorDetected()
{
    result = 0;
    QByteArray *tmpba = new QByteArray(3, 0x00);
    tmpba->data()[0] = Start;
    tmpba->data()[1] = ResOk;
    tmpba->data()[2] = ~ResOk;
    pc.SThread->InitiateWriteDataToPort(tmpba); // отправляем "ОК" и переходим к следующему сегменту
    disconnect(pc.SThread,SIGNAL(newdataarrived(QByteArray)),this,SLOT(GetSomeData(QByteArray)));
    emit DataReady();
}
