#include "canal.h"
#include "serialthread.h"

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QCoreApplication>

canal *cn;

canal::canal(QObject *parent) : QObject(parent)
{
    cmd = CN_Unk;
    ReconModeEnabled = false;
    ThreadStarted = false;
    ConnectedToPort = PortErrorDetected = false;
    ReadData = new QByteArray;
    SegEnd = 0;
    SegLeft = 0;
    ReconTry = 0;
    FirstRun = true;
    tmr = new QTimer;
    tmr->setInterval(CS_MSGTRIG);
    tmr2 = new QTimer;
    tmr2->setInterval(CS_TIMEOUT);
    connect(tmr,SIGNAL(timeout()),this,SLOT(ToggleReconLabel()));
    connect(tmr2,SIGNAL(timeout()),this,SLOT(Reconnect()));
    TTimer = new QTimer;
    TTimer->setInterval(CN_TIMEOUT);
    connect(TTimer, SIGNAL(timeout()),this,SLOT(Timeout()));
}

canal::~canal()
{
    thread->quit();
    thread->wait();
}

void canal::Send(int command, void *ptr, quint32 ptrsize, int filenum, publicclass::DataRec *DRptr)
{
    Busy = true;
    QByteArray tmpba;
    outdata = static_cast<char *>(ptr);
    outdatasize = ptrsize; // размер области данных, в которую производить запись
    cmd = command;
    fnum = static_cast<quint32>(filenum);
    DR = DRptr;
    switch (cmd)
    {
    case CN_GBsi: // запрос блока стартовой информации
    case CN_Gda:
    case CN_Gac:
    case CN_GBd: // запрос блока текущих данных
    case CN_Cnc: // переход на новую конфигурацию
    case CN_Gip: // запрос ip-адреса модуля
    case CN_GNosc: // запрос блока информации об осциллограммах
    {
        tmpba.append(CN_Start);
        tmpba.append(cmd);
        tmpba.append(~cmd);
        break;
    }
    case CN_GF: // запрос файла
    {
        tmpba.append(CN_Start);
        tmpba.append(cmd);
        tmpba.append(~cmd);
        tmpba.append(fnum);
        break;
    }
    case CN_GBosc: // запрос осциллограммы
    {
        tmpba.append(CN_Start);
        tmpba.append(cmd);
        tmpba.append(~cmd);
        tmpba.append(fnum/256);
        tmpba.append(fnum-(tmpba.at(3)*256));
        break;
    }
    case CN_WF: // запись файла
    {
        tmpba.append(CN_Start);
        tmpba.append(cmd);
        tmpba.append(~cmd);
        tmpba.append(fnum);
        tmpba.resize(30000);
        if (DR == NULL)
            Finish(CN_NULLDATAERROR);
        pc.StoreDataMem(&(tmpba.data()[7]), DR);
        DLength = static_cast<quint8>(tmpba.at(9))*65536;
        DLength += static_cast<quint8>(tmpba.at(8))*256;
        DLength += static_cast<quint8>(tmpba.at(7));
        DLength += 12; // sizeof(DataHeader)
        quint32 tmpi1 = tmpba.data()[4] = (DLength/65536);
        quint32 tmpi2 = tmpba.data()[5] = (DLength - tmpi1*65536)/256;
        tmpba.data()[6] = DLength - tmpi1*65536 - tmpi2*256;
        tmpba.resize(DLength+7);
        SetWR(tmpba,7); // 7 - длина заголовка
        if (SegLeft)
        {
            tmpba.truncate(512+7);
            WriteData->remove(0,512+7);
        }
        break;
    }
    case CN_Wac:
    case CN_Wsn:
    {
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
    Q_UNUSED(data);
    emit somedatawritten(data);
    if (cmd == CN_Unk) // игнорируем вызовы процедуры без команды
        return;
    TTimer->start();
}

void canal::GetSomeData(QByteArray ba)
{
    emit gotsomedata(ba);
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
}

void canal::Finish(int ernum)
{
    TTimer->stop();
    cmd = CN_Unk; // предотвращение вызова newdataarrived по приходу чего-то в канале, если ничего не было послано
    if (ernum != CN_OK)
        emit error(ernum);
    result = ernum;
    Busy = false;
}

void canal::Connect()
{
    SerialThread *SThread = new SerialThread;
    thread = new QThread;
    SThread->portinfo = info;
    SThread->baud = baud;
    SThread->moveToThread(thread);
    connect(thread, SIGNAL(started()), SThread, SLOT(run()));
    connect(SThread,SIGNAL(finished()),thread,SLOT(quit()));
    connect(this,SIGNAL(stopall()),SThread,SLOT(stop()));
    connect(thread,SIGNAL(finished()), thread,SLOT(deleteLater()));
    connect(SThread,SIGNAL(finished()), SThread,SLOT(deleteLater()));
    connect(SThread,SIGNAL(error(int)),this,SLOT(CanalError(int)));
    connect(SThread,SIGNAL(canalisready()),this,SLOT(CanalReady()));
    connect(SThread,SIGNAL(canalisready()),this,SLOT(TryOnceMore()));
    connect(SThread,SIGNAL(datawritten(QByteArray)),this,SLOT(DataWritten(QByteArray)));
    connect(SThread,SIGNAL(newdataarrived(QByteArray)),this,SLOT(GetSomeData(QByteArray)));
    connect(this,SIGNAL(writedatatoport(QByteArray)),SThread,SLOT(InitiateWriteDataToPort(QByteArray)));
    ConnectedToPort = PortErrorDetected = false;
    thread->start();
    ThreadStarted = true;
    while ((!ConnectedToPort) && (!PortErrorDetected))
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    if (ConnectedToPort)
    {
        emit portopened();
        result = CN_OK;
    }
    else
    {
        emit stopall();
        result=CN_PORTOPENERROR;
    }
}

void canal::Disconnect()
{
    if (ThreadStarted)
    {
        emit stopall();
        ThreadStarted = false;
    }
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
    if (FirstRun)
        Finish(ernum);
    else
    {
        ReconTry++;
        if (!ReconModeEnabled)
            StartReconnect();
    }
}

void canal::StartReconnect()
{
    ReconModeEnabled = true;
    QWidget *wdgt = new QWidget;
    lbl = new QLabel("Потеряна связь с модулем!\nПопытка восстановления № 1");
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(lbl);
    wdgt->setLayout(lyout);
    wdgt->setVisible(true);
    tmr->start();
    tmr2->start();
}

void canal::Reconnect()
{
    if (ReconTry == 4)
    {
        tmr->stop();
        tmr2->stop();
        emit error(ernum);
        Finish(CN_TIMEOUTERROR);
        ReconTry = 0;
        ReconModeEnabled = false;
    }
    else
    {
        QString tmps = lbl->text();
        tmps.chop(1);
        lbl->setText(tmps+QString::number(ReconTry));
        if (ThreadStarted)
        {
            emit stopall();
            ThreadStarted = false;
        }
        Connect();
    }
}

void canal::ToggleReconLabel()
{
    lbl->setVisible(!(lbl->isVisible()));
}

void canal::TryOnceMore()
{
    if (ReconModeEnabled) // срабатываем только в режиме реконнекта
        Send(cmd, outdata, outdatasize, fnum, DR);
}
