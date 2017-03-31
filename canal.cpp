#include "Canal.h"
#include "config.h"

//#include <QtDebug>
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QTime>

Canal *cn;

Canal::Canal(QObject *parent) : QObject(parent)
{
    RDLength = 0;
    SegEnd = 0;
    SegLeft = 0;
    FirstRun = true;
    FirstSegment = true;
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
    else // BT_NONE
        BoardType = 0x00;
    InitiateSend();
}

void Canal::InitiateSend()
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
        WriteData.resize(4);
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
        WriteData.resize(5);
        WriteData.append(CN_MS);
        WriteData.append(cmd);
        AppendSize(WriteData, 1);
        WriteData.append(BoardType);
        WriteDataToPort(WriteData);
    }
    case CN_GF:     // запрос файла
    {
        if (DR == NULL)
        {
            SendErr();
            Finish(CN_NULLDATAERROR);
            break;
        }
        WriteData.resize(6);
        WriteData.append(CN_MS);
        WriteData.append(cmd);
        AppendSize(WriteData, 2);
        WriteData.append(static_cast<char>(fnum&0x00FF));
        WriteData.append(static_cast<char>((fnum&0xFF00)>>8));
//        qDebug() << "cnGf_1";
        WriteDataToPort(WriteData);
        break;
    }
    case CN_WHv:
    {
        WriteData.resize(5+sizeof(Bhb_Main));
        WriteData.append(CN_MS);
        WriteData.append(cmd);
        AppendSize(WriteData, 5); // BoardType, HiddenBlock
        WriteData.append(BoardType);
        memcpy(&(WriteData.data()[5]), &outdata[0], sizeof(Bhb_Main));
        WriteDataToPort(WriteData);
        break;
    }
    case CN_WF: // запись файла
    {
//        qDebug() << "cn1";
        if (DR == NULL)
            Finish(CN_NULLDATAERROR);
        WriteData.resize(CN_MAXFILESIZE);
        WriteData.append(CN_MS);
        WriteData.append(cmd);
        AppendSize(WriteData, 0); // временно записываем нулевую длину, впоследствии поменяем
        pc.StoreDataMem(&(WriteData.data()[4]), DR, fnum);
//        qDebug() << "cn1_5";
        // считываем длину файла из полученной в StoreDataMem и вычисляем количество сегментов
        WRLength = static_cast<quint8>(WriteData.at(11))*16777216; // с 8 байта начинается FileHeader.size
        WRLength += static_cast<quint8>(WriteData.at(10))*65536;
        WRLength += static_cast<quint8>(WriteData.at(9))*256;
        WRLength += static_cast<quint8>(WriteData.at(8));
        WRLength += sizeof(publicclass::FileHeader); // sizeof(FileHeader)
        WRLength += 4; // + 4 bytes prefix
        WriteData.resize(WRLength);
//        qDebug() << "cn1_7";
        emit writedatalength(WRLength); // сигнал для прогрессбара
        FirstSegment = true;
        SetWRSegNum();
        WRCheckForNextSegment();
//        qDebug() << "cn1_9";
        break;
    }
    case CN_WBac:
    case CN_CtEr:
    {
//        qDebug() << "cnWsn_1";
        WRLength = outdatasize+5;
        WriteData.resize(WRLength); // MS, c, L, L, B
        WriteData.append(CN_MS);
        WriteData.append(cmd);
        AppendSize(WriteData, 0); // временно записываем нулевую длину, впоследствии поменяем
        WriteData.append(QByteArray::fromRawData((const char *)outdata, outdatasize));
        emit writedatalength(WRLength); // сигнал для прогрессбара
        FirstSegment = true;
        SetWRSegNum();
        WRCheckForNextSegment();
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
    ReadDataChunk.clear();
    LastBlock = false;
    bStep = 0;
    RDLength = 0;
}

void Canal::ParseIncomeData(QByteArray &ba)
{
    if (cmd == CN_Unk) // игнорирование вызова процедуры, если не было послано никакой команды
        return;
    int res;
    ReadDataChunk.append(ba);
    quint32 RDSize = static_cast<quint32>(ReadDataChunk.size());
    if (RDSize<3) // ждём, пока принятый буфер не будет хотя бы длиной 3 байта или не произойдёт таймаут
        return;
    if (ReadDataChunk.at(1) == CN_ResErr)
    {
        if (RDSize < 5) // некорректная посылка
            Finish(CN_RCVDATAERROR);
        else
            Finish(USO_NOERR + ReadData.at(4));
        return;
    }
    if (ReadDataChunk.at(0) != CN_SS)
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
        case CN_GBTe:
        case CN_GF:
        {
            if (!GetLength(false)) // !ok = cmd
            {
                Finish(CN_RCVDATAERROR);
                return;
            }
            ReadDataChunk.remove(0, 4); // убираем заголовок с < и длиной
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
        emit bytesreceived(RDSize); // сигнал для прогрессбара
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
        {
            if (LastBlock)
            {
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
//            qDebug() << "cnGf_4";
//            qDebug() << "cnGf_5";
            if (LastBlock)
            {
                res = pc.RestoreDataMem(ReadData.data(), RDSize, DR);
                if (res == 0)
                    Finish(NOERROR);
                else
                {
                    SendErr();
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
                emit osceraseremaining(OscNum);
                OscNum = 0;
                Finish(NOERROR);
                OscTimer->stop();
                break;
            }
            if (OscNum == 0)
            {
                emit oscerasesize(OscNumRemaining);
                OscNum = OscNumRemaining; // максимальный диапазон для прогрессбара
            }
            else
                emit osceraseremaining(OscNum - OscNumRemaining);
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

bool Canal::GetLength(bool ok)
{
    if (ReadDataChunk.size() < 4)
        return false;
    if (ReadDataChunk.at(0) == CN_SS)
    {
        if (ok)
        {
            if ((ReadDataChunk.at(1) == CN_ResOk) && (ReadDataChunk.at(2) == 0x00) && (ReadDataChunk.at(3) == 0x00))
                return 0;
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

void Canal::SetWRSegNum()
{
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
}

void Canal::WRCheckForNextSegment()
{
    if (SegLeft)
    {
        --SegLeft;
        QByteArray tmpba;
        if (FirstSegment)
        {
            tmpba.append(CN_MS);
            FirstSegment = false;
        }
        else
            tmpba.append(CN_MS3);
        tmpba.append(cmd);
        if (SegLeft)
        {
            AppendSize(tmpba, CN_MAXSEGMENTLENGTH);
            tmpba += WriteData.mid(SegEnd, CN_MAXSEGMENTLENGTH);
            SegEnd += CN_MAXSEGMENTLENGTH;
        }
        else
        {
            AppendSize(tmpba, WriteData.size()-SegEnd);
            tmpba += WriteData.right(WriteData.size()-SegEnd);
            WriteData.clear();
        }
        emit writedatapos(SegEnd);
        WriteDataToPort(tmpba);
    }
}

void Canal::SendOk(bool cont)
{
    WriteData.clear();
    if (cont)
        WriteData.append(CN_MS3);
    else
        WriteData.append(CN_MS);
    WriteData.append(CN_ResOk);
    AppendSize(WriteData, 0);
    WriteDataToPort(WriteData); // отправляем "ОК" и переходим к следующему сегменту
}

void Canal::AppendSize(QByteArray &ba, quint16 size)
{
    quint8 byte = static_cast<quint8>(size%0x100);
    ba.append(byte);
    byte = static_cast<quint8>(size/0x100);
    ba.append(byte);
}

void Canal::SendErr()
{
    QByteArray tmpba;
    tmpba.append(CN_MS);
    tmpba.append(CN_ResErr);
    AppendSize(tmpba, 1);
    tmpba.append(CN_BYTE0); // модулю не нужны коды ошибок
    WriteDataToPort(tmpba);
}

void Canal::Timeout()
{
    Finish(CN_TIMEOUT);
    emit sendend();
}

void Canal::Finish(int ernum)
{
    TTimer->stop();
    cmd = CN_Unk; // предотвращение вызова newdataarrived по приходу чего-то в канале, если ничего не было послано
    if (ernum != NOERROR)
    {
        if (ernum < pc.errmsgs.size())
            ERMSG(pc.errmsgs.at(ernum));
        else
            ERMSG("Произошла неведомая фигня #"+QString::number(ernum,10));
    }
    result = ernum;
    Busy = false;
    emit sendend();
//    qDebug() << "Finish";
}

bool Canal::Connect()
{
    if (!InitializePort(info, baud))
        return false;
    return true;
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
    return true;
}

void Canal::ClosePort()
{
    try
    {
        port->close();
        QTimer *tmr = new QTimer;
        tmr->setInterval(CN_TIMEOUT);
        connect(tmr,SIGNAL(timeout()),this,SLOT(PortCloseTimeout()));
        PortCloseTimeoutSet = false;
        tmr->start();
        while ((port->isOpen() && (!PortCloseTimeoutSet)))
        {
            QTime tme;
            tme.start();
            while (tme.elapsed() < CN_MAINLOOP_DELAY)
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
    emit readbytessignal(ba);
    ParseIncomeData(ba);
}

void Canal::Error(QSerialPort::SerialPortError err)
{
    if (!err) // нет ошибок
        return;
    quint16 ernum = err + COM_ERROR;
    pc.ErMsg(ernum);
}

void Canal::PortCloseTimeout()
{
    PortCloseTimeoutSet = true;
}

bool Canal::WriteDataToPort(QByteArray &ba)
{
    if (cmd == CN_Unk) // игнорируем вызовы процедуры без команды
        return false;
    quint64 byteswritten = 0;
    quint64 basize = ba.size();
    while ((byteswritten < basize) && (!ba.isEmpty()))
    {
        qint64 tmpi = port->write(ba);
        if (tmpi == GENERALERROR)
            return false;
        byteswritten += tmpi;
        emit writebytessignal(ba.left(tmpi));
        if (tmpi <= ba.size())
            ba = ba.remove(0, tmpi);
        else
            ba.clear();
    }
    TTimer->start();
    return true;
}
