#include <QCoreApplication>
#include <QTime>
#include "eabstractprotocomchannel.h"

EAbstractProtocomChannel::EAbstractProtocomChannel(QObject *parent) : QObject(parent)
{
    log = new Log;
    log->Init("EAbstractProtocomChannel.log");
    log->WriteRaw("Log started!");
    RDLength = 0;
    SegEnd = 0;
    SegLeft = 0;
    FirstRun = true;
    FirstSegment = true;
    OscNum = 0;
    Connected = false;
    Cancelled = false;
    TTimer = new QTimer(this);
    TTimer->setInterval(CN_TIMEOUT);
    NeedToSend = false;
    OscTimer = new QTimer(this);
    OscTimer->setInterval(CN_OSCT);
    OscTimer->setSingleShot(false);
    connect(OscTimer,SIGNAL(timeout()),this,SLOT(OscTimerTimeout()));
    connect(TTimer, SIGNAL(timeout()),this,SLOT(Timeout())); // для отладки закомментарить
}

EAbstractProtocomChannel::~EAbstractProtocomChannel()
{
}

void EAbstractProtocomChannel::Send(int command, int board_type, void *ptr, quint32 ptrsize, quint16 filenum, QVector<publicclass::DataRec> *DRptr)
{
    if (!Connected)
    {
        result = CN_NULLDATAERROR;
        return;
    }
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
    else if (board_type == BT_NONE)
        BoardType = 0x00;
    else
        BoardType = board_type; // in GBd command it is a block number
    InitiateSend();
    while (Busy)
    {
        QTime tme;
        tme.start();
        while (tme.elapsed() < CN_MAINLOOP_DELAY)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
}

void EAbstractProtocomChannel::InitiateSend()
{
    WriteData.clear();
    switch (cmd)
    {
    case CN_GBsi:   // запрос блока стартовой информации
    case CN_GBo:    // чтение осциллограмм
    case CN_IP:    // запрос ip-адреса модуля
    case CN_OscEr:  // команда стирания осциллограмм
    case CN_OscPg:  // запрос количества нестёртых страниц
    {
//        WriteData.resize(4);
        WriteData.append(CN_MS);
        WriteData.append(cmd);
        AppendSize(WriteData, 0);
        WriteDataToPort(WriteData);
        break;
    }
    case CN_GBac:   // чтение настроечных коэффициентов
    case CN_GBda:   // чтение текущих данных без настройки
    case CN_GBd:    // запрос блока (подблока) текущих данных
    {
//        WriteData.resize(5);
        WriteData.append(CN_MS);
        WriteData.append(cmd);
        AppendSize(WriteData, 1);
        WriteData.append(BoardType);
        WriteDataToPort(WriteData);
        break;
    }
    case CN_GF:     // запрос файла
    {
        if (DR->isEmpty())
        {
//            SendErr();
            Finish(CN_NULLDATAERROR);
            break;
        }
//        WriteData.resize(6);
        WriteData.append(CN_MS);
        WriteData.append(cmd);
        AppendSize(WriteData, 2);
        WriteData.append(static_cast<char>(fnum&0x00FF));
        WriteData.append(static_cast<char>((fnum&0xFF00)>>8));
        WriteDataToPort(WriteData);
        break;
    }
    case CN_WHv:
    {
        WriteData.append(CN_MS);
        WriteData.append(cmd);
        AppendSize(WriteData, 17); // BoardType(1), HiddenBlock(16)
        WriteData.append(BoardType);
        WriteData.resize(WriteData.size()+outdatasize);
        memcpy(&(WriteData.data()[5]), &outdata[0], outdatasize);
        WriteDataToPort(WriteData);
        break;
    }
    case CN_WF: // запись файла
    {
        if (DR->isEmpty())
            Finish(CN_NULLDATAERROR);
//        WriteData.resize(CN_MAXFILESIZE);
//        WriteData.append(CN_MS);
//        WriteData.append(cmd);
//        AppendSize(WriteData, 0); // временно записываем нулевую длину, впоследствии поменяем
        WriteData.resize(CN_MAXFILESIZE);
        pc.StoreDataMem(&(WriteData.data()[0]), DR, fnum);
        // считываем длину файла из полученной в StoreDataMem и вычисляем количество сегментов
        WRLength = static_cast<quint8>(WriteData.at(7))*16777216; // с 4 байта начинается FileHeader.size
        WRLength += static_cast<quint8>(WriteData.at(6))*65536;
        WRLength += static_cast<quint8>(WriteData.at(5))*256;
        WRLength += static_cast<quint8>(WriteData.at(4));
        WRLength += sizeof(publicclass::FileHeader); // sizeof(FileHeader)
//        WRLength += 4; // + 4 bytes prefix
        WriteData.resize(WRLength);
        emit SetDataSize(WRLength); // сигнал для прогрессбара
        FirstSegment = true;
        SetWRSegNum();
        WRCheckForNextSegment();
        break;
    }
    case CN_WBac:
    case CN_CtEr:
    {
        WriteData.append(BoardType);
        WriteData.append(QByteArray::fromRawData((const char *)outdata, outdatasize));
        WRLength = outdatasize + 1;
        emit SetDataSize(WRLength); // сигнал для прогрессбара
        FirstSegment = true;
        SetWRSegNum();
        WRCheckForNextSegment();
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
    ReadDataChunk.clear();
    LastBlock = false;
    bStep = 0;
    RDLength = 0;
}

void EAbstractProtocomChannel::ParseIncomeData(QByteArray ba)
{
    if (pc.WriteUSBLog)
    {
        log->WriteRaw("<-");
        log->WriteRaw(ba.toHex());
        log->WriteRaw("\n");
    }
    if (cmd == CN_Unk) // игнорирование вызова процедуры, если не было послано никакой команды
        return;
    int res;
    ReadDataChunk.append(ba);
    quint32 RDSize = static_cast<quint32>(ReadDataChunk.size());
    if (RDSize<3) // ждём, пока принятый буфер не будет хотя бы длиной 3 байта или не произойдёт таймаут
        return;
    switch (bStep)
    {
    case 0: // первая порция
    {
        if (ReadDataChunk.at(0) != CN_SS)
        {
            Finish(CN_RCVDATAERROR);
            return;
        }
        if (ReadDataChunk.at(1) == CN_ResErr)
        {
            if (RDSize < 5) // некорректная посылка
                Finish(CN_RCVDATAERROR);
            else
                Finish(USO_NOERR + ReadDataChunk.at(4));
            return;
        }
        switch (cmd)
        {
        // команды с ответом "ОК"
        case CN_WHv:
        case CN_OscEr:
        case CN_CtEr:
        {
            if (GetLength())
            {
                if (cmd == CN_OscEr)
                    OscTimer->start(); // start timer to send OscPg command periodically
                Finish(NOERROR);
            }
            else
                Finish(CN_RCVDATAERROR);
            return;
        }
        // команды с ответом "ОК" и с продолжением
        case CN_WF:
        case CN_WBac:
        {
            if (!SegLeft)
            {
                if (GetLength())
                    Finish(NOERROR);
                else
                    Finish(CN_RCVDATAERROR);
                return;
            }
            if (GetLength())
            {
                ReadDataChunk.clear();
                WRCheckForNextSegment();
            }
            else
                Finish(CN_RCVDATAERROR);
            break;
        }
        // команды с ответом SS c L L ... и продолжением
        case CN_GBsi:
        case CN_GBda:
        case CN_GBac:
        case CN_GBd:
        case CN_IP:
        case CN_OscPg:
        case CN_GBe:
        case CN_GBo:
        case CN_GBTe:
        case CN_GF:
        {
            if (!GetLength(false)) // !ok = cmd
            {
                Finish(CN_RCVDATAERROR);
                return;
            }
            emit SetDataSize(RDLength);
            ReadDataChunk.remove(0, 4); // убираем заголовок с < и длиной
            RDSize -= 4;
            if (cmd == CN_GF) // надо проверить, тот ли номер файла принимаем
            {
                if (RDSize < 6) // не пришёл ещё номер файла
                    return;
                quint16 filenum = static_cast<quint16>(ReadDataChunk[0])+static_cast<quint16>(ReadDataChunk[1])*256; // 0 & 1 because 4 bytes already removed
                if (filenum != fnum)
                {
                    Finish(USO_UNKNFILESENT);
                    return;
                }
            }
            bStep++;
            break;
        }
        default:
            Finish(CN_UNKNOWNCMDERROR);
            break;
        }
    }
    case 1:
    {
        if (RDSize < RDLength)
            return; // пока не набрали целый буфер соответственно присланной длине или не произошёл таймаут
        ReadData += ReadDataChunk;
        RDSize = static_cast<quint32>(ReadData.size());
        emit SetDataCount(RDSize); // сигнал для прогрессбара
        ReadDataChunk.clear();
        switch (cmd)
        {
        case CN_GBsi:
        case CN_GBda:
        case CN_GBac:
        case CN_GBd:
        case CN_IP:
        case CN_GBe:
        case CN_GBTe:
        case CN_GBo:
        {
            if (LastBlock)
            {
                if (RDSize > outdatasize)
                    RDSize = outdatasize;
                memcpy(outdata,ReadData.data(),RDSize);
                Finish(NOERROR);
            }
            else
            {
                SendOk();
                bStep = 0; // переход к проверке длины блока и т.п.
            }
            break;
        }
        case CN_GF:
        {
            if (LastBlock)
            {
                res = pc.RestoreDataMem(ReadData.data(), RDSize, DR);
                if (res == 0)
                    Finish(NOERROR);
                else
                {
//                    SendErr();
                    Finish(res);
                }
            }
            else
            {
                SendOk();
                bStep = 0; // переход к проверке длины блока и т.п.
            }
            break;
        }
        case CN_OscPg:
        {
            quint16 OscNumRemaining = static_cast<quint8>(ReadData.at(0))+static_cast<quint8>(ReadData.at(1))*256;
            if (OscNumRemaining == 0)
            {
                emit SetDataCount(OscNum);
                OscNum = 0;
                Finish(NOERROR);
                OscTimer->stop();
                break;
            }
            if (OscNum == 0)
            {
                emit SetDataSize(OscNumRemaining);
                OscNum = OscNumRemaining; // максимальный диапазон для прогрессбара
            }
            else
                emit SetDataCount(OscNum - OscNumRemaining);
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

bool EAbstractProtocomChannel::GetLength(bool ok)
{
    if (ReadDataChunk.size() < 4)
        return false;
    if (ReadDataChunk.at(0) == CN_SS)
    {
        if (ok)
        {
            if ((ReadDataChunk.at(1) == CN_ResOk) && (ReadDataChunk.at(2) == 0x00) && (ReadDataChunk.at(3) == 0x00))
                return true;
            else
                return false;
        }
        if (ReadDataChunk.at(1) != cmd)
            return false;
        RDLength = static_cast<quint8>(ReadDataChunk.at(2));
        RDLength += static_cast<quint8>(ReadDataChunk.at(3))*256; // посчитали длину посылки
        if (RDLength < CN_MAXSEGMENTLENGTH)
            LastBlock = true;
        else
            LastBlock = false;
        if (RDLength > MAXLENGTH)
            return false;
        else
            return true;
    }
    else
        return false;
}

void EAbstractProtocomChannel::SetWRSegNum()
{
    if (WRLength > CN_MAXSEGMENTLENGTH)
    {
        SegLeft = WRLength / CN_MAXSEGMENTLENGTH;
        SegEnd = CN_MAXSEGMENTLENGTH;
    }
    else
    {
        SegLeft = 1;
        SegEnd = WRLength;
    }
}

void EAbstractProtocomChannel::WRCheckForNextSegment()
{
    QByteArray tmpba;
    if (SegLeft)
    {
        --SegLeft;
        if (FirstSegment)
        {
            tmpba.append(CN_MS);
            FirstSegment = false;
        }
        else
            tmpba.append(CN_MS3);
        tmpba.append(cmd);
    }
    if (SegLeft)
    {
        AppendSize(tmpba, CN_MAXSEGMENTLENGTH);
        tmpba += WriteData.mid(SegEnd, CN_MAXSEGMENTLENGTH);
        SegEnd += CN_MAXSEGMENTLENGTH;
    }
    else
    {
        AppendSize(tmpba, SegEnd);
        tmpba += WriteData.right(SegEnd);
        WriteData.clear();
    }
    emit SetDataCount(SegEnd);
    WriteDataToPort(tmpba);
}

void EAbstractProtocomChannel::SendOk(bool cont)
{
    QByteArray tmpba;
    if (cont)
        tmpba.append(CN_MS3);
    else
        tmpba.append(CN_MS);
    tmpba.append(CN_ResOk);
    AppendSize(tmpba, 0);
    WriteDataToPort(tmpba); // отправляем "ОК" и переходим к следующему сегменту
}

void EAbstractProtocomChannel::AppendSize(QByteArray &ba, quint16 size)
{
    quint8 byte = static_cast<quint8>(size%0x100);
    ba.append(byte);
    byte = static_cast<quint8>(size/0x100);
    ba.append(byte);
}

void EAbstractProtocomChannel::SendErr()
{
    QByteArray tmpba;
    tmpba.append(CN_MS);
    tmpba.append(CN_ResErr);
    AppendSize(tmpba, 1);
    tmpba.append(CN_BYTE0); // модулю не нужны коды ошибок
    WriteDataToPort(tmpba);
}

void EAbstractProtocomChannel::Timeout()
{
    Finish(USO_TIMEOUTER);
}

void EAbstractProtocomChannel::Finish(int ernum)
{
    TTimer->stop();
    cmd = CN_Unk; // предотвращение вызова newdataarrived по приходу чего-то в канале, если ничего не было послано
    if (ernum != NOERROR)
    {
        if (ernum < 0)
        {
            log->WriteRaw("### ОШИБКА В ПЕРЕДАННЫХ ДАННЫХ ###");
            ERMSG("ОШИБКА В ПЕРЕДАННЫХ ДАННЫХ!!!");
        }
        else if (ernum < pc.errmsgs.size())
            ERMSG(pc.errmsgs.at(ernum));
        else
            ERMSG("Произошла неведомая фигня #"+QString::number(ernum,10));
    }
    result = ernum;
    Busy = false;
}

void EAbstractProtocomChannel::Disconnect()
{
    RawClose();
    emit Disconnected();
}

void EAbstractProtocomChannel::OscTimerTimeout()
{
    Send(CN_OscPg);
}

void EAbstractProtocomChannel::CheckForData()
{
    QByteArray ba = RawRead(1000);
    emit readbytessignal(ba);
    ParseIncomeData(ba);
}

void EAbstractProtocomChannel::WriteDataToPort(QByteArray &ba)
{
    QByteArray tmpba = ba;
    if (cmd == CN_Unk) // игнорируем вызовы процедуры без команды
    {
        pc.ErMsg(USB_WRONGCOMER);
        return;
    }
    quint64 byteswritten = 0;
    quint64 basize = tmpba.size();
    while ((byteswritten < basize) && (!tmpba.isEmpty()))
    {
        if (pc.WriteUSBLog)
        {
            log->WriteRaw("->");
            log->WriteRaw(tmpba.toHex());
            log->WriteRaw("\n");
        }
        qint64 tmpi = RawWrite(tmpba);
        if (tmpi == GENERALERROR)
        {
            pc.ErMsg(COM_WRITEER);
            return;
        }
        byteswritten += tmpi;
        emit writebytessignal(tmpba.left(tmpi));
        tmpba = tmpba.remove(0, tmpi);
    }
    TTimer->start();
}

void EAbstractProtocomChannel::SetCancelled()
{
    Cancelled = true;
}