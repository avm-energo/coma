#include <QCoreApplication>
#include <QStandardPaths>
#include <QSettings>
#include <QTime>
#include <QtGlobal>
#include "error.h"
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
//    OscNum = 0;
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
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    SetWriteUSBLog(sets->value("WriteLog", "0").toBool());
}

EAbstractProtocomChannel::~EAbstractProtocomChannel()
{
}

void EAbstractProtocomChannel::Send(char command, char board_type)
{
    // only for these commands
    switch(command)
    {
    case CN_ErPg:
    case CN_VPO:
        break;
    default:
        return;
    }

    if (!Connected)
    {
        Result = CN_NULLDATAERROR;
        return;
    }
    Command = command;
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

void EAbstractProtocomChannel::Send(char command, char board_type, QByteArray &ba)
{
    if (!Connected)
    {
        Result = CN_NULLDATAERROR;
        return;
    }
    InData = ba;
//    InDataSize = ptrsize; // размер области данных, в которую производить запись
    Command = command;
//    FNum = filenum;
//    DR = DRptr;
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

void EAbstractProtocomChannel::SendFile(unsigned char command, unsigned char board_type, int filenum, QByteArray &ba)
{
    if (!Connected)
    {
        Result = CN_NULLDATAERROR;
        return;
    }
    Command = command;
    FNum = filenum;
    InData = ba;
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
    ba = OutData;
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
}

void EAbstractProtocomChannel::InitiateSend()
{
    WriteData.clear();
    switch (Command)
    {
    case CN_GBsi:   // запрос блока стартовой информации
    case CN_ErPg:  // запрос текущего прогресса
    case CN_GVar:
    case CN_GMode:
    {
        WriteData.append(CN_MS);
        WriteData.append(Command);
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
        WriteData.append(Command);
        AppendSize(WriteData, 1);
        WriteData.append(BoardType);
        WriteDataToPort(WriteData);
        break;
    }
    case CN_GF:     // запрос файла
    {
        WriteData.append(CN_MS);
        WriteData.append(Command);
        AppendSize(WriteData, 2);
        WriteData.append(static_cast<char>(FNum&0x00FF));
        WriteData.append(static_cast<char>((FNum&0xFF00)>>8));
        WriteDataToPort(WriteData);
        break;
    }
    case CN_WHv:
    {
        WriteData.append(CN_MS);
        WriteData.append(Command);
        int size = (BoardType == BoardTypes::BT_BSMZ) ? WHV_SIZE_TWOBOARDS : WHV_SIZE_ONEBOARD;
        AppendSize(WriteData, size); // BoardType(1), HiddenBlock(16)
        WriteData.append(BoardType);
//        WriteData.resize(WriteData.size()+InDataSize);
//        size_t tmpi = static_cast<size_t>(InDataSize);
        WriteData.append(InData);
//        memcpy(&(WriteData.data()[5]), &OutData[0], tmpi);
        WriteDataToPort(WriteData);
        break;
    }
    case CN_WF: // запись файла
    {
        WriteData = InData;
        WRLength = WriteData.length();
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
//        WriteData.append(QByteArray::fromRawData(reinterpret_cast<const char *>(OutData.data()), OutDataSize));
        WriteData.append(InData);
        WRLength = InDataSize + 1;
        emit SetDataSize(WRLength); // сигнал для прогрессбара
        SetWRSegNum();
        WRCheckForNextSegment(true);
        break;
    }     
    case CN_GTime:
    {
        WriteData.append(CN_MS);
        WriteData.append(Command);
        AppendSize(WriteData, 0);
        WriteDataToPort(WriteData);
        break;
    }
    case CN_WTime:
    {
        WriteData.append(CN_MS);
        WriteData.append(Command);
        AppendSize(WriteData, InDataSize);
//        WriteData.resize(WriteData.size()+OutDataSize);
//        size_t tmpi = static_cast<size_t>(OutDataSize);
//        memcpy(&(WriteData.data()[4]), &OutData[0], tmpi);
        WriteData.append(InData);
        WriteDataToPort(WriteData);
        break;
    }
    case CN_WBd:
    {
        WriteData.append(CN_MS);
        WriteData.append(Command);
        AppendSize(WriteData, InDataSize);
        WriteData.append(BoardType);
/*        WriteData.resize(WriteData.size()+OutDataSize);
        size_t tmpi = static_cast<size_t>(OutDataSize);
        memcpy(&(WriteData.data()[5]), &OutData[0], tmpi); */
        WriteData.append(InData);
        WriteDataToPort(WriteData);
        break;
    }

    case CN_WCom:
    {
        WriteData.append(CN_MS);
        WriteData.append(Command);
        AppendSize(WriteData, 1);
        WriteData.append(BoardType);
//        WriteData.resize(WriteData.size());
        WriteDataToPort(WriteData);
        break;
    }

    case CN_VPO:
    {
        WriteData.append(CN_MS);
        WriteData.append(Command);
        AppendSize(WriteData, 0);
        WriteDataToPort(WriteData);
        break;
    }

    case CN_STest:
    {
        WriteData.append(CN_MS);
        WriteData.append(Command);
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
    OutData.clear();
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
    if (Command == CN_Unk) // игнорирование вызова процедуры, если не было послано никакой команды
        return;
//    int res;
    ReadDataChunk.append(ba);
    quint32 rdsize = ReadDataChunk.size();
    if (rdsize<4) // ждём, пока принятый буфер не будет хотя бы длиной 3 байта или не произойдёт таймаут
        return;
    if (ReadDataChunk.at(0) != CN_SS)
    {
        Finish(CN_RCVDATAERROR);
        return;
    }
    if (ReadDataChunk.at(1) == CN_ResErr)
    {
        if (rdsize < 5) // некорректная посылка
            Finish(CN_RCVDATAERROR);
        else
            Finish(USO_NOERR + ReadDataChunk.at(4));
        return;
    }
    switch (bStep)
    {
        case 0: // первая порция
        {
            switch (Command)
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
                if (Command == CN_Ert)
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
                    rdsize = 0;
                    Finish(Error::ER_NOERROR);
                    return;
                }
                if (Command == CN_ErPg)
                    emit SetDataSize(100);
                else
                    emit SetDataSize(0); // длина неизвестна для команд с ответами без длины
                bStep++;
                break;
            }
            case CN_GF:
            {
                if (!GetLength())
                {
                    Finish(CN_RCVDATAERROR);
                    return;
                }
                if (ReadDataChunkLength == 0)
                {
                    rdsize = 0;
                    Finish(Error::ER_NOERROR);
                    return;
                }
                // надо проверить, тот ли номер файла принимаем
                if (rdsize < 16) // не пришла ещё шапка файла
                    return;
                // шапка:
                // WORD fname;		// имя файла
                // WORD service;	// сервисное слово (по умолчанию 0xFF)
                // DWORD size;		// размер файла (без заголовка)
                // DWORD crc32;     // контрольная сумма
                // DWORD thetime;	// время создания файла

                quint8 tmpi = ReadDataChunk[5];
                quint16 filenum = static_cast<unsigned short>(tmpi) * 256;
                tmpi = ReadDataChunk[4];
                filenum += tmpi;
                if (filenum != FNum)
                {
                    Finish(USO_UNKNFILESENT);
                    return;
                }
                // вытаскиваем размер файла
                memcpy(&RDLength, &(ReadDataChunk.data()[8]), sizeof(RDLength));
                RDLength += 16;
                if (RDLength > CN_MAXGETFILESIZE) // размер файла должен быть не более 16М
                {
                    Finish(CN_RCVLENGTHERROR);
                    return;
                }
                emit SetDataSize(RDLength);
                OutData.resize(RDLength);
//                RDCount = 0;
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
            if (rdsize < ReadDataChunkLength)
                return; // пока не набрали целый буфер соответственно присланной длине или не произошёл таймаут
            ReadDataChunk.remove(0, 4); // убираем заголовок с < и длиной
            switch (Command)
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
                // команды с чтением определённого InDataSize количества байт из устройства
                ReadDataChunk.truncate(ReadDataChunkLength);
                OutData.append(ReadDataChunk);
                int outdatasize = OutData.size();
//                RDSize = OutData.size();
                emit SetDataCount(outdatasize); // сигнал для прогрессбара
                ReadDataChunk.clear();
                if ((outdatasize >= InDataSize) || (ReadDataChunkLength < CN_MAXSEGMENTLENGTH))
                {
                    emit SetDataSize(outdatasize); // установка размера прогрессбара, чтобы не мелькал
//                    RDSize = qMin(InDataSize, RDSize); // если даже приняли больше, копируем только требуемый размер
//                    size_t tmpi = static_cast<size_t>(RDSize);
//                    memcpy(OutData,ReadData.data(),tmpi);
//                    memcpy(OutData.data(),ReadData.data(),RDSize);
                    Finish(Error::ER_NOERROR);
                }
                else
                    SendOk(true);
                break;
            }
            case CN_GF:
            {
                // чтение файла количеством байт RDLength = (sizeof(FileHeader) + size)
                int outdatasize = OutData.size();
                int tmpi = outdatasize + ReadDataChunkLength;
                if (tmpi > RDLength) // проверка на выход за диапазон
                    // копируем только требуемое количество байт
                    ReadDataChunkLength = RDLength - outdatasize;
                ReadDataChunk.truncate(ReadDataChunkLength);
//                memcpy(OutData.data() + RDCount, &ReadDataChunk.data()[0], ReadDataChunkLength);
                OutData.append(ReadDataChunk);
//                RDCount += ReadDataChunkLength;
                outdatasize += ReadDataChunkLength;
                emit SetDataCount(outdatasize); // сигнал для прогрессбара
                ReadDataChunk.clear();
                if (outdatasize >= RDLength)
                {
//                    OutDataSize = RDLength;
                    Finish(Error::ER_NOERROR);
                    return;
                }
                else
                    SendOk(true);
                break;
            }
            case CN_ErPg:
            {
                quint16 OscNum = static_cast<quint8>(ReadDataChunk.at(0))+static_cast<quint8>(ReadDataChunk.at(1))*256;
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
    if (ReadDataChunk.at(1) != Command)
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
        tmpba.append(Command);
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
    tmpba.append(Command);
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
    Command = CN_Unk; // предотвращение вызова newdataarrived по приходу чего-то в канале, если ничего не было послано
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
    Result = ernum;
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
    if (Command == CN_Unk) // игнорируем вызовы процедуры без команды
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
