#include "eprotocom.h"

#include "../gen/error.h"
#include "../gen/stdfunc.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QSettings>

bool EProtocom::m_writeUSBLog;
EProtocom *EProtocom::pinstance_ { nullptr };
QMutex EProtocom::mutex_;

EProtocom::EProtocom(QObject *parent)
{
    Q_UNUSED(parent);
    QString tmps = "=== CLog started ===\n";
    CnLog = new LogClass;
    CnLog->Init("canal.log");
    CnLog->WriteRaw(tmps.toUtf8());
    RDLength = 0;
    SegEnd = 0;
    SegLeft = 0;
    OscNum = 0;
    // setConnected(false);
    TTimer = new QTimer(this);
    TTimer->setInterval(CN::Timeout);
    OscTimer = new QTimer(this);
    OscTimer->setInterval(CN::TimeoutOscillogram);
    OscTimer->setSingleShot(false);
    connect(OscTimer, SIGNAL(timeout()), this, SLOT(OscTimerTimeout()));
    connect(TTimer, SIGNAL(timeout()), this, SLOT(Timeout())); // для отладки закомментарить
    QSettings *sets = new QSettings("EvelSoft", PROGNAME);
    setWriteUSBLog(sets->value("WriteLog", "0").toBool());
}

qint32 EProtocom::result() const
{
    return m_result;
}

void EProtocom::setResult(const qint32 &result)
{
    m_result = result;
}

bool EProtocom::isWriteUSBLog()
{
    return m_writeUSBLog;
}

void EProtocom::setWriteUSBLog(bool writeUSBLog)
{
    m_writeUSBLog = writeUSBLog;
}

void EProtocom::Send(char command, char parameter, QByteArray &ba, qint64 datasize)
{
    if (Board::GetInstance()->connectionState() == Board::ConnectionState::ClosingState
        && Board::GetInstance()->interfaceType() == Board::InterfaceType::USB)
    {
        qDebug() << "Передача в отключенный прибор";
        ERMSG("В канальную процедуру переданы некорректные данные");
        m_result = CN_NULLDATAERROR;
        return;
    }
    InData = ba;
    InDataSize = datasize; // размер области данных, в которую производить запись
    Command = command;
    /*    if (parameter == BoardTypes::BT_BASE)
            BoardType = 0x01;
        else if (parameter == BoardTypes::BT_MEZONIN)
            BoardType = 0x02;
        else if (parameter == BoardTypes::BT_NONE)
            BoardType = 0x00;
        else */
    BoardType = parameter; // in GBd command it is a block number
    InitiateSend();
    QEventLoop loop;
    connect(this, &EProtocom::QueryFinished, &loop, &QEventLoop::quit);
    loop.exec();
}

void EProtocom::InitiateSend()
{
    WriteData.clear();
    switch (Command)
    {
    case CN::Read::BlkStartInfo: // запрос блока стартовой информации
    case CN::Read::Progress:     // запрос текущего прогресса
    case CN::Read::Variant:
    case CN::Read::Mode:
    case CN::Read::Time:
    case CN::Write::Upgrade:
    {
        WriteData.append(CN::Message::Start);
        WriteData.append(Command);
        AppendSize(WriteData, 0);
        WriteDataToPort(WriteData);
        break;
    }
    case CN::Read::BlkAC:    // чтение настроечных коэффициентов
    case CN::Read::BlkDataA: // чтение текущих данных без настройки
    case CN::Read::BlkData:  // запрос блока (подблока) текущих данных
    case CN::Write::Variant:
    case CN::Write::Mode:
    case CN::Read::BlkTech:    // чтение технологического блока
    case CN::Write::EraseTech: // команда стирания технологического блока
    case CN::Write::BlkCmd:
    case CN::Test:
    {
        WriteData.append(CN::Message::Start);
        WriteData.append(Command);
        AppendSize(WriteData, 1);
        WriteData.append(BoardType);
        WriteDataToPort(WriteData);
        break;
    }
    case CN::Read::File: // запрос файла
    {
        WriteData.append(CN::Message::Start);
        WriteData.append(Command);
        AppendSize(WriteData, 2);
        WriteData.append(static_cast<char>(FNum & 0x00FF));
        WriteData.append(static_cast<char>((FNum & 0xFF00) >> 8));
        WriteDataToPort(WriteData);
        break;
    }
    case CN::Write::Hardware:
    {
        WriteData.append(CN::Message::Start);
        WriteData.append(Command);
        int size = (BoardType == BoardTypes::BT_BSMZ) ? WHV_SIZE_TWOBOARDS : WHV_SIZE_ONEBOARD;
        AppendSize(WriteData, size); // BoardType(1), HiddenBlock(16)
        WriteData.append(BoardType);
        WriteData.append(InData);
        WriteDataToPort(WriteData);
        break;
    }
    case CN::Write::File: // запись файла
    {
        WriteData = InData;
        WRLength = WriteData.length();
        emit SetDataSize(WRLength); // сигнал для прогрессбара
        SetWRSegNum();
        WRCheckForNextSegment(true);
        break;
    }
    case CN::Write::BlkAC:
    case CN::Write::EraseCnt:
    case CN::Write::BlkTech:
    case CN::Write::BlkData:
    {
        WriteData.append(BoardType);
        WriteData.append(InData);
        WRLength = InDataSize + 1;
        emit SetDataSize(WRLength); // сигнал для прогрессбара
        SetWRSegNum();
        WRCheckForNextSegment(true);
        break;
    }
    case CN::Write::Time:
    {
        WriteData.append(CN::Message::Start);
        WriteData.append(Command);
        AppendSize(WriteData, InDataSize);
        WriteData.append(InData);
        WriteDataToPort(WriteData);
        break;
    }
    default:
    {
        ERMSG("Неизвестная команда");
        Finish(CN_UNKNOWNCMDERROR);
        return;
    }
    }
    OutData.clear();
    ReadDataChunk.clear();
    // LastBlock = false;
    bStep = 0;
    RDLength = 0;
}

void EProtocom::WriteDataToPort(QByteArray &ba)
{
    QByteArray tmpba = ba;
    if (Command == CN::Unknown) // игнорируем вызовы процедуры без команды
    {
        ERMSG("Не пришла ещё шапка файла");
        Error::ShowErMsg(USB_WRONGCOMER);
        return;
    }
    int byteswritten = 0;
    int basize = tmpba.size();
    while ((byteswritten < basize) && (!tmpba.isEmpty()))
    {
        if (isWriteUSBLog())
        {
            QByteArray tmps = "->" + tmpba.toHex() + "\n";
            CnLog->WriteRaw(tmps);
        }
        int tmpi = RawWrite(tmpba);
        if (tmpi == GENERALERROR)
        {
            ERMSG("Ошибка записи RawWrite");
            Error::ShowErMsg(COM_WRITEER);
            Disconnect();
            TTimer->start();
            return;
        }
        byteswritten += tmpi;
        emit writebytessignal(tmpba.left(tmpi));
        tmpba = tmpba.remove(0, tmpi);
    }
#ifndef NOTIMEOUT
    TTimer->start();
#endif
}

void EProtocom::Finish(int ernum)
{
    TTimer->stop();
    // предотвращение вызова newdataarrived по приходу чего-то в канале, если ничего не было послано
    Command = CN::Unknown;
    if (ernum != NOERROR)
    {
        if (ernum < 0)
        {
            CnLog->WriteRaw("### ОШИБКА В ПЕРЕДАННЫХ ДАННЫХ ###\n");
            WARNMSG("ОШИБКА В ПЕРЕДАННЫХ ДАННЫХ!!!");
        }
        else
            Error::ShowErMsg(ernum);
    }
    m_result = ernum;
    emit QueryFinished();
    //    Busy = false;
}

void EProtocom::SetWRSegNum()
{
    if (WRLength > CN::Limits::MaxSegmenthLength)
        SegLeft = (WRLength / CN::Limits::MaxSegmenthLength) + 1;
    else
        SegLeft = 1;
    SegEnd = 0;
}

void EProtocom::WRCheckForNextSegment(int first)
{
    // quint8 ForMihalich = 0;
    QByteArray tmpba;
    if (SegLeft)
    {
        --SegLeft;
        if (first)
            tmpba.append(CN::Message::Start);
        else
            tmpba.append(CN::Message::Continue);
        tmpba.append(Command);
    }
    if (SegLeft)
    {
        AppendSize(tmpba, CN::Limits::MaxSegmenthLength);
        tmpba += WriteData.mid(SegEnd, CN::Limits::MaxSegmenthLength); // -4 = '<', cmd, L, L
        SegEnd += CN::Limits::MaxSegmenthLength;
    }
    else
    {
        AppendSize(tmpba, (WriteData.size() - SegEnd));
        tmpba += WriteData.right(WriteData.size() - SegEnd);
        SegEnd = WriteData.size();
        WriteData.clear();
        // ForMihalich = 1;
    }
    emit SetDataCount(SegEnd);
    WriteDataToPort(tmpba);

    // if(ForMihalich == 1)
    // SendOk(true);
}

void EProtocom::AppendSize(QByteArray &ba, int size)
{
    char byte = static_cast<char>(size % 0x100);
    ba.append(byte);
    byte = static_cast<char>(size / 0x100);
    ba.append(byte);
}

void EProtocom::SendOk(bool cont)
{
    QByteArray tmpba;
    if (cont)
        tmpba.append(CN::Message::Continue);
    else
        tmpba.append(CN::Message::Start);
    tmpba.append(Command);
    AppendSize(tmpba, 0);
    // отправляем "ОК" и переходим к следующему сегменту
    WriteDataToPort(tmpba);
}

void EProtocom::SendErr()
{
    QByteArray tmpba;
    tmpba.append(CN::Message::Start);
    tmpba.append(CN::ResultError);
    AppendSize(tmpba, 1);
    // модулю не нужны коды ошибок
    tmpba.append(char(CN::NullByte));
    WriteDataToPort(tmpba);
}

bool EProtocom::GetLength()
{
    if (ReadDataChunk.at(1) != Command)
        return false;
    ReadDataChunkLength = static_cast<quint8>(ReadDataChunk.at(2));
    // посчитали длину посылки
    ReadDataChunkLength += static_cast<quint8>(ReadDataChunk.at(3)) * 256;
    return true;
}

// void EUsbHid::Timeout()
//{
//    Finish(USO_TIMEOUTER);
//    emit ReconnectSignal();
//}

void EProtocom::ParseIncomeData(QByteArray ba)
{
    emit readbytessignal(ba);
    if (isWriteUSBLog())
    {
        QByteArray tmps = "<-" + ba.toHex() + "\n";
        CnLog->WriteRaw(tmps);
    }
    if (Command == CN::Unknown) // игнорирование вызова процедуры, если не было послано никакой команды
    {
        ERMSG("Игнорирование вызова процедуры, если не было послано никакой команды");
        return;
    }
    ReadDataChunk.append(ba);
    qint64 rdsize = ReadDataChunk.size();
    if (rdsize < 4) // ждём, пока принятый буфер не будет хотя бы длиной 3 байта или не произойдёт таймаут
        return;
    if (ReadDataChunk.at(0) != CN::Message::Module)
    {
        ERMSG("Ошибка при приеме данных");
        Finish(CN_RCVDATAERROR);
        return;
    }
    if (ReadDataChunk.at(1) == CN::ResultError)
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
        case CN::Write::Hardware:
        case CN::Write::EraseTech:
        case CN::Write::EraseCnt:
        case CN::Write::Variant:
        case CN::Write::Mode:
        case CN::Write::Time:
        case CN::Write::BlkCmd:
        case CN::Write::Upgrade:
        case CN::Test:
        {
            if ((ReadDataChunk.at(1) != CN::ResultOk) || (ReadDataChunk.at(2) != 0x00) || (ReadDataChunk.at(3) != 0x00))
            {
                ERMSG("Ошибка при приеме данных");
                Finish(CN_RCVDATAERROR);
                return;
            }
            if (Command == CN::Write::EraseTech)
                OscTimer->start(); // start timer to send ErPg command periodically
            Finish(NOERROR);
            INFOMSG("Переход в тестовый режим без ошибок");
            return;
        }
        // команды с ответом "ОК" и с продолжением
        case CN::Write::File:
        case CN::Write::BlkAC:
        case CN::Write::BlkTech:
        case CN::Write::BlkData:
        {
            if ((ReadDataChunk.at(1) != CN::ResultOk) || (ReadDataChunk.at(2) != 0x00) || (ReadDataChunk.at(3) != 0x00))
            {
                ERMSG("Ошибка при приеме данных");
                Finish(CN_RCVDATAERROR);
                return;
            }
            if (!SegLeft)
            {
                Finish(NOERROR);
                return;
            }
            ReadDataChunk.clear();
            WRCheckForNextSegment(false);
            return;
        }
        // команды с ответом SS c L L ... и продолжением
        case CN::Read::BlkStartInfo:
        case CN::Read::BlkDataA:
        case CN::Read::BlkAC:
        case CN::Read::BlkData:
        case CN::Read::Progress:
        case CN::Read::BlkTech:
        case CN::Read::Variant:
        case CN::Read::Mode:
        case CN::Read::Time:
        {
            if (!GetLength())
            {
                ERMSG("Несовпадение длины");
                Finish(CN_RCVDATAERROR);
                return;
            }
            if (ReadDataChunkLength == 0)
            {
                rdsize = 0;
                Finish(NOERROR);
                return;
            }
            if (Command == CN::Read::Progress)
                emit SetDataSize(100);
            else
                emit SetDataSize(0); // длина неизвестна для команд с ответами без длины
            bStep++;
            break;
        }
        case CN::Read::File:
        {
            if (!GetLength())
            {
                ERMSG("Несовпадение длины");
                Finish(CN_RCVDATAERROR);
                return;
            }
            if (ReadDataChunkLength == 0)
            {
                rdsize = 0;
                Finish(NOERROR);
                return;
            }
            // надо проверить, тот ли номер файла принимаем
            if (rdsize < 20) // не пришла ещё шапка файла
            {
                ERMSG("Не пришла ещё шапка файла");
                return;
            }

            // шапка:
            // WORD fname;		// имя файла
            // WORD service;	// сервисное слово (по умолчанию 0xFF)
            // DWORD size;		// размер файла (без заголовка)
            // DWORD crc32;     // контрольная сумма
            // DWORD thetime;	// время создания файла

            quint8 tmpi = ReadDataChunk[5];
            quint16 filenum = quint16(tmpi * 256);
            tmpi = ReadDataChunk[4];
            filenum += tmpi;
            if (filenum != FNum)
            {
                Finish(USO_UNKNFILESENT);
                return;
            }
            // вытаскиваем размер файла
            memcpy(&RDLength, &(ReadDataChunk.data()[8]), sizeof(quint32));
            RDLength += 16;
            // размер файла должен быть не более 16М
            if (RDLength > CN::Limits::MaxGetFileSize)
            {
                ERMSG("Некорректная длина блока");
                Finish(CN_RCVLENGTHERROR);
                return;
            }
            emit SetDataSize(RDLength);
            //                OutData.resize(RDLength);
            bStep++;
            break;
        }

        default:
            Finish(CN_UNKNOWNCMDERROR);
            break;
        }
    }

        [[fallthrough]];
    case 1:
    {
        if (!GetLength())
        {
            Finish(CN_RCVDATAERROR);
            return;
        }
        // пока не набрали целый буфер соответственно присланной длине или не произошёл таймаут
        if (rdsize < ReadDataChunkLength)
            return;
        // убираем заголовок с < и длиной
        ReadDataChunk.remove(0, 4);
        switch (Command)
        {
        case CN::Read::BlkStartInfo:
        case CN::Read::BlkDataA:
        case CN::Read::BlkAC:
        case CN::Read::BlkData:
        case CN::Read::BlkTech:
        case CN::Read::Variant:
        case CN::Read::Mode:
        case CN::Read::Time:
        {
            // команды с чтением определённого InDataSize количества байт из устройства
            ReadDataChunk.truncate(ReadDataChunkLength);
            OutData.append(ReadDataChunk);
            int outdatasize = OutData.size();
            // сигнал для прогрессбара
            emit SetDataCount(outdatasize);
            ReadDataChunk.clear();
            if ((outdatasize >= InDataSize) || (ReadDataChunkLength < CN::Limits::MaxSegmenthLength))
            {
                // установка размера прогрессбара, чтобы не мелькал
                emit SetDataSize(outdatasize);
                Finish(NOERROR);
            }
            else
                SendOk(true);
            break;
        }
        case CN::Read::File:
        {
            // чтение файла количеством байт RDLength = (sizeof(FileHeader) + size)
            int outdatasize = OutData.size();
            int tmpi = outdatasize + ReadDataChunkLength;
            // проверка на выход за диапазон
            if (tmpi > RDLength)
                // копируем только требуемое количество байт
                ReadDataChunkLength = RDLength - outdatasize;
            ReadDataChunk.truncate(ReadDataChunkLength);
            OutData.append(ReadDataChunk);
            outdatasize += ReadDataChunkLength;
            // сигнал для прогрессбара
            emit SetDataCount(outdatasize);
            ReadDataChunk.clear();
            if (outdatasize >= RDLength)
            {
                Finish(NOERROR);
                return;
            }
            else
                SendOk(true);
            break;
        }
        case CN::Read::Progress:
        {
            quint16 OscNum = static_cast<quint8>(ReadDataChunk.at(0)) + static_cast<quint8>(ReadDataChunk.at(1)) * 256;
            emit SetDataCount(OscNum);
            if (OscNum == 100)
            {
                Finish(NOERROR);
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
    }
    }
}

void EProtocom::CheckForData()
{
    QByteArray ba = RawRead(1000);
    ParseIncomeData(ba);
}

void EProtocom::OscTimerTimeout()
{
    SendCmd(CN::Read::Progress);
}

QString EProtocom::deviceName() const
{
    return m_deviceName;
}

void EProtocom::setDeviceName(const QString &deviceName)
{
    m_deviceName = deviceName;
}

void EProtocom::SendCmd(char command, int parameter)
{
    // only for these commands
    switch (command)
    {
    case CN::Read::Progress:
    case CN::Write::Upgrade:
    case CN::Write::BlkCmd:
    case CN::Test:
    case CN::Write::Variant:
    case CN::Write::Mode:
    case CN::Write::EraseTech:
        break;
    default:
        Finish(CN_UNKNOWNCMDERROR);
        ERMSG("Неизвестная команда");
        return;
    }
    QByteArray ba;
    Send(command, parameter, ba, 0);
}

void EProtocom::SendIn(char command, char parameter, QByteArray &ba, qint64 maxdatasize)
{
    // only for these commands
    switch (command)
    {
    case CN::Read::BlkStartInfo:
    case CN::Read::BlkAC:
    case CN::Read::BlkDataA:
    case CN::Read::BlkData:
    case CN::Read::BlkTech:
    case CN::Read::Time:
    case CN::Read::Variant:
    case CN::Read::Mode:
        break;
    default:
        ERMSG("Неизвестная команда");
        Finish(CN_UNKNOWNCMDERROR);
        return;
    }
    Send(command, parameter, ba, maxdatasize);
    ba = OutData;
}

void EProtocom::SendOut(char command, char board_type, QByteArray &ba)
{
    // only for these commands
    switch (command)
    {
    case CN::Write::Hardware:
    case CN::Write::BlkAC:
    case CN::Write::Time:
    case CN::Write::BlkData:
    case CN::Write::BlkTech:
    case CN::Write::EraseCnt:
        break;
    default:
        ERMSG("Неизвестная команда");
        Finish(CN_UNKNOWNCMDERROR);
        return;
    }
    Send(command, board_type, ba, 0);
}

void EProtocom::SendFile(unsigned char command, char board_type, int filenum, QByteArray &ba)
{
    // only for these commands
    switch (command)
    {
    case CN::Read::File:
    case CN::Write::File:
        break;
    default:
        ERMSG("Неизвестная команда");
        Finish(CN_UNKNOWNCMDERROR);
        return;
    }
    FNum = filenum;
    Send(command, board_type, ba, 0);
    ba = OutData;
}

void EProtocom::TranslateDeviceAndSave(const QString &str)
{
    // формат строки: "VEN_" + QString::number(venid, 16) + "_ & DEV_" + QString::number(prodid, 16) + "_ & SN_" + sn;
    QStringList sl = str.split("_"); // 1, 3 и 5 - полезная нагрузка
    if (sl.size() < 6)
    {
        ERMSG("Неправильная длина имени порта");
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

// bool EProtocom::isConnected() const
//{
//    return m_connected;
//}

// void EProtocom::setConnected(bool isConnected)
//{
//    m_connected = isConnected;
//}

EProtocom::~EProtocom()
{
    m_workerThread.quit();
    m_workerThread.wait();
    pinstance_ = nullptr;
}
/**
 * The first time we call GetInstance we will lock the storage location
 *      and then we make sure again that the variable is null and then we
 *      set the value.
 */
EProtocom *EProtocom::GetInstance(QObject *parent)
{
    if (pinstance_ == nullptr)
    {
        QMutexLocker locker(&mutex_);
        if (pinstance_ == nullptr)
        {
            pinstance_ = new EProtocom(parent);
        }
    }
    return pinstance_;
}

bool EProtocom::Connect()
{
    QMutexLocker locker(&mutex_);
    if (Board::GetInstance()->connectionState() == Board::ConnectionState::ConnectedState
        && Board::GetInstance()->interfaceType() == Board::InterfaceType::USB)
        Disconnect();
    m_usbWorker = new EUsbWorker(UsbPort, CnLog, isWriteUSBLog());

    ///
    m_usbWorker->moveToThread(&m_workerThread);
    connect(&m_workerThread, &QThread::started, m_usbWorker, &EUsbWorker::interact);

    connect(m_usbWorker, &EUsbWorker::Finished, &m_workerThread, &QThread::quit);

    connect(&m_workerThread, &QThread::finished, &m_workerThread, &QThread::deleteLater);
    connect(m_usbWorker, &EUsbWorker::Finished, m_usbWorker, &EUsbWorker::deleteLater);

    connect(m_usbWorker, &EUsbWorker::NewDataReceived, this, &EProtocom::ParseIncomeData);

    if (m_usbWorker->setupConnection() == 0 && Board::GetInstance()->interfaceType() == Board::InterfaceType::USB)
    {
        Board::GetInstance()->setConnectionState(Board::ConnectionState::ConnectedState);
        m_workerThread.start();
    }
    else
        return false;
    return true;
}

void EProtocom::Disconnect()
{
    RawClose();
    CnLog->WriteRaw("Disconnected!\n");
    delete EProtocom::GetInstance();
}

QByteArray EProtocom::RawRead(int bytes)
{
    Q_UNUSED(bytes);
    return QByteArray();
}

int EProtocom::RawWrite(QByteArray &ba)
{
    if (Board::GetInstance()->connectionState() == Board::ConnectionState::ClosingState
        && Board::GetInstance()->interfaceType() == Board::InterfaceType::USB)
        return GENERALERROR;
    return m_usbWorker->WriteDataAttempt(ba);
}

void EProtocom::RawClose()
{
    if (Board::GetInstance()->connectionState() == Board::ConnectionState::ConnectedState
        && Board::GetInstance()->interfaceType() == Board::InterfaceType::USB)
    {
        // setConnected(false);
        Board::GetInstance()->setConnectionState(Board::ConnectionState::ConnectedState);
        m_usbWorker->Stop();
    }
}

QStringList EProtocom::DevicesFound() const
{
    struct hid_device_info *devs, *cur_dev;

    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;
    int venid, prodid;
    QString sn;
    QStringList sl;
    while (cur_dev)
    {
        if (cur_dev->vendor_id == 0xC251)
        {
            venid = cur_dev->vendor_id;
            prodid = cur_dev->product_id;
            sn = QString::fromWCharArray(cur_dev->serial_number);
            QString tmps
                = "VEN_" + QString::number(venid, 16) + "_ & DEV_" + QString::number(prodid, 16) + "_ & SN_" + sn;
            sl << tmps;
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
    return sl;
}

QThread *EProtocom::workerThread()
{
    return &m_workerThread;
}

EUsbWorker *EProtocom::usbWorker() const
{
    return m_usbWorker;
}
