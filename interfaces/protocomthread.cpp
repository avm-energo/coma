#include "protocomthread.h"

#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../gen/pch.h"
#include "../gen/stdfunc.h"
#include "defines.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QMetaEnum>
#include <QThread>
#include <QTimer>

typedef QQueue<QByteArray> ByteQueue;

quint16 extractLength(const QByteArray &ba)
{
    quint16 len = static_cast<quint8>(ba.at(2));
    len += static_cast<quint8>(ba.at(3)) * 256;
    return len;
}

void appendInt16(QByteArray &ba, quint16 size)
{
    ba.append(static_cast<char>(size % 0x100));
    ba.append(static_cast<char>(size / 0x100));
}

bool isCommandExist(int cmd)
{
    if (cmd == -1)
    {
        qCritical() << Error::WrongCommandError;
        return false;
    }
    return true;
}

// Если размер меньше MaxSegmenthLength то
// сегмент считается последним, тогда обрабатываем
// полученные данные
inline bool isOneSegment(unsigned len)
{
    using CN::Limits::MaxSegmenthLength;
    Q_ASSERT(len <= MaxSegmenthLength);
    if (len != MaxSegmenthLength)
        return true;
    return false;
}

QByteArray prepareOk(bool isStart)
{
    QByteArray tmpba;
    if (isStart)
        tmpba.append(CN::Starters::Request);
    else
        tmpba.append(CN::Starters::Continue);

    tmpba.append(CN::Commands::ResultOk);
    appendInt16(tmpba, 0);
    return tmpba;
}

QByteArray prepareError()
{
    QByteArray tmpba;
    tmpba.append(CN::Starters::Request);
    tmpba.append(CN::Commands::ResultError);
    appendInt16(tmpba, 1);
    // модулю не нужны коды ошибок
    tmpba.append(static_cast<char>(CN::NullByte));
    return tmpba;
}

QByteArray prepareBlock(CommandStruct &cmdStr, CN::Starters startByte = CN::Starters::Request)
{
    QByteArray ba;
    ba.append(startByte);
    ba.append(cmdStr.cmd);
    // TODO Проверить
    appendInt16(ba, cmdStr.ba.size() > 0 ? cmdStr.ba.size() : 1);
    ba.append(cmdStr.arg1.toUInt());
    ba.append(cmdStr.ba);
    return ba;
}

ByteQueue prepareLongBlk(CommandStruct &cmdStr)
{
    ByteQueue bq;
    using CN::Limits::MaxSegmenthLength;
    // Количество сегментов
    quint64 segCount = cmdStr.ba.size() / MaxSegmenthLength + 1;
    bq.reserve(segCount);

    CommandStruct temp { cmdStr.cmd, cmdStr.arg1, cmdStr.arg2, cmdStr.ba.left(MaxSegmenthLength) };
    bq << prepareBlock(temp);

    for (int pos = MaxSegmenthLength; pos < cmdStr.ba.size(); pos += MaxSegmenthLength)
    {
        temp = { cmdStr.cmd, cmdStr.arg1, cmdStr.arg2, cmdStr.ba.mid(pos, MaxSegmenthLength) };
        bq << prepareBlock(temp, CN::Starters::Continue);
    }
    return bq;
}

QByteArray prepareHiddenBlock(CommandStruct &cmdStr)
{
    QByteArray ba;
    ba.append(CN::Message::StartReq);
    ba.append(cmdStr.cmd);
    appendInt16(ba, cmdStr.ba.size() > 0 ? cmdStr.ba.size() : 1);
    ba.append(cmdStr.arg1.toUInt());
    ba.append(cmdStr.ba);
    return ba;
}

ProtocomThread::ProtocomThread(QObject *parent) : QObject(parent)
{
    QString tmps = "=== Log started ===";
    log = new LogClass;
    log->Init("canal.log");
    writeLog(tmps.toUtf8());
    RDLength = 0;
    SegEnd = 0;
    SegLeft = 0;
    connect(this, &ProtocomThread::readyRead, &ProtocomThread::initiateReceive);
}

Error::Msg ProtocomThread::result() const
{
    return m_result;
}

void ProtocomThread::setResult(const Error::Msg &result)
{
    m_result = result;
}

void ProtocomThread::setReadDataChunk(const QByteArray &readDataChunk)
{
    m_rwLocker.lockForWrite();
    m_buffer.second = readDataChunk;
    m_rwLocker.unlock();
}

void ProtocomThread::appendReadDataChunk(const QByteArray &readDataChunk)
{
    m_rwLocker.lockForWrite();
    m_buffer.second.append(readDataChunk);
    m_rwLocker.unlock();
    emit readyRead();
}

void ProtocomThread::parse()
{
}

void ProtocomThread::Send(char command, char parameter, QByteArray &ba, qint64 datasize)
{
    if (Board::GetInstance().connectionState() == Board::ConnectionState::Closed
        && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
    {
        qCritical() << Error::Msg::NoDeviceError;
        m_result = Error::Msg::NoDeviceError;
        return;
    }

    InData = ba;
    InDataSize = datasize; // размер области данных, в которую производить запись
    Command = command;
    BoardType = parameter; // in GBd command it is a block number
    InitiateSend();
    // m_waitTimer->start();
    // m_loop.exec(QEventLoop::ExcludeUserInputEvents);
}

void ProtocomThread::InitiateSend()
{
    using namespace CN;
    m_writeData.clear();
    switch (Command)
    {
    case CN::Read::BlkStartInfo: // запрос блока стартовой информации
    case CN::Read::Progress:     // запрос текущего прогресса
    case CN::Read::Variant:
    case CN::Read::Mode:
    case CN::Read::Time:
    case CN::Write::Upgrade:
    {
        m_writeData.append(CN::Message::StartReq);
        m_writeData.append(Command);
        appendInt16(m_writeData, 0);
        WriteDataToPort(m_writeData);
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
        m_writeData.append(CN::Message::StartReq);
        m_writeData.append(Command);
        appendInt16(m_writeData, 1);
        m_writeData.append(BoardType);
        WriteDataToPort(m_writeData);
        break;
    }
    case CN::Read::File: // запрос файла
    {
        m_writeData.append(CN::Message::StartReq);
        m_writeData.append(Command);
        appendInt16(m_writeData, 2);
        appendInt16(m_writeData, FNum);
        WriteDataToPort(m_writeData);
        break;
    }
    case CN::Write::Hardware:
    {
        m_writeData.append(CN::Message::StartReq);
        m_writeData.append(Command);
        appendInt16(m_writeData, InData.size()); // BoardType(1), HiddenBlock(16)
        m_writeData.append(BoardType);
        m_writeData.append(InData);
        WriteDataToPort(m_writeData);
        break;
    }
    case CN::Write::File: // запись файла
    {
        m_writeData = InData;
        SetWRSegNum(m_writeData.length());
        QByteArray tmpba = WRCheckForNextSegment(true);
        WriteDataToPort(tmpba);
        break;
    }
    case CN::Write::BlkAC:
    case CN::Write::EraseCnt:
    case CN::Write::BlkTech:
    case CN::Write::BlkData:
    {
        m_writeData.append(BoardType);
        m_writeData.append(InData);
        // WRLength = InDataSize + 1;
        SetWRSegNum(InDataSize + 1);
        QByteArray tmpba = WRCheckForNextSegment(true);
        WriteDataToPort(tmpba);
        break;
    }
    case CN::Write::Time:
    {
        m_writeData.append(CN::Message::StartReq);
        m_writeData.append(Command);
        appendInt16(m_writeData, InDataSize);
        m_writeData.append(InData);
        WriteDataToPort(m_writeData);
        break;
    }
    default:
    {
        Finish(Error::Msg::WrongCommandError);
        return;
    }
    }
    OutData.clear();
    // m_readDataChunk.clear();
    m_buffer.first = 0;
    m_buffer.second.clear();
    // LastBlock = false;
    bStep = 0;
    RDLength = 0;
}

void ProtocomThread::WriteDataToPort(QByteArray &ba)
{
    CN::Commands command = CN::Commands(ba.at(1));
    if (command == CN::Commands::Unknown) // игнорируем вызовы процедуры без команды
    {
        qCritical() << Error::Msg::WrongCommandError;
        return;
    }
    if (ba.isEmpty())
    {
        qCritical() << Error::Msg::SizeError;
        return;
    }

    writeLog(ba, ToDevice);

    if (Board::GetInstance().connectionState() == Board::ConnectionState::Closed
        && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
    {
        qCritical() << Error::Msg::WriteError;
        stop();
        return;
    }
    emit writeDataAttempt(ba);
}

void ProtocomThread::Finish(Error::Msg msg)
{
    // предотвращение вызова newdataarrived по приходу чего-то в канале, если
    // ничего не было послано
    Command = CN::Unknown;
    if (msg != Error::Msg::NoError)
    {
        writeLog("### ОШИБКА В ПЕРЕДАННЫХ ДАННЫХ ###");
        qWarning("ОШИБКА В ПЕРЕДАННЫХ ДАННЫХ!!!");
        qCritical() << msg;
    }
    m_result = msg;
    emit errorOccurred(msg);
}

void ProtocomThread::SetWRSegNum(quint32 WRLength)
{
    if (WRLength > CN::Limits::MaxSegmenthLength)
        SegLeft = (WRLength / CN::Limits::MaxSegmenthLength) + 1;
    else
        SegLeft = 1;
    SegEnd = 0;
}

QByteArray ProtocomThread::WRCheckForNextSegment(int first)
{

    QByteArray tmpba;
    if (SegLeft)
    {
        --SegLeft;
        if (first)
            tmpba.append(CN::Message::StartReq);
        else
            tmpba.append(CN::Message::Continue);
        tmpba.append(Command);
    }
    if (SegLeft)
    {
        appendInt16(tmpba, CN::Limits::MaxSegmenthLength);
        tmpba += m_writeData.mid(SegEnd, CN::Limits::MaxSegmenthLength); // -4 = '<', cmd, L, L
        SegEnd += CN::Limits::MaxSegmenthLength;
    }
    else
    {
        appendInt16(tmpba, (m_writeData.size() - SegEnd));
        tmpba += m_writeData.right(m_writeData.size() - SegEnd);
        SegEnd = m_writeData.size();
        m_writeData.clear();
    }
    return tmpba;
}

void ProtocomThread::ParseIncomeData(QByteArray ba)
{
    writeLog(ba, FromDevice);
    // игнорирование вызова процедуры, если не было послано никакой команды
    if (Command == CN::Unknown)
    {
        qCritical() << Error::Msg::WrongCommandError;
        return;
    }
    m_buffer.second.append(ba);
    qint64 rdsize = m_buffer.second.size();
    // ждём, пока принятый буфер не будет хотя бы длиной 3 байта или не произойдёт таймаут
    if (rdsize < 4)
        return;
    byte startByte = ba.front();
    if (m_buffer.second.at(0) != CN::Message::StartRes)
    {
        Finish(Error::Msg::ReadError);
        return;
    }
    if (static_cast<unsigned char>(m_buffer.second.at(1)) == CN::ResultError)
    {
        // некорректная посылка
        if (rdsize < 5)
            Finish(Error::Msg::ReadError);
        else
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
            //    OscTimer->start(); // start timer to send ErPg command periodically
        case CN::Write::EraseCnt:
        case CN::Write::Variant:
        case CN::Write::Mode:
        case CN::Write::Time:
        case CN::Write::BlkCmd:
        case CN::Write::Upgrade:
        case CN::Test:
        {
            if ((m_buffer.second.at(1) != CN::ResultOk) || (m_buffer.second.at(2) != 0x00)
                || (m_buffer.second.at(3) != 0x00))
            {
                Finish(Error::Msg::ReadError);
                return;
            }
            Finish(Error::Msg::NoError);
            qInfo("Переход в тестовый режим без ошибок");
            return;
        }
        // команды с ответом "ОК" и с продолжением
        case CN::Write::File:
        case CN::Write::BlkAC:
        case CN::Write::BlkTech:
        case CN::Write::BlkData:
        {
            if ((m_buffer.second.at(1) != CN::ResultOk) || (m_buffer.second.at(2) != 0x00)
                || (m_buffer.second.at(3) != 0x00))
            {
                Finish(Error::Msg::ReadError);
                return;
            }
            if (!SegLeft)
            {
                Finish(Error::Msg::NoError);
                return;
            }
            m_buffer.second.clear();
            QByteArray tmpba = WRCheckForNextSegment(false);
            WriteDataToPort(tmpba);
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
            if (!m_buffer.first)
            {
                Finish(Error::Msg::NoError);
                return;
            }

            bStep++;
            break;
        }
        case CN::Read::File:
        {
            if (!m_buffer.first)
            {
                Finish(Error::Msg::NoError);
                return;
            }
            // не пришла ещё шапка файла
            if (rdsize < 20)
            {
                qCritical() << Error::Msg::HeaderSizeError;
                return;
            }

            bStep++;
            break;
        }

        default:
            Finish(Error::Msg::WrongCommandError);
            break;
        }
    }

    case 1:
    {
        //        if (!GetLength())
        //        {
        //            Finish(Error::Msg::ReadError);
        //            return;
        //        }
        // пока не набрали целый буфер соответственно присланной длине или не
        // произошёл таймаут
        if (rdsize < m_buffer.first)
            return;
        // убираем заголовок с < и длиной
        m_buffer.second.remove(0, 4);
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
            m_buffer.second.truncate(m_buffer.first);
            OutData.append(m_buffer.second);
            quint32 outdatasize = OutData.size();
            // сигнал для прогрессбара
            m_buffer.second.clear();
            if ((outdatasize >= InDataSize) || (m_buffer.first < CN::Limits::MaxSegmenthLength))
            {
                Finish(Error::Msg::NoError);
            }
            else
            {
                QByteArray tmpba = prepareOk(false);
                WriteDataToPort(ba);
            }
            break;
        }
        case CN::Read::File:
        {
            // чтение файла количеством байт RDLength = (sizeof(FileHeader) + size)
            quint32 outdatasize = OutData.size();
            quint32 tmpi = outdatasize + m_buffer.first;
            // проверка на выход за диапазон
            if (tmpi > RDLength)
                // копируем только требуемое количество байт
                m_buffer.first = RDLength - outdatasize;
            m_buffer.second.truncate(m_buffer.first);
            OutData.append(m_buffer.second);
            outdatasize += m_buffer.first;

            m_buffer.second.clear();
            if (outdatasize >= RDLength)
            {
                Finish(Error::Msg::NoError);
                return;
            }
            else
            {
                QByteArray tmpba = prepareOk(false);
                WriteDataToPort(ba);
            }
            break;
        }
        case CN::Read::Progress:
        {
            quint16 OscNum
                = static_cast<quint8>(m_buffer.second.at(0)) + static_cast<quint8>(m_buffer.second.at(1)) * 256;
            if (OscNum == 100)
            {
                Finish(Error::Msg::NoError);
                // OscTimer->stop();
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
        Finish(Error::Msg::WrongCommandError);
        return;
    }
    FNum = filenum;
    Send(command, board_type, ba, 0);
    ba = OutData;
}

ProtocomThread::~ProtocomThread()
{
    log->deleteLater();
    // OscTimer->deleteLater();
    // m_waitTimer->deleteLater();
}

bool ProtocomThread::start(const int &devPos)
{
    // FIXME Не реализовано
    if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected
        && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
        stop();
    return true;
}

// bool ProtocomThread::Reconnect()
//{
// FIXME Сделать reconnect
//    m_usbWorker->closeConnection();
//    if (m_usbWorker->setupConnection() == Error::Msg::NoError
//        && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
//    {
//        Board::GetInstance().setConnectionState(Board::ConnectionState::Connected);
//    }
//    else
//        return false;
//    return true;
//}

void ProtocomThread::stop()
{
    // RawClose();
    writeLog("Disconnected!");
}

// QByteArray ProtocomThread::RawRead(int bytes)
//{
//    Q_UNUSED(bytes)
//    return QByteArray();
//}

// void ProtocomThread::RawClose()
//{
//    if (Board::GetInstance().connectionState() != Board::ConnectionState::Closed
//        && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
//    {
//        Board::GetInstance().setConnectionState(Board::ConnectionState::Closed);
//    }
//}

// TODO Проверить длину получаемой строки, если равна 4 то убрать проверки и сделать каст
void handleTime(const char *str)
{
    quint64 time = reinterpret_cast<quint64>(str);
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, time };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
}

void handleBitString(const QByteArray &ba)
{
    // 3c21 3c00
    // a200000087000000000003010b0002000e0000006a020000320043000451383233393736020000004323330000000004785634126070e712001
}

void handleFloat(const QByteArray &ba, quint16 addr)
{
    float blk = 0.0;
    std::copy_n(ba.constData(), sizeof(float), blk);
    DataTypes::FloatStruct resp { addr, blk };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::Float, resp);
}

void handleFile(const QByteArray &ba, quint16 addr)
{
    DataTypes::FileStruct resp { addr, ba };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::File, resp);
}

void handleInt(const byte num)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, num };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
}

void handleBool(const bool status = true, int errorSize = 0, int errorCode = 0)
{
    if (status)
    {
        DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, 0 };
        DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
    }
    else
    {
        DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Error, 0 };
        DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
        qCritical() << "Error size: " << errorSize << "Error code: " << errorCode;
    }
}

inline void handleCommand(const QByteArray &ba)
{
    qCritical("We should be here, something went wrong");
}

void ProtocomThread::handle(const CN::Commands cmd)
{
    // NOTE using enum after cpp20
    using namespace CN;
    using namespace DataTypes;
    uint addr = m_currentCommand.arg1.toUInt();
    switch (cmd)
    {
    case Commands::ResultOk:

        //  GVar MS GMode MS
        if (!m_buffer.second.isEmpty())
            handleInt(m_buffer.second.front());
        else
            handleBool();
        return;

    case Commands::ResultError:

        handleBool(false, m_buffer.first, m_buffer.second.front());
        return;

    case Commands::ReadTime:

        handleTime(m_buffer.second.constData());
        return;

    case Commands::ReadBlkStartInfo:

        handleBitString(m_buffer.second);
        return;

    case Commands::ReadBlkAC:

        handleFloat(m_buffer.second, addr);
        return;

    case Commands::ReadBlkDataA:

        handleFloat(m_buffer.second, addr);
        return;

    case Commands::ReadBlkData:

        handleFloat(m_buffer.second, addr);
        return;

    case Commands::ReadBlkTech:

        handleFloat(m_buffer.second, addr);
        return;

    case Commands::ReadProgress:

        handleBitString(m_buffer.second);
        return;

    case Commands::ReadFile:

        handleFile(m_buffer.second, addr);
        return;

    default:

        handleCommand(m_buffer.second);
        return;
    }
}

void ProtocomThread::checkQueue()
{
    CommandStruct inp;
    if (DataManager::deQueue(inp) == Error::Msg::NoError)
    {
        switch (inp.cmd)
        {
        default:
            m_currentCommand = inp;
            initiateSend(inp);
            break;
        }
    }
}

void ProtocomThread::initiateSend(const CommandStruct &cmdStr)
{
    using namespace CN;
    QByteArray ba;
    switch (m_currentCommand.cmd)
    {
    case Commands::WriteUpgrade:
    {
        ba.append(Message::StartReq);
        ba.append(static_cast<char>((m_currentCommand.cmd)));
        appendInt16(ba, 0);
        break;
    }
    case Commands::WriteVariant:
    {
        ba = prepareBlock(m_currentCommand);
        break;
    }
    case Commands::WriteMode:
    case Commands::EraseTech: // команда стирания технологического блока
    case Commands::WriteBlkCmd:
    case Commands::Test:
    {
        //        ba.append(CN::Message::Start);
        //        ba.append(static_cast<char>((m_currentCommand.cmd)));
        //        appendSize(ba, 1);
        ba = prepareBlock(m_currentCommand);
        // TODO Должно сработать т.к. ba.size будет равен 0
        break;
    }
    case Commands::WriteHardware:
    {
        ba = prepareBlock(m_currentCommand);
        break;
    }
    case Commands::WriteBlkAC:
    {
        ba = prepareBlock(m_currentCommand);
        break;
    }
    case Commands::EraseCnt:
    {
        ba = prepareBlock(m_currentCommand);
        break;
    }
    case Commands::WriteBlkTech:
    {
        ba = prepareBlock(m_currentCommand);
        break;
    }
    case Commands::WriteBlkData:
    {
        ba = prepareBlock(m_currentCommand);
        break;
    }
    default:
    {
        qCritical() << QVariant::fromValue(Error::Msg::WrongCommandError).toString();
        return;
    }
    }
    emit writeDataAttempt(ba);
}

void ProtocomThread::initiateReceive(QByteArray ba)
{
    using namespace CN;

    QByteArray tmps = "<-" + ba.toHex() + "\n";
    log->WriteRaw(tmps);
    // Нет шапки
    if (ba.size() < 4)
    {
        qCritical() << Error::HeaderSizeError;
        return;
    }
    byte cmd = ba.at(1);
    int cmdCode = QMetaEnum::fromType<Commands>().value(cmd);
    if (!isCommandExist(cmdCode))
        return;

    quint16 size;
    std::copy(&ba.constData()[2], &ba.constData()[3], &size);
    switch (ba.front())
    {
    //    case Response:
    //    {

    //        m_buffer.first = size;
    //        m_buffer.second.setRawData(&ba.constData()[4], size);
    //        handle(Commands(cmd));
    //    }
    //    case Continue:
    //    {

    //        ba.remove(0, 4);
    //        m_buffer.first += size;
    //        m_buffer.second.append(ba);
    //        // Если размер меньше MaxSegmenthLength то
    //        // сегмент считается последним, тогда обрабатываем
    //        // полученные данные
    //        if (size != MaxSegmenthLength)
    //            handle(Commands(cmd));
    //    }
    case Response:
    {
        ba.remove(0, 4);
        m_buffer.first += size;
        m_buffer.second.append(ba);

        if (isOneSegment(size))
            handle(Commands(cmd));
        else
            prepareOk(false);
        break;
    }
    default:
        qCritical() << Error::WrongCommandError;
        break;
    }
}

void ProtocomThread::writeLog(QByteArray ba, Direction dir)
{
#ifdef CANALUSB_LOG
    QByteArray tmpba = QByteArray(metaObject()->className());
    switch (dir)
    {
    case FromDevice:
        tmpba.append(": <-");
    case ToDevice:
        tmpba.append(": ->");
    default:
        tmpba.append(":  ");
    }
    tmpba.append(ba).append("\n");
    log->WriteRaw(tmpba);
#endif
}
