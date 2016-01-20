#include "canal.h"

//#include <QtDebug>
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QCoreApplication>

canal *cn;

canal::canal(QObject *parent) : QObject(parent)
{
    cmd = CN_Unk;
    ConnectedToPort = PortErrorDetected = false;
    ReadData = new QByteArray;
    SegEnd = 0;
    SegLeft = 0;
    FirstRun = true;
    OscNum = 0;
    TTimer = new QTimer(this);
    TTimer->setInterval(CN_TIMEOUT);
    NeedToSend = false;
    OscTimer = new QTimer(this);
    OscTimer->setInterval(CN_OSCT);
    OscTimer->setSingleShot(false);
    connect(OscTimer,SIGNAL(timeout()),this,SLOT(OscTimerTimeout()));
    connect(TTimer, SIGNAL(timeout()),this,SLOT(Timeout())); // для отладки закомментарить
    SThreadStarted = false;
}

canal::~canal()
{
}

void canal::Send(int command, void *ptr, quint32 ptrsize, int filenum, publicclass::DataRec *DRptr)
{
    outdata = static_cast<char *>(ptr);
    outdatasize = ptrsize; // размер области данных, в которую производить запись
    cmd = command;
    fnum = static_cast<quint32>(filenum);
    DR = DRptr;
    Busy = true;
    InitiateSend();
}

void canal::InitiateSend()
{
    QByteArray tmpba;
    switch (cmd)
    {
    case CN_GBsi: // запрос блока стартовой информации
    case CN_Gda:
    case CN_Gac:
    case CN_GBdi:
    case CN_Cnc: // переход на новую конфигурацию
    case CN_Gip: // запрос ip-адреса модуля
    case CN_GNosc: // запрос блока информации об осциллограммах
    case CN_OscEr: // команда стирания осциллограмм
    case CN_OscPg: // запрос количества нестёртых страниц
    {
        tmpba.append(CN_Start);
        tmpba.append(cmd);
        tmpba.append(~cmd);
        break;
    }
    case CN_GF: // запрос файла
    case CN_GBd: // запрос блока (подблока) текущих данных
    {
        tmpba.append(CN_Start);
        tmpba.append(cmd);
        tmpba.append(~cmd);
        tmpba.append(fnum);
//        qDebug() << "cnGf_1";
        break;
    }
    case CN_GBosc: // запрос осциллограммы
    {
        tmpba.append(CN_Start);
        tmpba.append(cmd);
        tmpba.append(~cmd);
        tmpba.append(fnum/256);
        tmpba.append(fnum-(static_cast<quint8>(tmpba.at(3))*256));
        break;
    }
    case CN_WF: // запись файла
    {
//        qDebug() << "cn1";
        tmpba.append(CN_Start);
        tmpba.append(cmd);
        tmpba.append(~cmd);
        tmpba.append(fnum);
        tmpba.resize(30000);
        if (DR == NULL)
            Finish(CN_NULLDATAERROR);
        pc.StoreDataMem(&(tmpba.data()[7]), DR);
//        qDebug() << "cn1_5";
        DLength = static_cast<quint8>(tmpba.at(9))*65536;
        DLength += static_cast<quint8>(tmpba.at(8))*256;
        DLength += static_cast<quint8>(tmpba.at(7));
        DLength += 12; // sizeof(FileHeader)
        quint32 tmpi1 = tmpba.data()[4] = (DLength/65536);
        quint32 tmpi2 = tmpba.data()[5] = (DLength - tmpi1*65536)/256;
        tmpba.data()[6] = DLength - tmpi1*65536 - tmpi2*256;
        tmpba.resize(DLength+7);
//        qDebug() << "cn1_7";
        SetWR(tmpba,7); // 7 - длина заголовка
        if (SegLeft)
        {
            tmpba.truncate(512+7);
            WriteData->remove(0,512+7);
        }
//        qDebug() << "cn1_9";

        break;
    }
    case CN_Wac:
    case CN_Wsn:
    case CN_WHv:
    {
//        qDebug() << "cnWsn_1";
        tmpba = QByteArray (QByteArray::fromRawData((const char *)outdata, outdatasize)); // 10000 - предположительная длина блока
        DLength = outdatasize;
        quint32 tmpi1 = (DLength/65536);
        quint32 tmpi2 = (DLength - tmpi1*65536)/256;
        tmpba.insert(0, DLength - tmpi1*65536 - tmpi2*256);
        tmpba.insert(0, tmpi2);
        tmpba.insert(0, tmpi1);
        tmpba.insert(0, ~cmd);
        tmpba.insert(0, cmd);
        tmpba.insert(0, CN_Start);
        SetWR(tmpba,6);
        if (SegLeft)
        {
            tmpba.truncate(512+7);
            WriteData->remove(0,512+7);
        }
//        qDebug() << "cnWsn_2";
        break;
    }
    default:
    {
        Finish(CN_UNKNOWNCMDERROR);
        return;
        break;
    }
    }
    ReadData->clear();
    bStep = 0;
    RDLength = 0;
    emit writedatatoport(tmpba);
}

void canal::DataWritten(QByteArray data)
{
    WriteDataMtx.lock();
    QByteArray *tmpba = new QByteArray(data);
    emit gotsomedata(tmpba);
    WriteDataMtx.unlock();
    if (cmd == CN_Unk) // игнорируем вызовы процедуры без команды
        return;
    TTimer->start();
}

void canal::GetSomeData(QByteArray ba)
{
    ReadDataMtx.lock();
    QByteArray *tmpba = new QByteArray(ba);
    emit gotsomedata(tmpba);
    ReadDataMtx.unlock();
    if (cmd == CN_Unk) // игнорирование вызова процедуры, если не было послано никакой команды
        return;
    int res;
    ReadData->append(ba);
    RDSize = static_cast<quint32>(ReadData->size());
    if (RDSize<3) // ждём, пока принятый буфер не будет хотя бы длиной 3 байта или не произойдёт таймаут
        return;
    if ((static_cast<quint8>(ReadData->at(1)) == 0xF0) && (static_cast<quint8>(ReadData->at(2)) == 0x0F))
    {
        Finish(71 + ReadData->at(3));
        return;
    }
    emit bytesreceived(RDSize); // сигнал для прогрессбара
    switch (bStep)
    {
    case 0: // первая порция
    {
        switch (cmd)
        {
        case CN_Cnc:
        case CN_OscEr:
        case CN_OscPg: // 3 байта: <, OSCPGst, OSCPGml
        {
            if (RDSize < 3)
                return;
            bStep++;
            break;
        }
        case CN_GBsi:
        case CN_Gda:
        case CN_Gac:
        case CN_GBd:
        case CN_GBdi:
        case CN_Gip:
        case CN_GNosc:
        case CN_GBosc:
        {
            if (RDSize < 4)
                return;
            if (ReadData->at(0) != CN_MStart)
            {
                Finish(CN_RCVDATAERROR);
                return;
            }
            SetRDLength(1);
            bStep++;
            break;
        }
        case CN_GF:
        {
            if (RDSize<5) // ещё не пришла длина, ждём-с
                return;
//            qDebug() << "cnGf_2";
            if ((ReadData->at(0) != CN_MStart) || (ReadData->at(1) != fnum)) // если первая не < и вторая - не необходимый нам номер файла
            {
                Finish(CN_RCVDATAERROR);
                return;
            }
            SetRDLength(2);
            bStep++;
//            qDebug() << "cnGf_3";
            break;
        }
        case CN_WF:
        case CN_Wac:
        case CN_Wsn:
        case CN_WHv:
        {
            if (!SegLeft)
            {
                bStep++;
                break;
            }
            if ((ReadData->at(0) == CN_MStart) && (ReadData->at(1) == CN_SegOk) && (ReadData->at(2) == ~CN_SegOk))
            {
                ReadData->clear();
                WRCheckForNextSegment();
                return;
            }
            else
            {
                Finish(CN_RCVDATAERROR);
                return;
            }
            break;
        }
        default:
        {
            Finish(CN_UNKNOWNCMDERROR);
            return;
        }
        }
    }
    case 1:
    {
        switch (cmd)
        {
        case CN_GBsi:
        case CN_Gda:
        case CN_Gac:
        case CN_GBd:
        case CN_GBdi:
        case CN_Gip:
        {
            if (!RDCheckForNextSegment())
                return;
            if (RDSize < RDLength)
                return; // пока не набрали целый буфер соответственно присланной длине или не произошёл таймаут
            if (outdatasize < DLength)
            {
                SendErr();
                Finish(CN_NULLDATAERROR);
                break;
            }
            ReadData->remove(0, 4); // убираем заголовок с < и длиной
            RDSize -= 4;
            memcpy(outdata,ReadData->data(),RDSize);
            if (LongBlock)
                SendOk();
            Finish(CN_OK);
            break;
        }
        case CN_GF:
        {
//            qDebug() << "cnGf_4";
            if (!RDCheckForNextSegment())
                return;
            if (RDSize < RDLength)
                return; // пока не набрали целый буфер соответственно присланной длине или не произошёл таймаут
            if (DR == NULL)
            {
                SendErr();
                Finish(CN_NULLDATAERROR);
                break;
            }
            ReadData->remove(0, 5); // убираем заголовок с <, номером файла и длиной
            RDSize -= 5;
            res = pc.RestoreDataMem(ReadData->data(), RDSize, DR);
//            qDebug() << "cnGf_5";
            if (!res)
            {
                if (LongBlock)
                    SendOk();
                Finish(CN_OK);
            }
            else
            {
                SendErr();
                Finish(res);
            }
            break;
        }
        case CN_GNosc:
        case CN_GBosc:
        {
            if (!RDCheckForNextSegment())
                return;
            if (RDSize < RDLength)
                return; // пока не набрали целый буфер соответственно присланной длине или не произошёл таймаут
            if (RDSize > (RDLength+4))
            {
                SendErr();
                Finish(CN_RCVLENGTHERROR);
            }
            ReadData->remove(0, 4); // убираем заголовок с < и длиной
            RDSize -= 4;
//            qDebug() << RDSize;
            memcpy(outdata,ReadData->data(),RDSize);
            if (LongBlock)
                SendOk();
            Finish(CN_OK);
            break;
        }
        case CN_WF:
        case CN_Wac:
        case CN_Cnc:
        case CN_Wsn:
        case CN_WHv:
        {
            if ((ReadData->at(0) == CN_MStart) && (ReadData->at(1) == CN_ResOk) && (ReadData->at(2) == ~CN_ResOk))
            {
                Finish(CN_OK);
                break;
            }
            Finish(CN_RCVDATAERROR);
            break;
        }
        case CN_OscEr:
        {
            if ((ReadData->at(0) == CN_MStart) && (ReadData->at(1) == CN_ResOk) && (ReadData->at(2) == ~CN_ResOk))
            {
                OscTimer->start();
                break;
            }
            Finish(CN_RCVDATAERROR);
            break;
        }
        case CN_OscPg:
        {
            quint16 OscNumRemaining = static_cast<quint8>(ReadData->at(1))*256+static_cast<quint8>(ReadData->at(2));
            if (OscNumRemaining == 0)
            {
                OscNum = 0;
                emit OscEraseCompleted();
                Finish(CN_OK);
                OscTimer->stop();
                break;
            }
            if (OscNum == 0)
            {
                emit OscEraseSize(OscNumRemaining);
                OscNum = OscNumRemaining; // максимальный диапазон для прогрессбара
            }
            else
                emit OscEraseRemaining(OscNumRemaining);
            break;
        }
        default:
        {
            Finish(CN_UNKNOWNCMDERROR);
            break;
        }
        }
    }
    }
}

void canal::SetRDLength(int startpos)
{
    DLength = static_cast<quint8>(ReadData->at(startpos))*65536;
    DLength += static_cast<quint8>(ReadData->at(startpos+1))*256;
    quint32 tmpi = static_cast<quint8>(ReadData->at(startpos+2));
    DLength += tmpi; // посчитали длину посылки
    if (DLength > 512)
    {
        LongBlock = true;
        SegLeft = (DLength-1) / 512; // -1, чтобы не делать последний сегмент нулевой длины
        SegEnd = 512 + startpos + 3; // 512 - длина сегмента, startpos - позиция начала длины, 3 - длина
    }
    else
        LongBlock = false;
    RDLength = DLength + startpos + 3;
    emit incomingdatalength(RDLength); // сигнал для прогрессбара
}

// false - есть ещё сегменты для приёма
// true - последний сегмент

bool canal::RDCheckForNextSegment()
{
    quint32 RDSize = static_cast<quint32>(ReadData->size());
    if ((RDSize >= SegEnd) && (SegLeft)) // если достигли конца текущего сегмента, и ещё есть, что принимать
    {
        SegLeft--;
        SegEnd += 512; // устанавливаем границу следующего сегмента
        QByteArray tmpba;
        tmpba.append(CN_Start);
        tmpba.append(CN_SegOk);
        tmpba.append(~CN_SegOk);
        emit writedatatoport(tmpba); // отправляем "ОК" и переходим к следующему сегменту
        return false;
    }
    return true;
}

void canal::SetWR(QByteArray ba, quint32 startpos)
{
    WriteData = new QByteArray(ba);
    WRLength = WriteData->size();
    if (static_cast<quint32>(WriteData->size())>(512+startpos)) // startpos - заголовок
    {
        SegLeft = (WRLength - startpos-1) / 512; // -1, чтобы не делать последний сегмент нулевой длины
        SegEnd = 512 + startpos;
    }
    else
    {
        SegLeft = 0;
        SegEnd = WRLength;
    }
    emit incomingdatalength(WRLength); // сигнал для прогрессбара
}

void canal::WRCheckForNextSegment()
{
    if (SegLeft)
    {
        SegLeft--;
        QByteArray tmpba;
        if (SegLeft)
        {
            SegEnd += 512;
            tmpba = QByteArray(WriteData->left(512));
            WriteData->truncate(512);
        }
        else
        {
            tmpba = QByteArray(*WriteData);
            WriteData->clear();
        }
        emit bytesreceived(SegEnd);
        emit writedatatoport(tmpba);
    }
}

void canal::SendOk()
{
    QByteArray tmpba;
    tmpba.append(CN_Start);
    tmpba.append(CN_ResOk);
    tmpba.append(~CN_ResOk);
    emit writedatatoport(tmpba); // отправляем "ОК" и переходим к следующему сегменту
}

void canal::SendErr()
{
    QByteArray tmpba;
    tmpba.append(CN_Start);
    tmpba.append(~CN_ResErr);
    tmpba.append(CN_ResErr);
    emit writedatatoport(tmpba); // отправляем "ОК" и переходим к следующему сегменту
}

void canal::Timeout()
{
    CanalError(CN_TIMEOUTERROR);
    emit SendEnd();
}

void canal::Finish(int ernum)
{
    TTimer->stop();
    cmd = CN_Unk; // предотвращение вызова newdataarrived по приходу чего-то в канале, если ничего не было послано
    if (ernum != CN_OK)
    {
        if (ernum < pc.errmsgs.size())
            CANALER(pc.errmsgs.at(ernum));
        else
            CANALER("Произошла неведомая фигня #"+QString::number(ernum,10));
    }
    result = ernum;
    Busy = false;
    emit SendEnd();
//    qDebug() << "Finish";
}

void canal::Connect()
{
    QThread *thr = new QThread;
    SThread = new SerialThread;
    SThread->portinfo = info;
    SThread->baud = baud;
    SThread->moveToThread(thr);
    connect(thr, &QThread::started, this, &canal::SetStarted);
    connect(thr, &QThread::finished, this, &canal::ClearStarted);
    connect(thr, &QThread::finished, SThread, &SerialThread::deleteLater);
    connect(thr, &QThread::finished, thr, &QThread::deleteLater);
    connect(thr, &QThread::started, SThread, &SerialThread::Run);
    connect(SThread,SIGNAL(canalisready()),this,SLOT(CanalReady()));
    connect(SThread,SIGNAL(datawritten(QByteArray)),this,SLOT(DataWritten(QByteArray)));
    connect(SThread,SIGNAL(newdataarrived(QByteArray)),this,SLOT(GetSomeData(QByteArray)));
    connect(this,SIGNAL(writedatatoport(QByteArray)),SThread,SLOT(InitiateWriteDataToPort(QByteArray)));
    ConnectedToPort = PortErrorDetected = false;
    thr->start();
    while ((!ConnectedToPort) && (!PortErrorDetected))
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (ConnectedToPort)
    {
        emit portopened();
        result = CN_OK;
    }
    else
    {
        StopSThread();
        result=CN_PORTOPENERROR;
    }
}

void canal::Disconnect()
{
    if (SThreadStarted)
        StopSThread();
}

void canal::CanalReady()
{
    ConnectedToPort = true;
    FirstRun = false;
    Finish(CN_OK);
}

void canal::CanalError(int ernum)
{
    this->ernum = ernum;
    PortErrorDetected = true;
    ConnectedToPort = false;
    Finish(ernum);
}

void canal::StopSThread()
{
    SThread->Stop();
    if (SThread)
    {
        QThread *thr = SThread->thread();
        if (thr->isRunning())
        {
            thr->quit();
            thr->wait(1000);
        }
    }
}

void canal::OscTimerTimeout()
{
    Send(CN_OscPg);
}

void canal::SetStarted()
{
    SThreadStarted = true;
}

void canal::ClearStarted()
{
    SThreadStarted = false;
}
