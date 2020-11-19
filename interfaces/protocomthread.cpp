#include "protocomthread.h"

#include "../gen/datamanager.h"
#include "../gen/logclass.h"
#include "../gen/pch.h"

#include <QDebug>
#include <QMetaEnum>
#include <QThread>
#include <array>

typedef QQueue<QByteArray> ByteQueue;
using Proto::CommandStruct;
using Proto::Direction;
using Proto::Starters;
quint16 extractLength(const QByteArray &ba);
void appendInt16(QByteArray &ba, quint16 size);

bool isCommandExist(int cmd);
// Если размер меньше MaxSegmenthLength то сегмент считается последним (единственным)
inline bool isOneSegment(unsigned len);

// TODO вынести в отдельный класс как static методы?
QByteArray prepareOk(bool isStart, byte cmd);
QByteArray prepareError();
QByteArray prepareBlock(CommandStruct &cmdStr, Starters startByte = Starters::Request, quint16 size = 0);
ByteQueue prepareLongBlk(CommandStruct &cmdStr);

void handleBitString(const QByteArray &ba, quint16 sigAddr);
void handleBitStringArray(const QByteArray &ba, QList<quint16> arr_addr);
void handleFloat(const QByteArray &ba, quint32 sigAddr);
void handleFloatArray(const QByteArray &ba, quint32 sigAddr, quint32 sigCount);
void handleFile(QByteArray &ba, quint16 addr, bool isShouldRestored);
void handleInt(const byte num);
void handleBool(const bool status = true, int errorSize = 0, int errorCode = 0);
inline void handleCommand(const QByteArray &ba);

ProtocomThread::ProtocomThread(QObject *parent) : QObject(parent)
{
    // QString tmps = "=== Log started ===";
    // log = new LogClass;
    // log->Init("canal.log");
    // writeLog(tmps.toUtf8());
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

void ProtocomThread::wakeUp()
{
    _waiter.wakeAll();
}

void ProtocomThread::parse()
{
    forever
    {
        m_rwLocker.lockForWrite();
        //_waiter.wait(&m_rwLocker);
        if (!m_readData.isEmpty())
        {

            parseResponse(m_readData);
            m_readData.clear();
        }
        checkQueue();
        m_rwLocker.unlock();
    }
}

void ProtocomThread::finish(Error::Msg msg)
{
    if (msg != Error::Msg::NoError)
    {
        // writeLog("### ОШИБКА В ПЕРЕДАННЫХ ДАННЫХ ###");
        qWarning("ОШИБКА В ПЕРЕДАННЫХ ДАННЫХ!!!");
        qCritical() << msg;
        emit errorOccurred(msg);
    }
}

ProtocomThread::~ProtocomThread()
{
    // log->deleteLater();
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

void ProtocomThread::handle(const Proto::Commands cmd)
{
    using namespace Proto;
    using namespace DataTypes;
    quint32 addr = m_currentCommand.arg1.toUInt();
    quint32 count = m_currentCommand.arg2.toUInt();
    switch (cmd)
    {
    case Commands::ResultOk:

        // Ignore replies to splitted packet
        // Не прибавляем никаких 1 или 2, надо будет проверить
        if (!isOneSegment(m_currentCommand.ba.size()))
            return;
        //  GVar MS GMode MS
        if (!m_buffer.second.isEmpty())
            handleInt(m_buffer.second.front());
        else
            handleBool();
        break;

    case Commands::ResultError:

        handleBool(false, m_buffer.first, m_buffer.second.front());
        break;

    case Commands::ReadTime:

        handleBitString(m_buffer.second, addr);
        break;

    case Commands::ReadBlkStartInfo:

        handleBitStringArray(m_buffer.second, bsiReg);
        break;

    case Commands::ReadBlkAC:

        handleFloatArray(m_buffer.second, addr, count);
        break;

    case Commands::ReadBlkDataA:

        handleFloatArray(m_buffer.second, addr, count);
        break;

    case Commands::ReadBlkData:

        handleFloatArray(m_buffer.second, addr, count);
        break;

    case Commands::ReadBlkTech:

        handleFloatArray(m_buffer.second, addr, count);
        break;

    case Commands::ReadProgress:

        handleBitString(m_buffer.second, addr);
        break;

    case Commands::ReadFile:

        handleFile(m_buffer.second, addr, static_cast<bool>(count));
        break;

    default:

        handleCommand(m_buffer.second);
        break;
    }
    m_buffer.first = 0;
    m_buffer.second.clear();
}

void ProtocomThread::checkQueue()
{
    CommandStruct inp;
    if (DataManager::deQueue(inp) != Error::Msg::NoError)
        return;
    switch (inp.cmd)
    {
    default:
        m_currentCommand = inp;
        parseRequest(inp);
        break;
    }
}

void ProtocomThread::parseRequest(const CommandStruct &cmdStr)
{
    // Предполагается не хранить текущую команду
    Q_UNUSED(cmdStr)
    qDebug() << QThread::currentThreadId();
    using namespace Proto;

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
        {
            QByteArray ba = prepareBlock(m_currentCommand);
            emit writeDataAttempt(ba);
        }
        break;
    }
    }
}

void ProtocomThread::parseResponse(QByteArray ba)
{
    using namespace Proto;

    // QByteArray tmps = "<-" + ba.toHex() + "\n";
    // log->WriteRaw(tmps);
    // Нет шапки
    if (ba.size() < 4)
    {
        qCritical() << Error::HeaderSizeError << ba.toHex();
        return;
    }
    byte cmd = ba.at(1);
    // BUG Не работает проверка на существование команды
    // int cmdCode = QMetaEnum::fromType<Commands>().value(cmd);
    // if (!isCommandExist(cmdCode))
    //    return;

    quint16 size;
    std::copy(&ba.constData()[2], &ba.constData()[3], &size);
    switch (ba.front())
    {

    case Response:
    {
        ba.remove(0, 4);
        // TODO Проверять размер
        ba.resize(size);
        // Q_ASSERT(size == ba.size());
        m_buffer.first += size;
        m_buffer.second.append(ba);

        if (isOneSegment(size))
            handle(Commands(cmd));
        else
        {
            // Потому что на эту команду модуль не отдает пустой ответ
            if (cmd != ReadBlkStartInfo)
            {
                auto tba = prepareOk(false, cmd);
                Q_ASSERT(tba.size() == 4);
                emit writeDataAttempt(tba);
            }
        }
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
    case Proto::FromDevice:
        tmpba.append(": <-");
    case Proto::ToDevice:
        tmpba.append(": ->");
    default:
        tmpba.append(":  ");
    }
    tmpba.append(ba).append("\n");
    // log->WriteRaw(tmpba);
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
    using Proto::Limits::MaxSegmenthLength;
    Q_ASSERT(len <= MaxSegmenthLength);
    if (len != MaxSegmenthLength)
        return true;
    return false;
}

QByteArray prepareOk(bool isStart, byte cmd)
{
    QByteArray tmpba;
    if (isStart)
        tmpba.append(Proto::Starters::Request);
    else
        tmpba.append(Proto::Starters::Continue);
    // NOTE Михалыч не следует документации поэтому пока так
    // tmpba.append(Proto::Commands::ResultOk);
    tmpba.append(cmd);
    appendInt16(tmpba, 0);
    return tmpba;
}

QByteArray prepareError()
{
    QByteArray tmpba;
    tmpba.append(Proto::Starters::Request);
    tmpba.append(Proto::Commands::ResultError);
    appendInt16(tmpba, 1);
    // модулю не нужны коды ошибок
    tmpba.append(static_cast<char>(Proto::NullByte));
    return tmpba;
}

QByteArray prepareBlock(CommandStruct &cmdStr, Proto::Starters startByte, quint16 size)
{
    QByteArray ba;
    ba.append(startByte);
    ba.append(cmdStr.cmd);
    appendInt16(ba, cmdStr.ba.size());
    if (!cmdStr.arg1.isNull())
        ba.append(cmdStr.arg1.toUInt());
    if (!cmdStr.ba.isEmpty())
        ba.append(cmdStr.ba);
    return ba;
}
// NOTE Не проверено
ByteQueue prepareLongBlk(CommandStruct &cmdStr)
{
    ByteQueue bq;
    using Proto::Limits::MaxSegmenthLength;
    // Количество сегментов
    quint64 segCount
        = (cmdStr.ba.size() + 1) // +1 Т.к. некоторые команды имеют в значимой части один дополнительный байт
            / MaxSegmenthLength  // Максимальная длинна сегмента
        + 1; // Добавляем еще один сегмент в него попадет последняя часть
    bq.reserve(segCount);

    CommandStruct temp { cmdStr.cmd, cmdStr.arg1, cmdStr.arg2, cmdStr.ba.left(MaxSegmenthLength) };
    bq << prepareBlock(temp);

    for (int pos = MaxSegmenthLength; pos < cmdStr.ba.size(); pos += MaxSegmenthLength)
    {
        temp = { cmdStr.cmd, cmdStr.arg1, cmdStr.arg2, cmdStr.ba.mid(pos, MaxSegmenthLength) };
        bq << prepareBlock(temp, Proto::Starters::Continue);
    }
    return bq;
}

void handleBitString(const QByteArray &ba, quint16 sigAddr)
{
    Q_ASSERT(ba.size() != 4);
    quint32 value = ba.toUInt();
    DataTypes::BitStringStruct resp { sigAddr, value, {} };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::BitString, resp);
}

void handleBitStringArray(const QByteArray &ba, QList<quint16> arr_addr)
{
    Q_ASSERT(ba.size() / 4 != arr_addr.size());
    for (int i = 0; i != arr_addr.size(); i++)
    {
        QByteArray temp = ba.mid(sizeof(qint32) * i, sizeof(qint32));
        handleBitString(temp, arr_addr.at(i));
    }
}

void handleFloat(const QByteArray &ba, quint32 sigAddr)
{
    Q_ASSERT(ba.size() != 4);
    float blk = ba.toFloat();
    // std::copy_n(ba.constData(), sizeof(float), blk);
    DataTypes::FloatStruct resp { sigAddr, blk };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::Float, resp);
}

void handleFloatArray(const QByteArray &ba, quint32 sigAddr, quint32 sigCount)
{
    if (!sigCount)
        handleFloat(ba, sigAddr);
    Q_ASSERT(ba.size() != int(sigCount * 4));
    for (quint32 i = 0; i != sigCount; i++)
    {
        QByteArray temp = ba.mid(sizeof(qint32) * i, sizeof(qint32));
        handleFloat(temp, sigAddr + i);
    }
}

void handleFile(QByteArray &ba, quint16 addr, bool isShouldRestored)
{

    if (isShouldRestored)
    {
        QList<DataTypes::ConfParameterStruct> outlist;
        Error::Msg error_code = S2::RestoreData(ba, outlist);
        if (error_code == Error::Msg::NoError)
            DataManager::addSignalToOutList(DataTypes::ConfParametersList, outlist);
        else
            qCritical() << error_code;
    }
    else
    {
        DataTypes::FileStruct resp { addr, ba };
        DataManager::addSignalToOutList(DataTypes::SignalTypes::File, resp);
    }
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