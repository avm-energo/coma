#include "Canal.h"

//#include <QtDebug>
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QCoreApplication>

Canal cn;

Canal::Canal(QObject *parent) : QObject(parent)
{
    RDLength = 0;
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
}

Canal::~Canal()
{
}

void Canal::Send(int command, int board_type, void *ptr, quint32 ptrsize, quint16 filenum, publicclass::DataRec *DRptr)
{
    outdata = static_cast<char *>(ptr);
    outdatasize = ptrsize; // размер области данных, в которую производить запись
    cmd = command;
    fnum = filenum;
    DR = DRptr;
    Busy = true;
    if (board_type == BT_BASE)
        BoardType = 0x01;
    else if (board_type == BT_MEZONIN)
        BoardType = 0x02;
    else
        BoardType = 0x00;
    InitiateSend();
}

void Canal::InitiateSend()
{
    QByteArray tmpba;
    switch (cmd)
    {
    case CN_GBsi:   // запрос блока стартовой информации
    case CN_Gip:    // запрос ip-адреса модуля
    case CN_OscEr:  // команда стирания осциллограмм
    case CN_OscPg:  // запрос количества нестёртых страниц
    case CN_GBo:    // чтение осциллограмм
    {
        tmpba.resize(5); // +1 on '\x0'
        tmpba.append(CN_Start);
        tmpba.append(cmd);
        tmpba.append(static_cast<char>(0x00));
        tmpba.append(static_cast<char>(0x00));
        break;
    }
    case CN_GBda:   // чтение текущих данных без настройки
    case CN_GBac:   // чтение настроечных коэффициентов
    case CN_GBd:    // запрос блока (подблока) текущих данных
    {
        tmpba.resize(6);
        tmpba.append(CN_Start);
        tmpba.append(cmd);
        tmpba.append(static_cast<char>(0x01));
        tmpba.append(static_cast<char>(0x00));
        tmpba.append(BoardType); // length of block is zero for GBsi and it's a board type for GBda & GBac
    }
    case CN_GF:     // запрос файла
    {
        tmpba.resize(7);
        tmpba.append(CN_Start);
        tmpba.append(cmd);
        tmpba.append(static_cast<char>(0x02)); // length of block (2 bytes)
        tmpba.append(static_cast<char>(0x00));
        tmpba.append(static_cast<char>(fnum&0x00FF));
        tmpba.append(static_cast<char>((fnum&0xFF00)>>8));
//        qDebug() << "cnGf_1";
        break;
    }
    case CN_WHv:
    {
        tmpba.resize(5);
        tmpba.append(CN_Start);
        tmpba.append(cmd);
        tmpba.append(static_cast<char>(0x00));
        tmpba.append(static_cast<char>(0x00));
        break;
    }
/*    case CN_GBosc: // запрос осциллограммы
    {
        tmpba.append(CN_Start);
        tmpba.append(cmd);
        tmpba.append(~cmd);
        tmpba.append(fnum/256);
        tmpba.append(fnum-(static_cast<quint8>(tmpba.at(3))*256));
        break;
    } */
    case CN_WF: // запись файла
    {
//        qDebug() << "cn1";
        tmpba.append(CN_Start);
        tmpba.append(cmd);
        tmpba.append(fnum);
        tmpba.resize(CN_MAXFILESIZE);
        if (DR == NULL)
            Finish(CN_NULLDATAERROR);
        pc.StoreDataMem(&(tmpba.data()[7]), DR, fnum);
//        qDebug() << "cn1_5";
        DLength = static_cast<quint8>(tmpba.at(9))*65536;
        DLength += static_cast<quint8>(tmpba.at(8))*256;
        DLength += static_cast<quint8>(tmpba.at(7));
        DLength += sizeof(publicclass::FileHeader); // sizeof(FileHeader)
        quint32 tmpi1 = tmpba.data()[4] = (DLength/65536);
        quint32 tmpi2 = tmpba.data()[5] = (DLength - tmpi1*65536)/256;
        tmpba.data()[6] = DLength - tmpi1*65536 - tmpi2*256;
        tmpba.resize(DLength+7);
//        qDebug() << "cn1_7";
        SetWR(tmpba); // 7 - длина заголовка
        if (SegLeft)
        {
            tmpba.truncate(512+7);
            WriteData.remove(0,512+7);
        }
//        qDebug() << "cn1_9";

        break;
    }
    case CN_WBac:
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
        SetWR(tmpba);
        if (SegLeft)
        {
            tmpba.truncate(512+7);
            WriteData.remove(0,512+7);
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
    ReadData.clear();
    LastBlock = false;
    bStep = 0;
    RDLength = 0;
    WriteDataToPort(tmpba);
}

void Canal::ParseIncomeData(QByteArray &ba)
{
    if (cmd == CN_Unk) // игнорирование вызова процедуры, если не было послано никакой команды
        return;
    int res;
    ReadData.append(ba);
    RDSize = static_cast<quint32>(ReadData.size());
    if (RDSize<3) // ждём, пока принятый буфер не будет хотя бы длиной 3 байта или не произойдёт таймаут
        return;
    if (static_cast<quint8>(ReadData.at(1)) == CN_ResErr)
    {
        if (RDSize < 5) // некорректная посылка
            Finish(CN_RCVDATAERROR);
        else
            Finish(USO_NOERR + ReadData.at(4));
        return;
    }
    switch (bStep)
    {
    case 0: // первая порция
    {
        switch (cmd)
        {
        case CN_OscEr:
        case CN_OscPg: // 3 байта: <, OSCPGst, OSCPGml
        {
            if (RDSize < 3)
                return;
            bStep++;
            break;
        }
        case CN_GBsi:
        case CN_GBda:
        case CN_GBac:
        case CN_GBd:
        case CN_Gip:
        case CN_GF:
        {
            if (RDSize < 4) // '<', CN, L, L
                return;
            if ((ReadData.at(0) != CN_MStart) || (ReadData.at(1) != cmd))
            {
                Finish(CN_RCVDATAERROR);
                return;
            }
            SetRDLength();
            bStep++;
            break;
        }
        case CN_WF:
        case CN_WBac:
        case CN_WHv:
        {
            if (!SegLeft)
            {
                bStep++;
                break;
            }
            if ((ReadData.at(0) == CN_MStart) && (ReadData.at(1) == CN_ResOk) && (ReadData.at(2) == 0x00) && (ReadData.at(3) == 0x00))
            {
                ReadData.clear();
                WRCheckForNextSegment();
            }
            else
                Finish(CN_RCVDATAERROR);
            break;
        }
        default:
            Finish(CN_UNKNOWNCMDERROR);
            break;
        }
    }
    case 1:
    {
        switch (cmd)
        {
        case CN_GBsi:
        case CN_GBda:
        case CN_GBac:
        case CN_GBd:
        case CN_Gip:
        {
            if (!RDCheckForNextSegment())
                return;
            if (RDSize < RDLength)
                return; // пока не набрали целый буфер соответственно присланной длине или не произошёл таймаут
            RDLength = 0; // обнуляем, чтобы не было лишних вызовов incomingdatalength
            if (outdatasize < DLength)
            {
                SendErr();
                Finish(CN_NULLDATAERROR);
                break;
            }
            ReadData.remove(0, 4); // убираем заголовок с < и длиной
            RDSize -= 4;
            memcpy(outdata,ReadData.data(),RDSize);
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
            RDLength = 0;
            if (DR == NULL)
            {
                SendErr();
                Finish(CN_NULLDATAERROR);
                break;
            }
            ReadData.remove(0, 5); // убираем заголовок с <, номером файла и длиной
            RDSize -= 5;
            res = pc.RestoreDataMem(ReadData.data(), RDSize, DR);
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
            RDLength = 0;
            if (RDSize > (RDLength+4))
            {
                SendErr();
                Finish(CN_RCVLENGTHERROR);
            }
            ReadData.remove(0, 4); // убираем заголовок с < и длиной
            RDSize -= 4;
//            qDebug() << RDSize;
            memcpy(outdata,ReadData.data(),RDSize);
            if (LongBlock)
                SendOk();
            Finish(CN_OK);
            break;
        }
        case CN_WF:
        case CN_WBac:
        case CN_Cnc:
        case CN_Wsn:
        case CN_WHv:
        {
            if ((ReadData.at(0) == CN_MStart) && (ReadData.at(1) == CN_ResOk) && (ReadData.at(2) == ~CN_ResOk))
            {
                Finish(CN_OK);
                break;
            }
            Finish(CN_RCVDATAERROR);
            break;
        }
        case CN_OscEr:
        {
            if ((ReadData.at(0) == CN_MStart) && (ReadData.at(1) == CN_ResOk) && (ReadData.at(2) == ~CN_ResOk))
            {
                OscTimer->start();
                break;
            }
            Finish(CN_RCVDATAERROR);
            break;
        }
        case CN_OscPg:
        {
            quint16 OscNumRemaining = static_cast<quint8>(ReadData.at(1))*256+static_cast<quint8>(ReadData.at(2));
            if (OscNumRemaining == 0)
            {
                emit OscEraseRemaining(OscNum);
                OscNum = 0;
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
                emit OscEraseRemaining(OscNum - OscNumRemaining);
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

void Canal::SetRDLength()
{
    DLength = static_cast<quint8>(ReadData.at(2));
    DLength += static_cast<quint8>(ReadData.at(3))*256; // посчитали длину посылки
    if (DLength < 768)
        LastBlock = true;
}

// false - есть ещё сегменты для приёма
// true - последний сегмент

bool Canal::RDCheckForNextSegment()
{
    quint32 RDSize = static_cast<quint32>(ReadData.size());
    emit bytesreceived(RDSize); // сигнал для прогрессбара
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

void Canal::SetWR(QByteArray &ba)
{
    WriteData = ba;
    WRLength = WriteData.size();
    if (WRLength > CN_MAXSEGMENTLENGTH)
    {
        SegLeft = WRLength / CN_MAXSEGMENTLENGTH;
        SegEnd = CN_MAXSEGMENTLENGTH;
    }
    else
    {
        SegLeft = 0;
        SegEnd = WRLength;
    }
    emit WriteDataLength(WRLength); // сигнал для прогрессбара
}

void Canal::WRCheckForNextSegment()
{
    if (SegLeft)
    {
        --SegLeft;
        QByteArray tmpba;
        if (SegLeft)
        {
            tmpba = WriteData.mid(SegEnd, CN_MAXSEGMENTLENGTH);
            SegEnd += CN_MAXSEGMENTLENGTH;
        }
        else
        {
            tmpba = WriteData.right(WriteData.size()-SegEnd);
            WriteData.clear();
        }
        emit WriteDataPos(SegEnd);
        WriteDataToPort(tmpba);
    }
}

void Canal::SendOk()
{
    QByteArray tmpba;
    tmpba.append(CN_Start);
    tmpba.append(CN_ResOk);
    tmpba.append(~CN_ResOk);
    WriteDataToPort(tmpba); // отправляем "ОК" и переходим к следующему сегменту
}

void Canal::SendErr()
{
    QByteArray tmpba;
    tmpba.append(CN_Start);
    tmpba.append(~CN_ResErr);
    tmpba.append(CN_ResErr);
    WriteDataToPort(tmpba); // отправляем "ОК" и переходим к следующему сегменту
}

void Canal::Timeout()
{
    Finish(CN_TIMEOUTERROR);
    emit SendEnd();
}

void Canal::Finish(int ernum)
{
    TTimer->stop();
    cmd = CN_Unk; // предотвращение вызова newdataarrived по приходу чего-то в канале, если ничего не было послано
    if (ernum != CN_OK)
    {
        if (ernum < pc.errmsgs.size())
            ERMSG(pc.errmsgs.at(ernum));
        else
            ERMSG("Произошла неведомая фигня #"+QString::number(ernum,10));
    }
    result = ernum;
    Busy = false;
    emit SendEnd();
//    qDebug() << "Finish";
}

bool Canal::Connect()
{
    if (!InitializePort(info, baud))
        return false;
}

void Canal::Disconnect()
{
    ClosePort();
}

void Canal::OscTimerTimeout()
{
    Send(CN_OscPg);
}

bool Canal::InitializePort(QSerialPortInfo &pinfo, int baud)
{
    port = new QSerialPort;
    port->setPort(pinfo);
    connect(port,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(Error(QSerialPort::SerialPortError)));
    if (!port->open(QIODevice::ReadWrite))
        return false;
    port->setBaudRate(baud);
    port->setParity(QSerialPort::NoParity);
    port->setDataBits(QSerialPort::Data8);
    port->setFlowControl(QSerialPort::NoFlowControl);
    port->setStopBits(QSerialPort::OneStop);
    connect(port,SIGNAL(readyRead()),this,SLOT(CheckForData()));
    emit Canalisready();
}

void Canal::ClosePort()
{
    try
    {
        port->close();
        QTimer *tmr = new QTimer;
        tmr->setInterval(ST_CLOSEPORT_TIMEOUT);
        connect(tmr,SIGNAL(timeout()),this,SLOT(PortCloseTimeout()));
        PortCloseTimeoutSet = false;
        tmr->start();
        while ((port->isOpen() && (!PortCloseTimeoutSet)))
        {
            QTime tme;
            tme.start();
            while (tme.elapsed() < ST_MAINLOOP_DELAY)
                qApp->processEvents();
        }
        delete port;
    }
    catch(...)
    {
        DBGMSG;
    }
}

void Canal::CheckForData()
{
    QByteArray ba = port->read(1000);
    emit ReadBytesSignal(ba);
    ParseIncomeData(ba);
}

void Canal::Error(QSerialPort::SerialPortError err)
{
    if (!err) // нет ошибок
        return;
    quint16 ernum = err + COM_ERROR;
    if (pc.ErMsgsOk)
    {
        if (ernum < pc.errmsgs.size())
            ERMSG(pc.errmsgs.at(ernum));
        else
            ERMSG("Произошла неведомая фигня #"+QString::number(ernum,10));
    }
    else
        ERMSG("Произошла неведомая фигня #"+QString::number(ernum,10));
}

void Canal::PortCloseTimeout()
{
    PortCloseTimeoutSet = true;
}

bool Canal::WriteDataToPort(QByteArray &ba)
{
    if (cmd == CN_Unk) // игнорируем вызовы процедуры без команды
        return false;
    qint64 byteswritten = 0;
    qint64 basize = ba.size();
    while ((byteswritten < basize) && (!ba.isEmpty()))
    {
        qint64 tmpi = port->write(ba);
        if (res == GENERALERROR)
            return false;
        byteswritten += tmpi;
        emit WriteBytesSignal(ba.left(tmpi));
        if (tmpi <= ba.size())
            ba = ba.remove(0, tmpi);
        else
            ba.clear();
    }
    TTimer->start();
    return true;
}
