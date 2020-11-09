#include "protocomthread.h"

#include "../gen/board.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../gen/pch.h"
#include "../gen/stdfunc.h"
#include "defines.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QSettings>
#include <QThread>
#include <QTimer>

//#define MAX_STR 255
#ifdef _WIN32
// clang-format off
#include <windows.h>
// Header dbt must be the last header, thanx to microsoft
#include <dbt.h>
// clang-format on
#endif
bool ProtocomThread::m_writeUSBLog;

ProtocomThread::ProtocomThread(QWidget *parent)
{
    QString tmps = "=== CLog started ===\n";
    Log = new LogClass;
    Log->Init("canal.log");
    Log->WriteRaw(tmps.toUtf8());
    RDLength = 0;
    SegEnd = 0;
    SegLeft = 0;
    OscNum = 0;
    OscTimer = new QTimer;
    OscTimer->setInterval(CN::TimeoutOscillogram);
    OscTimer->setSingleShot(false);
    m_waitTimer = new QTimer;
    m_waitTimer->setInterval(CN::Timeout);
    connect(OscTimer, &QTimer::timeout, this, &ProtocomThread::OscTimerTimeout);
    connect(m_waitTimer, &QTimer::timeout, &m_loop, &QEventLoop::quit);
    connect(this, &ProtocomThread::QueryFinished, &m_loop, &QEventLoop::quit);
    // QSharedPointer<QSettings> sets = QSharedPointer<QSettings>(new QSettings("EvelSoft", PROGNAME));
    // bool writeUSBLog = sets->value("WriteLog", "0").toBool();
    setWriteUSBLog(true);
}

Error::Msg ProtocomThread::result() const
{
    return m_result;
}

void ProtocomThread::setResult(const Error::Msg &result)
{
    m_result = result;
}

bool ProtocomThread::isWriteUSBLog()
{
    return m_writeUSBLog;
}

void ProtocomThread::setWriteUSBLog(bool writeUSBLog)
{
    m_writeUSBLog = writeUSBLog;
}

void ProtocomThread::Send(char command, char parameter, QByteArray &ba, qint64 datasize)
{
    if (Board::GetInstance().connectionState() == Board::ConnectionState::Closed
        && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
    {
        qDebug() << Error::Msg::NoDeviceError;
        m_result = Error::Msg::NoDeviceError;
        return;
    }

    InData = ba;
    InDataSize = datasize; // размер области данных, в которую производить запись
    Command = command;
    BoardType = parameter; // in GBd command it is a block number
    InitiateSend();
    m_waitTimer->start();
    m_loop.exec(QEventLoop::ExcludeUserInputEvents);
}

void ProtocomThread::InitiateSend()
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
        Finish(Error::Msg::WrongCommandError);
        return;
    }
    }
    OutData.clear();
    ReadDataChunk.clear();
    // LastBlock = false;
    bStep = 0;
    RDLength = 0;
}

void ProtocomThread::WriteDataToPort(QByteArray &ba)
{
    QByteArray tmpba = ba;
    if (Command == CN::Unknown) // игнорируем вызовы процедуры без команды
    {
        ERMSG("Не пришла ещё шапка файла");
        qCritical() << QVariant::fromValue(Error::Msg::WrongCommandError).toString();
        return;
    }
    int byteswritten = 0;
    int basize = tmpba.size();
    while ((byteswritten < basize) && (!tmpba.isEmpty()))
    {
        if (isWriteUSBLog())
        {
            QByteArray tmps = "->" + tmpba.toHex() + "\n";
            Log->WriteRaw(tmps);
        }
        if (Board::GetInstance().connectionState() == Board::ConnectionState::Closed
            && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
        {
            ERMSG("Ошибка записи RawWrite");
            qCritical() << QVariant::fromValue(Error::Msg::WriteError).toString();
            Disconnect();
            return;
        }
        int tmpi = RawWrite(tmpba);
        byteswritten += tmpi;
        tmpba.remove(0, tmpi);
    }
}

void ProtocomThread::Finish(Error::Msg msg)
{
    // предотвращение вызова newdataarrived по приходу чего-то в канале, если
    // ничего не было послано
    Command = CN::Unknown;
    if (msg != Error::Msg::NoError)
    {

        qDebug(__PRETTY_FUNCTION__);
        Log->WriteRaw("### ОШИБКА В ПЕРЕДАННЫХ ДАННЫХ ###\n");
        qWarning("ОШИБКА В ПЕРЕДАННЫХ ДАННЫХ!!!");
        qCritical() << QVariant::fromValue(msg).toString();
    }
    m_result = msg;
    emit QueryFinished();
}

void ProtocomThread::SetWRSegNum()
{
    if (WRLength > CN::Limits::MaxSegmenthLength)
        SegLeft = (WRLength / CN::Limits::MaxSegmenthLength) + 1;
    else
        SegLeft = 1;
    SegEnd = 0;
}

void ProtocomThread::WRCheckForNextSegment(int first)
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
    }
    WriteDataToPort(tmpba);
}

void ProtocomThread::AppendSize(QByteArray &ba, int size)
{
    char byte = static_cast<char>(size % 0x100);
    ba.append(byte);
    byte = static_cast<char>(size / 0x100);
    ba.append(byte);
}

void ProtocomThread::SendOk(bool cont)
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

void ProtocomThread::SendErr()
{
    QByteArray tmpba;
    tmpba.append(CN::Message::Start);
    tmpba.append(CN::ResultError);
    AppendSize(tmpba, 1);
    // модулю не нужны коды ошибок
    tmpba.append(char(CN::NullByte));
    WriteDataToPort(tmpba);
}

bool ProtocomThread::GetLength()
{
    if (ReadDataChunk.at(1) != Command)
        return false;
    ReadDataChunkLength = static_cast<quint8>(ReadDataChunk.at(2));
    // посчитали длину посылки
    ReadDataChunkLength += static_cast<quint8>(ReadDataChunk.at(3)) * 256;
    return true;
}

void ProtocomThread::ParseIncomeData(QByteArray ba)
{
    if (isWriteUSBLog())
    {
        QByteArray tmps = "<-" + ba.toHex() + "\n";
        Log->WriteRaw(tmps);
    }
    if (Command == CN::Unknown) // игнорирование вызова процедуры, если не было
                                // послано никакой команды
    {
        ERMSG("Игнорирование вызова процедуры, если не было послано никакой команды");
        return;
    }
    ReadDataChunk.append(ba);
    qint64 rdsize = ReadDataChunk.size();
    if (rdsize < 4) // ждём, пока принятый буфер не будет хотя бы длиной 3 байта
                    // или не произойдёт таймаут
        return;
    if (ReadDataChunk.at(0) != CN::Message::Module)
    {
        ERMSG("Ошибка при приеме данных");
        Finish(Error::Msg::ReadError);
        return;
    }
    if (static_cast<unsigned char>(ReadDataChunk.at(1)) == CN::ResultError)
    {
        if (rdsize < 5) // некорректная посылка
            Finish(Error::Msg::ReadError);
        else
            // Finish(Error::Msg::USO_NOERR + ReadDataChunk.at(4));
            Finish(Error::Msg::NoError);
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
                Finish(Error::Msg::ReadError);
                return;
            }
            if (Command == CN::Write::EraseTech)
                OscTimer->start(); // start timer to send ErPg command periodically
            Finish(Error::Msg::NoError);
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
                Finish(Error::Msg::ReadError);
                return;
            }
            if (!SegLeft)
            {
                Finish(Error::Msg::NoError);
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
                Finish(Error::Msg::ReadError);
                return;
            }
            if (ReadDataChunkLength == 0)
            {
                Finish(Error::Msg::NoError);
                return;
            }

            bStep++;
            break;
        }
        case CN::Read::File:
        {
            if (!GetLength())
            {
                ERMSG("Несовпадение длины");
                Finish(Error::Msg::ReadError);
                return;
            }
            if (ReadDataChunkLength == 0)
            {
                Finish(Error::Msg::NoError);
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
                Finish(Error::Msg::WrongFileError);
                return;
            }
            // вытаскиваем размер файла
            memcpy(&RDLength, &(ReadDataChunk.data()[8]), sizeof(quint32));
            RDLength += 16;
            // размер файла должен быть не более 16М
            if (RDLength > CN::Limits::MaxGetFileSize)
            {
                ERMSG("Некорректная длина блока");
                Finish(Error::Msg::SizeError);
                return;
            }
            //                OutData.resize(RDLength);
            bStep++;
            break;
        }

        default:
            Finish(Error::Msg::WrongCommandError);
            break;
        }
    }

        [[fallthrough]];
    case 1:
    {
        if (!GetLength())
        {
            Finish(Error::Msg::ReadError);
            return;
        }
        // пока не набрали целый буфер соответственно присланной длине или не
        // произошёл таймаут
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
            // команды с чтением определённого InDataSize количества байт из
            // устройства
            ReadDataChunk.truncate(ReadDataChunkLength);
            OutData.append(ReadDataChunk);
            quint32 outdatasize = OutData.size();
            // сигнал для прогрессбара
            ReadDataChunk.clear();
            if ((outdatasize >= InDataSize) || (ReadDataChunkLength < CN::Limits::MaxSegmenthLength))
            {

                Finish(Error::Msg::NoError);
            }
            else
                SendOk(true);
            break;
        }
        case CN::Read::File:
        {
            // чтение файла количеством байт RDLength = (sizeof(FileHeader) + size)
            quint32 outdatasize = OutData.size();
            quint32 tmpi = outdatasize + ReadDataChunkLength;
            // проверка на выход за диапазон
            if (tmpi > RDLength)
                // копируем только требуемое количество байт
                ReadDataChunkLength = RDLength - outdatasize;
            ReadDataChunk.truncate(ReadDataChunkLength);
            OutData.append(ReadDataChunk);
            outdatasize += ReadDataChunkLength;

            ReadDataChunk.clear();
            if (outdatasize >= RDLength)
            {
                Finish(Error::Msg::NoError);
                return;
            }
            else
                SendOk(true);
            break;
        }
        case CN::Read::Progress:
        {
            quint16 OscNum = static_cast<quint8>(ReadDataChunk.at(0)) + static_cast<quint8>(ReadDataChunk.at(1)) * 256;
            if (OscNum == 100)
            {
                Finish(Error::Msg::NoError);
                OscTimer->stop();
                break;
            }
            break;
        }

        default:
        {
            Finish(Error::Msg::WrongCommandError);
            break;
        }
        }
    }
    }
}

void ProtocomThread::CheckForData()
{
    QByteArray ba = RawRead(1000);
    ParseIncomeData(ba);
}

void ProtocomThread::OscTimerTimeout()
{
    SendCmd(CN::Read::Progress);
}

void ProtocomThread::SendCmd(unsigned char command, int parameter)
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
        Finish(Error::Msg::WrongCommandError);
        ERMSG("Неизвестная команда");
        return;
    }
    QByteArray ba;
    Send(command, parameter, ba, 0);
}

void ProtocomThread::SendIn(unsigned char command, char parameter, QByteArray &ba, qint64 maxdatasize)
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
        Finish(Error::Msg::WrongCommandError);
        return;
    }
    Send(command, parameter, ba, maxdatasize);
    ba = OutData;
}

void ProtocomThread::SendOut(unsigned char command, char board_type, QByteArray &ba)
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
        Finish(Error::Msg::WrongCommandError);
        return;
    }
    Send(command, board_type, ba, 0);
}

void ProtocomThread::SendFile(unsigned char command, char board_type, int filenum, QByteArray &ba)
{
    // only for these commands
    switch (command)
    {
    case CN::Read::File:
    case CN::Write::File:
        break;
    default:
        ERMSG("Неизвестная команда");
        Finish(Error::Msg::WrongCommandError);
        return;
    }
    FNum = filenum;
    Send(command, board_type, ba, 0);
    ba = OutData;
}

void ProtocomThread::usbStateChanged(void *message)
{
#ifdef _WIN32
    MSG *msg = static_cast<MSG *>(message);
    int msgType = msg->message;
    if (msgType == WM_DEVICECHANGE)
    {
        msgType = msg->wParam;
        switch (msgType)
        {
        case DBT_CONFIGCHANGECANCELED:
            qDebug("DBT_CONFIGCHANGECANCELED");
            break;
        case DBT_CONFIGCHANGED:
            qDebug("DBT_CONFIGCHANGED");
            break;
        case DBT_CUSTOMEVENT:
            qDebug("DBT_CUSTOMEVENT");
            break;
        case DBT_DEVICEARRIVAL:
        {
            DevicesFound();
            if (Board::GetInstance().connectionState() == Board::ConnectionState::AboutToFinish)
            {
                if (m_devices.contains(m_usbWorker->deviceInfo()))
                {
                    m_devicePosition = m_devices.indexOf(m_usbWorker->deviceInfo());
                    m_usbWorker->setDeviceInfo(m_devices.at(m_devicePosition));
                    qDebug("Device arrived again");
                    if (!Reconnect())
                    {
                        qDebug("Reconnection failed");
                        Disconnect();
                    }
                }
            }
            break;
        }
        case DBT_DEVICEQUERYREMOVE:
            qDebug("DBT_DEVICEQUERYREMOVE");
            break;
        case DBT_DEVICEQUERYREMOVEFAILED:
            qDebug("DBT_DEVICEQUERYREMOVEFAILED");
            break;
        case DBT_DEVICEREMOVEPENDING:
            qDebug("DBT_DEVICEREMOVEPENDING");
            break;
        case DBT_DEVICEREMOVECOMPLETE:
        {
            qDebug("DBT_DEVICEREMOVECOMPLETE");
            qDebug() << DevicesFound();
            if (Board::GetInstance().connectionState() != Board::ConnectionState::Closed)
            {
                if (!m_devices.contains(m_usbWorker->deviceInfo()))
                {
                    qDebug() << "Device " << m_usbWorker->deviceInfo().serial << " removed completely";
                    WriteData.clear();
                    OutData.clear();
                    Finish(Error::Msg::NullDataError);
                    m_loop.exit();
                    QMessageBox::critical(nullptr, "Ошибка", "Связь с прибором была разорвана", QMessageBox::Ok);
                }
            }
            break;
        }
        case DBT_DEVICETYPESPECIFIC:
            qDebug("DBT_DEVICETYPESPECIFIC");
            break;
        case DBT_QUERYCHANGECONFIG:
            qDebug("DBT_QUERYCHANGECONFIG");
            break;
        case DBT_DEVNODES_CHANGED:
        {
            qDebug("DBT_DEVNODES_CHANGED");
            DevicesFound();

            // Ивенты должны происходить только если отключен подключенный раннее
            // прибор
            if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected)
            {
                if (!m_devices.contains(m_usbWorker->deviceInfo()))
                {
                    qDebug() << "Device " << m_usbWorker->deviceInfo().serial << " state changed";
                    Board::GetInstance().setConnectionState(Board::ConnectionState::AboutToFinish);
                }
            }
            break;
        }
        case DBT_USERDEFINED:
            qDebug("DBT_USERDEFINED");
            break;
        default:
            qDebug() << "Default";
            break;
        }
    }
#endif
}

ProtocomThread::~ProtocomThread()
{

    Log->deleteLater();
    OscTimer->deleteLater();
    m_waitTimer->deleteLater();
}

bool ProtocomThread::start(const int &devPos)
{
    if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected
        && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
        Disconnect();
    m_usbWorker = new UsbHidPort(m_devices.at(devPos), Log, isWriteUSBLog());
    m_devicePosition = devPos;
    QThread *workerThread = new QThread;
    m_usbWorker->moveToThread(workerThread);

    connect(workerThread, &QThread::started, m_usbWorker, &UsbHidPort::poll);
    connect(workerThread, &QThread::started, [this] { m_workerStatus = true; });
    connect(workerThread, &QThread::finished, [this] { m_workerStatus = false; });
    connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);
    connect(m_usbWorker, &UsbHidPort::finished, workerThread, &QThread::quit);
    connect(m_usbWorker, &UsbHidPort::finished, m_usbWorker, &UsbHidPort::deleteLater);
    connect(m_usbWorker, &UsbHidPort::NewDataReceived, this, &ProtocomThread::ParseIncomeData);

    //    QThread *parserThread = new QThread;

    //    connect(parserThread, &QThread::started, [this] { m_parserStatus = true; });
    //    connect(parserThread, &QThread::finished, [this] { m_parserStatus = false; });
    //    connect(parserThread, &QThread::finished, parserThread, &QThread::deleteLater);

    if (m_usbWorker->setupConnection() == Error::Msg::NoError
        && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
    {
        Board::GetInstance().setConnectionState(Board::ConnectionState::Connected);
        workerThread->start();
    }
    else
        return false;
    return true;
}

bool ProtocomThread::Reconnect()
{
    m_usbWorker->closeConnection();
    if (m_usbWorker->setupConnection() == Error::Msg::NoError
        && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
    {
        Board::GetInstance().setConnectionState(Board::ConnectionState::Connected);
    }
    else
        return false;
    return true;
}

void ProtocomThread::Disconnect()
{
    qDebug(__PRETTY_FUNCTION__);
    RawClose();
    Log->WriteRaw("Disconnected!\n");
}

QByteArray ProtocomThread::RawRead(int bytes)
{
    Q_UNUSED(bytes)
    return QByteArray();
}

int ProtocomThread::RawWrite(QByteArray &ba)
{
    return m_usbWorker->WriteDataAttempt(ba);
}

void ProtocomThread::RawClose()
{
    if (Board::GetInstance().connectionState() != Board::ConnectionState::Closed
        && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
    {
        Board::GetInstance().setConnectionState(Board::ConnectionState::Closed);
    }
}

QList<QStringList> ProtocomThread::DevicesFound()
{
    hid_device_info *devs, *cur_dev;

    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;
    QList<QStringList> sl;
    m_devices.clear();
    while (cur_dev)
    {

        if (cur_dev->vendor_id == 0xC251)
        {
            const DeviceConnectStruct buffer(cur_dev->vendor_id, cur_dev->product_id,
                QString::fromWCharArray(cur_dev->serial_number), QString(cur_dev->path));

            /*#ifdef __linux__
                        wstr[0] = 0x0000;
                        res = hid_get_serial_number_string(handle, wstr, MAX_STR);
                        if (res < 0)
                            printf("Unable to read serial number string\n");
            #endif
            #ifdef _WIN32

             sn = QString::fromWCharArray(cur_dev->serial_number);
            #endif*/
            QStringList tmps { QString::number(buffer.vendor_id, 16), QString::number(buffer.product_id, 16),
                buffer.serial, buffer.path };
            sl.append(tmps);
            m_devices.push_back(buffer);
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
    return sl;
}

UsbHidPort *ProtocomThread::usbWorker() const
{
    return m_usbWorker;
}
