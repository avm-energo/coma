#include "canal.h"
#include "config.h"
#include "serialthread.h"

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

canal *cn;

canal::canal(QObject *parent) : QObject(parent)
{
    ReadData = new QByteArray;
    SegEnd = 0;
    SegLeft = 0;
    ReconTry = 0;
    t_t = false;
    nda_gsd = false;
    FirstRun = true;
    tmr = new QTimer;
    tmr->setInterval(CS_MSGTRIG);
    tmr2 = new QTimer;
    tmr2->setInterval(CS_TIMEOUT);
    connect(tmr,SIGNAL(timeout()),this,SLOT(ToggleReconLabel()));
    connect(tmr2,SIGNAL(timeout()),this,SLOT(Reconnect()));
}

canal::~canal()
{
    thread->quit();
    thread->wait();
}

void canal::Send(int command, void *ptr, quint32 ptrsize, int filenum, publicclass::DataRec *DRptr)
{
    QByteArray *tmpba;
    outdata = static_cast<unsigned char *>(ptr);
    outdatasize = ptrsize; // размер области данных, в которую производить запись
    cmd = command;
    fnum = filenum;
    DR = DRptr;
    if (!pc.SThread)
    {
        Finish(CN_PORTOPENERROR);
        return;
    }
    switch (cmd)
    {
    case CN_GBsi:
    case CN_Gda:
    case CN_Gac:
    case CN_GBd:
    case CN_Cnc: // переход на новую конфигурацию
    case CN_Gip:
    case CN_GNosc:
    {
        tmpba = new QByteArray(3,0x00);
        tmpba->data()[0] = CN_Start;
        tmpba->data()[1] = cmd;
        tmpba->data()[2] = ~cmd;
        break;
    }
    case CN_GF: // запрос файла
    {
        tmpba = new QByteArray(4,0x00);
        tmpba->data()[0] = CN_Start;
        tmpba->data()[1] = cmd;
        tmpba->data()[2] = ~cmd;
        tmpba->data()[3] = fnum;
        break;
    }
    case CN_WF: // запись файла
    {
        tmpba = new QByteArray(7+10000,0x00); // 10000 - предположительная длина файла конфигурации
        tmpba->data()[0] = CN_Start;
        tmpba->data()[1] = cmd;
        tmpba->data()[2] = ~cmd;
        tmpba->data()[3] = fnum;
        if (DR == NULL)
            Finish(CN_NULLDATAERROR);
        pc.StoreDataMem(&(tmpba->data()[7]), DR);
//        DLength = tmpba->data()[9]*65536+tmpba->data()[8]*256+tmpba->data()[7]; // DataHeader.size
        DLength = static_cast<quint8>(tmpba->data()[9])*65536;
        DLength += static_cast<quint8>(tmpba->data()[8])*256;
        DLength += static_cast<quint8>(tmpba->data()[7]);
        DLength += 12; // sizeof(DataHeader)
        quint32 tmpi1 = tmpba->data()[4] = (DLength/65536);
        quint32 tmpi2 = tmpba->data()[5] = (DLength - tmpi1*65536)/256;
        tmpba->data()[6] = DLength - tmpi1*65536 - tmpi2*256;
        tmpba->resize(DLength+7);
        SetWR(tmpba,7); // 7 - длина заголовка
        if (SegLeft)
        {
            tmpba->truncate(512+7);
            WriteData->remove(0,512+7);
        }
        break;
    }
    case CN_Wac:
    case CN_Wsn:
    {
        tmpba = new QByteArray (QByteArray::fromRawData((const char *)outdata, outdatasize)); // 10000 - предположительная длина блока
        DLength = outdatasize;
        quint32 tmpi1 = (DLength/65536);
        quint32 tmpi2 = (DLength - tmpi1*65536)/256;
        tmpba->insert(0, DLength - tmpi1*65536 - tmpi2*256);
        tmpba->insert(0, tmpi2);
        tmpba->insert(0, tmpi1);
        tmpba->insert(0, ~cmd);
        tmpba->insert(0, cmd);
        tmpba->insert(0, CN_Start);
        SetWR(tmpba,6);
        if (SegLeft)
        {
            tmpba->truncate(512+7);
            WriteData->remove(0,512+7);
        }
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
    try
    {
        connect(pc.SThread,SIGNAL(newdataarrived(QByteArray)),this,SLOT(GetSomeData(QByteArray)));
        nda_gsd = true;
        pc.SThread->InitiateWriteDataToPort(tmpba);
        connect(pc.SThread,SIGNAL(datawritten(QByteArray)),this,SLOT(DataWritten(QByteArray)));
    }
    catch (int)
    {
    }
}

void canal::DataWritten(QByteArray data)
{
    Q_UNUSED(data);
    disconnect(pc.SThread,SIGNAL(datawritten(QByteArray)),this,SLOT(DataWritten(QByteArray)));
    connect(pc.SThread,SIGNAL(timeout()),this,SLOT(Timeout()));
    t_t = true;
}

void canal::GetSomeData(QByteArray ba)
{
    int res;
    ReadData->append(ba);
    quint32 RDSize = static_cast<quint32>(ReadData->size());
    if (RDSize<3) // ждём, пока принятый буфер не будет хотя бы длиной 3 байта или не произойдёт таймаут
        return;
    if ((static_cast<quint8>(ReadData->at(1)) == 0xF0) && (static_cast<quint8>(ReadData->at(2)) == 0x0F))
    {
        Finish(CN_RCVDATAERROR);
        return;
    }
    switch (bStep)
    {
    case 0: // первая порция
    {
        switch (cmd)
        {
        case CN_Cnc:
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
        case CN_Gip:
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
            if ((ReadData->at(0) != CN_MStart) || (ReadData->at(1) != fnum)) // если первая не < и вторая - не необходимый нам номер файла
            {
                Finish(CN_RCVDATAERROR);
                return;
            }
            SetRDLength(2);
            bStep++;
            break;
        }
        case CN_WF:
        case CN_Wac:
        case CN_Wsn:
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
            for (quint32 i = 4; i < RDSize; i++)
            {
                *outdata = ReadData->at(i);
                outdata++;
            }
            if (LongBlock)
                SendOk();
            Finish(CN_OK);
            break;
        }
        case CN_GF:
        {
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
        case CN_WF:
        case CN_Wac:
        case CN_Cnc:
        case CN_Wsn:
        {
            if ((ReadData->at(0) == CN_MStart) && (ReadData->at(1) == CN_ResOk) && (ReadData->at(2) == ~CN_ResOk))
            {
                Finish(CN_OK);
                break;
            }
            Finish(CN_RCVDATAERROR);
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
        SegLeft = DLength / 512;
        SegEnd = 512 + startpos + 3; // 512 - длина сегмента, startpos - позиция начала длины, 3 - длина
    }
    else
        LongBlock = false;
    RDLength = DLength + startpos + 3;
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
        QByteArray *tmpba = new QByteArray(3, 0x00);
        tmpba->data()[0] = CN_Start;
        tmpba->data()[1] = CN_SegOk;
        tmpba->data()[2] = ~CN_SegOk;
        pc.SThread->InitiateWriteDataToPort(tmpba); // отправляем "ОК" и переходим к следующему сегменту
        return false;
    }
    return true;
}

void canal::SetWR(QByteArray *ba, quint32 startpos)
{
    WriteData = new QByteArray(*ba);
    WRLength = WriteData->size();
    if (static_cast<quint32>(WriteData->size())>(512+startpos)) // startpos - заголовок
    {
        SegLeft = (WRLength - startpos) / 512;
        SegEnd = 512 + startpos;
    }
    else
    {
        SegLeft = 0;
        SegEnd = WRLength;
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

void canal::SendOk()
{
    QByteArray *tmpba = new QByteArray(3, 0x00);
    tmpba->data()[0] = CN_Start;
    tmpba->data()[1] = CN_ResOk;
    tmpba->data()[2] = ~CN_ResOk;
    pc.SThread->InitiateWriteDataToPort(tmpba); // отправляем "ОК" и переходим к следующему сегменту
}

void canal::SendErr()
{
    QByteArray *tmpba = new QByteArray(3, 0x00);
    tmpba->data()[0] = CN_Start;
    tmpba->data()[1] = ~CN_ResErr;
    tmpba->data()[2] = CN_ResErr;
    pc.SThread->InitiateWriteDataToPort(tmpba); // отправляем "ОК" и переходим к следующему сегменту
}

void canal::Timeout()
{
    if (nda_gsd)
    {
        ReconTry++;
        StartReconnect();
    }
}

void canal::Finish(int ernum)
{
    result = ernum;
    if (nda_gsd)
    {
        disconnect(pc.SThread,SIGNAL(newdataarrived(QByteArray)),this,SLOT(GetSomeData(QByteArray)));
        nda_gsd = false;
    }
    if (t_t)
    {
        disconnect(pc.SThread,SIGNAL(timeout()),this,SLOT(Timeout()));
        t_t = false;
    }
    emit DataReady();
}

void canal::CanalReady()
{
    FirstRun = false;
    Finish(CN_OK);
    emit portopened();
}

void canal::NoErrorDetected()
{
    result = 0;
    QByteArray *tmpba = new QByteArray(3, 0x00);
    tmpba->data()[0] = CN_Start;
    tmpba->data()[1] = CN_ResOk;
    tmpba->data()[2] = ~CN_ResOk;
    pc.SThread->InitiateWriteDataToPort(tmpba); // отправляем "ОК" и переходим к следующему сегменту
    disconnect(pc.SThread,SIGNAL(newdataarrived(QByteArray)),this,SLOT(GetSomeData(QByteArray)));
    nda_gsd = false;
    emit DataReady();
}

void canal::Connect()
{
    thread = new QThread;
    pc.SThread = new SerialThread;
    pc.SThread->portinfo = info;
    pc.SThread->baud = baud;
    pc.SThread->moveToThread(thread);
    connect(thread, SIGNAL(started()), pc.SThread, SLOT(run()));
    connect(thread,SIGNAL(finished()),pc.SThread,SLOT(deleteLater()));
//    connect(this,SIGNAL(stopall()),pc.SThread,SLOT(stop()));
    connect(pc.SThread,SIGNAL(error(int)),this,SLOT(SetErNum(int)));
    connect(pc.SThread,SIGNAL(error(int)),this,SLOT(CanalError(int)));
    connect(pc.SThread,SIGNAL(canalisready()),this,SLOT(CanalReady()));
//    connect(pc.SThread,SIGNAL(finished()),this,SLOT(KillSThread()));
//    connect(pc.SThread,SIGNAL(finished()),thread,SLOT(quit()));
//    connect(thread,SIGNAL(finished()),pc.SThread,SLOT(deleteLater()));
    thread->start();
}

void canal::SetErNum(int ernum)
{
    this->ernum = ernum;
}

void canal::CanalError(int ernum)
{
    if (FirstRun)
        Finish(ernum);
    else
    {
        ReconTry++;
        StartReconnect();
    }
}

void canal::KillSThread()
{
    delete pc.SThread;
}

void canal::Disconnect()
{
    emit stopall();
}

void canal::StartReconnect()
{
    nda_gsd = t_t = false;
    QDialog *dlg = new QDialog;
    dlg->setModal(true);
    QLabel *lbl = new QLabel("Потеряна связь с модулем!\nПопытка восстановления № 0");
    lbl->setObjectName("tryreconlbl");
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(lbl);
    dlg->setLayout(lyout);
    dlg->setVisible(true);
    tmr->start();
    tmr2->start();
}

void canal::Reconnect()
{
    try
    {
        if (ReconTry == 4)
        {
            tmr->stop();
            tmr2->stop();
            emit error(ernum);
            Finish(CN_TIMEOUTERROR);
            ReconTry = 0;
        }
        else
        {
            thread->quit();
            thread->wait();
            Connect();
            connect(pc.SThread,SIGNAL(canalisready()),this,SLOT(TryOnceMore()));
        }
    }
    catch(int)
    {
    }
}

void canal::ToggleReconLabel()
{
    QLabel *lbl = this->findChild<QLabel *>("tryreconlbl");
    if (lbl == 0)
        return;
    lbl->setVisible(!(lbl->isVisible()));
}

void canal::TryOnceMore()
{
    disconnect(pc.SThread,SIGNAL(canalisready()),this,SLOT(TryOnceMore()));
    Send(cmd, outdata, outdatasize, fnum, DR);
}
