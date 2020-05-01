#include <QCoreApplication>
#include <QStandardPaths>
#include <QTime>
#include "error.h"
#include "modulebsi.h"
#include "eabstractprotocomchannel.h"

bool EAbstractProtocomChannel::WriteUSBLog = false;

EAbstractProtocomChannel::EAbstractProtocomChannel(QObject *parent) : QObject(parent)
{
    QString tmps = "=== CLog started ===\n";
    CnLog = new Log;
    CnLog->Init("canal.log");
    CnLog->WriteRaw(tmps.toUtf8());
    RDLength = 0;
    SegEnd = 0;
    SegLeft = 0;
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

void EAbstractProtocomChannel::Send(char command, char board_type, void *ptr, int ptrsize, int filenum, QVector<S2::DataRec> *DRptr)
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
    if (board_type == BoardTypes::BT_BASE)
        BoardType = 0x01;
    else if (board_type == BoardTypes::BT_MEZONIN)
        BoardType = 0x02;
    else if (board_type == BoardTypes::BT_NONE)
        BoardType = 0x00;
    else
        BoardType = board_type; // in GBd command it is a block number
    InitiateSend();
    QEventLoop loop;
    connect(this, SIGNAL(QueryFinished()), &loop, SLOT(quit()));
    loop.exec();
}

void EAbstractProtocomChannel::SetWriteUSBLog(bool bit)
{
    WriteUSBLog = bit;
}

bool EAbstractProtocomChannel::IsWriteUSBLog()
{
    return WriteUSBLog;
}

void EAbstractProtocomChannel::TranslateDeviceAndSave(const QString &str)
{
/*#ifdef COMPORTENABLE
    ComPort = str;
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    sets->setValue("Port", ComPort);
#endif*/
//#ifdef USBENABLE
    // формат строки: "VEN_" + QString::number(venid, 16) + "_ & DEV_" + QString::number(prodid, 16) + "_ & SN_" + sn;
    QStringList sl = str.split("_"); // 1, 3 и 5 - полезная нагрузка
    if (sl.size() < 6)
    {
        DBGMSG;
        return;
    }
    QString tmps = sl.at(1);
    UsbPort.vendor_id = tmps.toUShort(nullptr, 16);
    tmps = sl.at(3);
    UsbPort.product_id = tmps.toUShort(nullptr, 16);
    tmps = sl.at(5);
    int z = tmps.toWCharArray(UsbPort.serial);
    UsbPort.serial[z] = '\x0';
//#endif
}

void EAbstractProtocomChannel::InitiateSend()
{
    WriteData.clear();
    switch (cmd)
    {
    case CN_GBsi:   // запрос блока стартовой информации
    case CN_ErPg:  // запрос текущего прогресса
    case CN_GVar:
    case CN_GMode:
    {
        WriteData.append(CN_MS);
        WriteData.append(cmd);
        AppendSize(WriteData, 0);
        WriteDataToPort(WriteData);
        break;
    }
    case CN_GBac:   // чтение настроечных коэффициентов
    case CN_GBda:   // чтение текущих данных без настройки
    case CN_GBd:    // запрос блока (подблока) текущих данных
    case CN_NVar:
    case CN_SMode:
    case CN_GBt:    // чтение технологического блока
    case CN_Ert:  // команда стирания технологического блока
    {
        WriteData.append(CN_MS);
        WriteData.append(cmd);
        AppendSize(WriteData, 1);
        WriteData.append(BoardType);
        WriteDataToPort(WriteData);
        break;
    }
    case CN_GF:     // запрос файла
    {
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
        int size = (BoardType == BoardTypes::BT_BSMZ) ? WHV_SIZE_TWOBOARDS : WHV_SIZE_ONEBOARD;
        AppendSize(WriteData, size); // BoardType(1), HiddenBlock(16)
        WriteData.append(BoardType);
        WriteData.resize(WriteData.size()+outdatasize);
        size_t tmpi = static_cast<size_t>(outdatasize);
        memcpy(&(WriteData.data()[5]), &outdata[0], tmpi);
        WriteDataToPort(WriteData);
        break;
    }
    case CN_WF: // запись файла
    {
        if (DR->isEmpty())
            Finish(CN_NULLDATAERROR);
        WriteData.resize(CN_MAXFILESIZE);
        S2::StoreDataMem(&(WriteData.data()[0]), DR, fnum);
        // считываем длину файла из полученной в StoreDataMem и вычисляем количество сегментов
        WRLength = static_cast<quint8>(WriteData.at(7))*16777216; // с 4 байта начинается FileHeader.size
        WRLength += static_cast<quint8>(WriteData.at(6))*65536;
        WRLength += static_cast<quint8>(WriteData.at(5))*256;
        WRLength += static_cast<quint8>(WriteData.at(4));
        WRLength += sizeof(S2::FileHeader); // sizeof(FileHeader)
        WriteData.resize(WRLength);
        emit SetDataSize(WRLength); // сигнал для прогрессбара
        SetWRSegNum();
        WRCheckForNextSegment(true);
        break;
    }
    case CN_WBac:
    case CN_CtEr:
    case CN_WBt:
    {
        WriteData.append(BoardType);
        WriteData.append(QByteArray::fromRawData(static_cast<const char *>(outdata), outdatasize));
        WRLength = outdatasize + 1;
        emit SetDataSize(WRLength); // сигнал для прогрессбара
        SetWRSegNum();
        WRCheckForNextSegment(true);
        break;
    }     
    case CN_GTime:
    {
        WriteData.append(CN_MS);
        WriteData.append(cmd);
        AppendSize(WriteData, 0);
        WriteDataToPort(WriteData);
        break;
    }
    case CN_WTime:
    {
        WriteData.append(CN_MS);
        WriteData.append(cmd);
        AppendSize(WriteData, outdatasize);
        WriteData.resize(WriteData.size()+outdatasize);
        size_t tmpi = static_cast<size_t>(outdatasize);
        memcpy(&(WriteData.data()[4]), &outdata[0], tmpi);
        WriteDataToPort(WriteData);
        break;
    }
    case CN_WBd:
    {
        WriteData.append(CN_MS);
        WriteData.append(cmd);
        AppendSize(WriteData, outdatasize);
        WriteData.append(BoardType);
        WriteData.resize(WriteData.size()+outdatasize);
        size_t tmpi = static_cast<size_t>(outdatasize);
        memcpy(&(WriteData.data()[5]), &outdata[0], tmpi);
        WriteDataToPort(WriteData);
        break;
    }

    case CN_WCom:
    {
        WriteData.append(CN_MS);
        WriteData.append(cmd);
        AppendSize(WriteData, 1);
        WriteData.append(BoardType);
        WriteData.resize(WriteData.size());
        WriteDataToPort(WriteData);
        break;
    }

    case CN_VPO:
    {
        WriteData.append(CN_MS);
        WriteData.append(cmd);
        AppendSize(WriteData, 0);
        WriteDataToPort(WriteData);
        break;
    }

    case CN_STest:
    {
        WriteData.append(CN_MS);
        WriteData.append(cmd);
        AppendSize(WriteData, 1);
        WriteData.append(BoardType);
        WriteDataToPort(WriteData);
        break;
    }

    default:
    {
        Finish(CN_UNKNOWNCMDERROR);
        return;
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
    emit readbytessignal(ba);
    if (WriteUSBLog)
    {
        QByteArray tmps = "<-" + ba.toHex() + "\n";
        CnLog->WriteRaw(tmps);
    }
    if (cmd == CN_Unk) // игнорирование вызова процедуры, если не было послано никакой команды
        return;
    int res;
    ReadDataChunk.append(ba);
    RDSize = ReadDataChunk.size();
    if (RDSize<4) // ждём, пока принятый буфер не будет хотя бы длиной 3 байта или не произойдёт таймаут
        return;
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
    switch (bStep)
    {
        case 0: // первая порция
        {
            switch (cmd)
            {
            // команды с ответом "ОК"
            case CN_WHv:
            case CN_Ert:
            case CN_CtEr:
            case CN_NVar:
            case CN_SMode:
            case CN_WTime:
            case CN_WBd:
            case CN_WCom:
            case CN_VPO:
            case CN_STest:
            {
                if ((ReadDataChunk.at(1) != CN_ResOk) || (ReadDataChunk.at(2) != 0x00) || (ReadDataChunk.at(3) != 0x00))
                {
                    Finish(CN_RCVDATAERROR);
                    return;
                }
                if (cmd == CN_Ert)
                    OscTimer->start(); // start timer to send ErPg command periodically
                Finish(Error::ER_NOERROR);
                return;
            }
            // команды с ответом "ОК" и с продолжением
            case CN_WF:
            case CN_WBac:
            case CN_WBt:
            {
                if ((ReadDataChunk.at(1) != CN_ResOk) || (ReadDataChunk.at(2) != 0x00) || (ReadDataChunk.at(3) != 0x00))
                {
                    Finish(CN_RCVDATAERROR);
                    return;
                }
                if (!SegLeft)
                {
                    Finish(Error::ER_NOERROR);
                    return;
                }
                ReadDataChunk.clear();
                WRCheckForNextSegment(false);
                return;
            }
            // команды с ответом SS c L L ... и продолжением
            case CN_GBsi:
            case CN_GBda:
            case CN_GBac:
            case CN_GBd:
            case CN_ErPg:
            case CN_GBt:
            case CN_GF:
            case CN_GVar:
            case CN_GMode:
            case CN_GTime:
            {
                if (!GetLength())
                {
                    Finish(CN_RCVDATAERROR);
                    return;
                }
                if (ReadDataChunkLength == 0)
                {
                    RDSize = 0;
                    Finish(Error::ER_NOERROR);
                    return;
                }
                if (cmd == CN_GF) // надо проверить, тот ли номер файла принимаем
                {
                    if (RDSize < 16) // не пришла ещё шапка файла
                        return;
                    quint16 filenum;
                    quint8 tmpi= ReadDataChunk[5];
                    filenum = static_cast<unsigned short>(tmpi) * 256;
                    tmpi = ReadDataChunk[4];
                    filenum += tmpi;
                    if (filenum != fnum)
                    {
                        Finish(USO_UNKNFILESENT);
                        return;
                    }
                    memcpy(&RDLength, &(ReadDataChunk.data()[8]), sizeof(RDLength));
                    RDLength += 16;
                    emit SetDataSize(RDLength);
                }
                else if (cmd == CN_ErPg)
                    emit SetDataSize(100);
                else
                    emit SetDataSize(0); // длина неизвестна для команд с ответами без длины
                bStep++;
                break;
            }

            default:
                Finish(CN_UNKNOWNCMDERROR);
                break;
            }
        }

        [[clang::fallthrough]]; case 1:
        {
            if (!GetLength())
            {
                Finish(CN_RCVDATAERROR);
                return;
            }
            if (RDSize < ReadDataChunkLength)
                return; // пока не набрали целый буфер соответственно присланной длине или не произошёл таймаут
            ReadDataChunk.remove(0, 4); // убираем заголовок с < и длиной
            ReadData.append(&(ReadDataChunk.data()[0]), ReadDataChunkLength);
            RDSize = ReadData.size();
            emit SetDataCount(RDSize); // сигнал для прогрессбара
            ReadDataChunk.clear();
            switch (cmd)
            {
            case CN_GBsi:
            case CN_GBda:
            case CN_GBac:
            case CN_GBd:
            case CN_GBt:
            case CN_GVar:
            case CN_GMode:
            case CN_GTime:
            {
                if ((RDSize >= outdatasize) || (ReadDataChunkLength < CN_MAXSEGMENTLENGTH))
                {
                    emit SetDataSize(RDSize); // установка размера прогрессбара, чтобы не мелькал
                    RDSize = qMin(outdatasize, RDSize); // если даже приняли больше, копируем только требуемый размер
                    size_t tmpi = static_cast<size_t>(RDSize);
                    memcpy(outdata,ReadData.data(),tmpi);
                    Finish(Error::ER_NOERROR);
                }
                else
                    SendOk(true);
                break;
            }
            case CN_GF:
            {
                if (RDSize >= RDLength)
                {
                    if ((fnum >= CN_MINOSCID) && (fnum <= CN_MAXOSCID)) // для осциллограмм особая обработка
                    {
                        QVector<S2::DataRec> DRosc;
                        RDSize = qMin(RDLength, RDSize); // если даже приняли больше, копируем только требуемый размер
                        size_t tmpi = static_cast<size_t>(RDSize);
                        memcpy(outdata,ReadData.data(),tmpi);
                        DRosc.append({static_cast<quint32>(ReadData.data()[16]),static_cast<quint32>(ReadData.data()[20]),&ReadData.data()[24]});
                        tmpi = 8; //sizeof(FileHeader);
                        memcpy(&DRosc.data()[0],&ReadData.data()[16],tmpi);
                        Finish(Error::ER_NOERROR);

                        if (DRosc.isEmpty())
                        {
                            Finish(CN_NULLDATAERROR);
                            break;
                        }
                        res = S2::RestoreDataMem(ReadData.data(), RDSize, &DRosc);

                        break;
                    }

                    if ((fnum >= MINJOURID) && (fnum <= MAXJOURID)) // для осциллограмм особая обработка
                    {
                        QVector<S2::DataRec> DRJour;
                        RDSize = qMin(RDLength, RDSize); // если даже приняли больше, копируем только требуемый размер
                        size_t tmpi = static_cast<size_t>(RDSize);
                        memcpy(outdata,ReadData.data(),tmpi);
                        DRJour.append({static_cast<quint32>(ReadData.data()[16]),static_cast<quint32>(ReadData.data()[20]),&ReadData.data()[24]});
                        tmpi = 8; //sizeof(FileHeader);
                        memcpy(&DRJour.data()[0],&ReadData.data()[16],8);
                        Finish(Error::ER_NOERROR);

                        if (DRosc.isEmpty())
                        {
                            Finish(CN_NULLDATAERROR);
                            break;
                        }
                        res = S2::RestoreDataMem(ReadData.data(), RDSize, &DRJour);

                        break;
                    }

                    if (DR->isEmpty())
                    {
                        Finish(CN_NULLDATAERROR);
                        break;
                    }
                    res = S2::RestoreDataMem(ReadData.data(), RDSize, DR);
                    if (res == 0)
                    {
    //                    SendOk(false);
                        Finish(Error::ER_NOERROR);
                    }
                    else
                        Finish(res);
                }
                else
                    SendOk(true);
                break;
            }
            case CN_ErPg:
            {
                quint16 OscNum = static_cast<quint8>(ReadData.at(0))+static_cast<quint8>(ReadData.at(1))*256;
                emit SetDataCount(OscNum);
                if (OscNum == 100)
                {
                    Finish(Error::ER_NOERROR);
                    OscTimer->stop();
                    break;
                }
                break;
            }


            default:
            {
                Finish(CN_UNKNOWNCMDERROR);
                break;
            }
            }
    //        bStep = 0;
        }
    }
}

bool EAbstractProtocomChannel::GetLength()
{
    if (ReadDataChunk.at(1) != cmd)
        return false;
    ReadDataChunkLength = static_cast<quint8>(ReadDataChunk.at(2));
    ReadDataChunkLength += static_cast<quint8>(ReadDataChunk.at(3))*256; // посчитали длину посылки
    return true;
}

void EAbstractProtocomChannel::SetWRSegNum()
{
    if (WRLength > CN_MAXSEGMENTLENGTH)
        SegLeft = (WRLength / CN_MAXSEGMENTLENGTH) + 1;
    else
        SegLeft = 1;
    SegEnd = 0;
}

void EAbstractProtocomChannel::WRCheckForNextSegment(int first)
{
    //quint8 ForMihalich = 0;
    QByteArray tmpba;
    if (SegLeft)
    {
        --SegLeft;
        if (first)
            tmpba.append(CN_MS);
        else
            tmpba.append(CN_MS3);
        tmpba.append(cmd);
    }
    if (SegLeft)
    {
        AppendSize(tmpba, CN_MAXSEGMENTLENGTH);
        tmpba += WriteData.mid(SegEnd, CN_MAXSEGMENTLENGTH); // -4 = '<', cmd, L, L
        SegEnd += CN_MAXSEGMENTLENGTH;
    }
    else
    {
        AppendSize(tmpba, (WriteData.size() - SegEnd));
        tmpba += WriteData.right(WriteData.size() - SegEnd);
        SegEnd = WriteData.size();
        WriteData.clear();
        //ForMihalich = 1;
    }
    emit SetDataCount(SegEnd);
    WriteDataToPort(tmpba);

    //if(ForMihalich == 1)
    //SendOk(true);
}

void EAbstractProtocomChannel::SendOk(bool cont)
{
    QByteArray tmpba;
    if (cont)
        tmpba.append(CN_MS3);
    else
        tmpba.append(CN_MS);
    tmpba.append(cmd);
    AppendSize(tmpba, 0);
    WriteDataToPort(tmpba); // отправляем "ОК" и переходим к следующему сегменту
}

void EAbstractProtocomChannel::AppendSize(QByteArray &ba, int size)
{
    char byte = static_cast<char>(size%0x100);
    ba.append(byte);
    byte = static_cast<char>(size/0x100);
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
    if (ernum != Error::ER_NOERROR)
    {
        if (ernum < 0)
        {
            CnLog->WriteRaw("### ОШИБКА В ПЕРЕДАННЫХ ДАННЫХ ###\n");
            WARNMSG("ОШИБКА В ПЕРЕДАННЫХ ДАННЫХ!!!");
        }
        else
            Error::ShowErMsg(ernum);
    }
    result = ernum;
    emit QueryFinished();
//    Busy = false;
}

void EAbstractProtocomChannel::Disconnect()
{
    RawClose();
    CnLog->WriteRaw("Disconnected!\n");
}

void EAbstractProtocomChannel::OscTimerTimeout()
{
    Send(CN_ErPg);
}

void EAbstractProtocomChannel::CheckForData()
{
    QByteArray ba = RawRead(1000);
    ParseIncomeData(ba);
}

void EAbstractProtocomChannel::WriteDataToPort(QByteArray &ba)
{
    QByteArray tmpba = ba;
    if (cmd == CN_Unk) // игнорируем вызовы процедуры без команды
    {
        Error::ShowErMsg(USB_WRONGCOMER);
        return;
    }
    int byteswritten = 0;
    int basize = tmpba.size();
    while ((byteswritten < basize) && (!tmpba.isEmpty()))
    {
        if (WriteUSBLog)
        {
            QByteArray tmps = "->" + tmpba.toHex() + "\n";
            CnLog->WriteRaw(tmps);
        }
        int tmpi = RawWrite(tmpba);
        if (tmpi == Error::ER_GENERALERROR)
        {
            Error::ShowErMsg(COM_WRITEER);
            Disconnect();
        }
        byteswritten += tmpi;
        emit writebytessignal(tmpba.left(tmpi));
        tmpba = tmpba.remove(0, tmpi);
    }
#ifndef NOTIMEOUT
    TTimer->start();
#endif
}

void EAbstractProtocomChannel::SetCancelled()
{
    Cancelled = true;
}
