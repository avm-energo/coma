#include "protocomthread.h"

#include "../gen/datamanager.h"
#include "../gen/logclass.h"
#include "../gen/pch.h"

#include <QDebug>
#include <QMetaEnum>

typedef QQueue<QByteArray> ByteQueue;

quint16 extractLength(const QByteArray &ba);
void appendInt16(QByteArray &ba, quint16 size);

bool isCommandExist(int cmd);
// Если размер меньше MaxSegmenthLength то сегмент считается последним (единственным)
inline bool isOneSegment(unsigned len);

// TODO вынести в отдельный класс как static методы?
QByteArray prepareOk(bool isStart);
QByteArray prepareError();
QByteArray prepareBlock(CommandStruct &cmdStr, CN::Starters startByte = CN::Starters::Request);
ByteQueue prepareLongBlk(CommandStruct &cmdStr);

void handleTime(const QByteArray &ba);
void handleBitString(const QByteArray &ba);
void handleFloat(const QByteArray &ba, quint16 addr);
void handleFile(const QByteArray &ba, quint16 addr);
void handleInt(const byte num);
void handleBool(const bool status = true, int errorSize = 0, int errorCode = 0);
inline void handleCommand(const QByteArray &ba);

ProtocomThread::ProtocomThread(QObject *parent) : QObject(parent)
{
    QString tmps = "=== Log started ===";
    log = new LogClass;
    log->Init("canal.log");
    writeLog(tmps.toUtf8());
}

void ProtocomThread::setReadDataChunk(const QByteArray &readDataChunk)
{
    m_rwLocker.lockForWrite();
    m_readData = readDataChunk;
    m_rwLocker.unlock();
}

void ProtocomThread::appendReadDataChunk(const QByteArray &readDataChunk)
{
    m_rwLocker.lockForWrite();
    m_readData.append(readDataChunk);
    m_rwLocker.unlock();
    emit readyRead();
}

void ProtocomThread::parse()
{
    if (!m_readData.isEmpty())
    {
        m_rwLocker.lockForWrite();
        parseResponse(m_readData);
        m_readData.clear();
        m_rwLocker.unlock();
    }
    checkQueue();
}

void ProtocomThread::finish(Error::Msg msg)
{
    if (msg != Error::Msg::NoError)
    {
        writeLog("### ОШИБКА В ПЕРЕДАННЫХ ДАННЫХ ###");
        qWarning("ОШИБКА В ПЕРЕДАННЫХ ДАННЫХ!!!");
        qCritical() << msg;
        emit errorOccurred(msg);
    }
}

ProtocomThread::~ProtocomThread()
{
    log->deleteLater();
    // OscTimer->deleteLater();
    // m_waitTimer->deleteLater();
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

void ProtocomThread::handle(const CN::Commands cmd)
{
    using namespace CN;
    using namespace DataTypes;
    uint addr = m_currentCommand.arg1.toUInt();
    switch (cmd)
    {
    case Commands::ResultOk:

        // Ignore replies to splitted packet
        if (!isOneSegment(m_currentCommand.ba.size()))
            return;
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

        handleTime(m_buffer.second);
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
            parseRequest(inp);
            break;
        }
    }
}

void ProtocomThread::parseRequest(const CommandStruct &cmdStr)
{
    // Предполагается не хранить текущую команду
    Q_UNUSED(cmdStr)
    using namespace CN;
    QByteArray ba;
    switch (m_currentCommand.cmd)
    {
    default:
    {
        if (!isOneSegment(m_currentCommand.ba.size()))
        {
            auto query = prepareLongBlk(m_currentCommand);
            while (!query.isEmpty())
                emit writeDataAttempt(query.dequeue());
        }
        else
            ba = prepareBlock(m_currentCommand);
        break;
    }
    }
    emit writeDataAttempt(ba);
}

void ProtocomThread::parseResponse(QByteArray ba)
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

    case Response:
    {
        ba.remove(0, 4);
        Q_ASSERT(size != ba.size());
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

bool isOneSegment(unsigned len)
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

QByteArray prepareBlock(CommandStruct &cmdStr, CN::Starters startByte)
{
    QByteArray ba;
    ba.append(startByte);
    ba.append(cmdStr.cmd);
    appendInt16(ba, cmdStr.ba.size());
    if (!cmdStr.arg1.isNull())
    {
        // Номер файла 2 байта, в остальных случаях аргумент 1 байт
        if (cmdStr.cmd != CN::Commands::ReadFile)
            ba.append(cmdStr.arg1.toChar());
        else
            ba.append(cmdStr.arg1.toUInt());
    }
    if (!cmdStr.ba.isEmpty())
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

void handleTime(const QByteArray &ba)
{
    quint64 time = ba.toULongLong();
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
    float blk = ba.toFloat();
    // std::copy_n(ba.constData(), sizeof(float), blk);
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

void handleBool(const bool status, int errorSize, int errorCode)
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

void handleCommand(const QByteArray &ba)
{
    qCritical("We should be here, something went wrong");
}
