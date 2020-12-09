#include "protocomthread.h"

#include "../gen/datamanager.h"
#include "../gen/files.h"
#include "../gen/logclass.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"

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
inline bool isSplitted(unsigned len);

// TODO вынести в отдельный класс как static методы?
QByteArray prepareOk(bool isStart, byte cmd);
QByteArray prepareError();
QByteArray prepareBlock(CommandStruct &cmdStr, Starters startByte = Starters::Request);
QByteArray prepareBlock(Proto::Commands cmd, QByteArray &data, Proto::Starters startByte = Starters::Request);
ByteQueue prepareLongBlk(CommandStruct &cmdStr);

void handleBitString(const QByteArray &ba, quint16 sigAddr);
void handleBitStringArray(const QByteArray &ba, QList<quint16> arr_addr);
void handleFloat(const QByteArray &ba, quint32 sigAddr);
void handleFloatArray(const QByteArray &ba, quint32 sigAddr, quint32 sigCount);
void handleSinglePoint(const QByteArray &ba, const quint16 addr);
void handleFile(QByteArray &ba, DataTypes::FilesEnum addr, bool isShouldRestored);
void handleInt(const byte num);
void handleBool(const bool status = true, int errorSize = 0, int errorCode = 0);
void handleProgress(const quint64 progress)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataCount, progress };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
}
void handleMaxProgress(const quint64 progress)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataSize, progress };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
}
void handleRawBlock(const QByteArray &ba, quint32 blkNum);
inline void handleCommand(const QByteArray &ba);

ProtocomThread::ProtocomThread(QObject *parent) : QObject(parent), m_currentCommand({})
{
    // QString tmps = "=== Log started ===";
    // log = new LogClass;
    // log->Init("canal.log");
    // writeLog(tmps.toUtf8());
}

void ProtocomThread::setReadDataChunk(const QByteArray &readDataChunk)
{
    // QMutexLocker locker(&_mutex);
    m_readData = readDataChunk;
    _waiter.wakeOne();
    emit readyRead();
}

void ProtocomThread::appendReadDataChunk(const QByteArray &readDataChunk)
{
    // QMutexLocker locker(&_mutex);
    m_readData.append(readDataChunk);
    _waiter.wakeOne();
    emit readyRead();
}

void ProtocomThread::wakeUp()
{
    _waiter.wakeOne();
}

void ProtocomThread::parse()
{
    forever
    {
        _mutex.lock();
        if (!isCommandRequested)
            checkQueue();
        if (m_readData.isEmpty())
            _waiter.wait(&_mutex);
        else
        {
            parseResponse(m_readData);
            m_readData.clear();
        }

        _mutex.unlock();
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
        if (isSplitted(m_currentCommand.ba.size()))
        {
            // For first segment
            if (progress == NULL)
                handleMaxProgress(m_currentCommand.ba.size());
            progress += Proto::Limits::MaxSegmenthLength;
            // For last segment
            if (progress > m_currentCommand.ba.size())
            {
                progress = m_currentCommand.ba.size();
            }
            handleProgress(progress);
            // m_currentCommand.ba.chop(Proto::Limits::MaxSegmenthLength);
            return;
        }
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

        // handleFloatArray(m_buffer.second, addr, count);
        // Ожидается что в addr хранится номер блока
        handleRawBlock(m_buffer.second, addr);
        break;

    case Commands::ReadBlkDataA:

        // handleFloatArray(m_buffer.second, addr, count);
        // Ожидается что в addr хранится номер блока
        handleRawBlock(m_buffer.second, addr);
        break;

    case Commands::ReadBlkData:

        // Превосходный костыль для сигнализации
        if (addr != alarm_reg)
            handleFloatArray(m_buffer.second, addr, count);
        else
            handleSinglePoint(m_buffer.second, addr);

        break;

    case Commands::ReadBlkTech:

        handleFloatArray(m_buffer.second, addr, count);
        break;

    case Commands::ReadProgress:

        handleBitString(m_buffer.second, addr);
        break;

    case Commands::ReadFile:

        handleFile(m_buffer.second, FilesEnum(addr), static_cast<bool>(count));
        break;

    default:

        handleCommand(m_buffer.second);
        break;
    }
    isCommandRequested = false;
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
        isCommandRequested = true;
        progress = 0;
        m_currentCommand = inp;
        parseRequest(inp);
        break;
    }
}

// void JournalDialog::TryGetJourByUSB()
//{
//    QString filetofind;
//    int jourtype = GetJourNum(sender()->objectName());
//    switch (jourtype)
//    {
//    case Files::JourSys:
//        filetofind = "system.dat";
//        break;
//    case Files::JourWork:
//        filetofind = "workj.dat";
//        break;
//    case Files::JourMeas:
//        filetofind = "measj.dat";
//        break;
//    default:
//        ERMSG("Incorrect jour type");
//        return;
//        break;
//    }

//    JourType = jourtype;
//    JourFuncs->SetJourType(jourtype);
//    // QByteArray ba;
//    QStringList drives = Files::Drives();
//    if (!drives.isEmpty())
//    {
//        QStringList files = Files::SearchForFile(drives, filetofind);
//        if (!files.isEmpty())
//        {
//            JourFile = Files::GetFirstDriveWithLabel(files, "AVM");
//            JourFuncs->SetJourFile(JourFile);
//            if (JourFile.isEmpty())
//                GetJour();
//            else
//                StartReadJourFile();
//        }
//        else
//            GetJour();
//    }
//    else
//        GetJour();
//}
void ProtocomThread::fileHelper(DataTypes::FilesEnum fileNum)
{
    QString fileToFind;
    switch (fileNum)
    {
    case DataTypes::JourSys:
    {
        fileToFind = "system.dat";
        break;
    }
    case DataTypes::JourMeas:
    {
        fileToFind = "measj.dat";
        break;
    }
    case DataTypes::JourWork:
    {
        fileToFind = "workj.dat";
        break;
    }

    default:
    {
        m_currentCommand.ba = StdFunc::arrayFromNumber(fileNum);
        QByteArray ba = prepareBlock(m_currentCommand);
        emit writeDataAttempt(ba);
        return;
    }
    }
    QStringList drives = Files::Drives();
    if (drives.isEmpty())
    {
        qCritical() << Error::NoDeviceError;
        return;
    }
    QStringList files = Files::SearchForFile(drives, fileToFind);
    if (files.isEmpty())
    {
        qCritical() << Error::FileNameError;
        return;
    }
    QString JourFile = Files::GetFirstDriveWithLabel(files, "AVM");
    if (JourFile.isEmpty())
    {
        qCritical() << Error::FileNameError;
        return;
    }
    QFile file(JourFile);
    if (!file.open(QIODevice::ReadOnly))
    {
        qCritical() << Error::FileOpenError;
        return;
    }
    QByteArray ba = file.readAll();
    handleFile(ba, fileNum, false);
    isCommandRequested = false;
}

void ProtocomThread::parseRequest(const CommandStruct &cmdStr)
{
#ifdef PROTOCOM_DEBUG
    qDebug("Start parse request");
#endif
    // Предполагается не хранить текущую команду
    Q_UNUSED(cmdStr)
    using namespace Proto;

    switch (m_currentCommand.cmd)
    {
    case Commands::ReadBlkData:
    {
        quint16 blk = Proto::getBlkByReg.value(m_currentCommand.arg1.value<quint16>()).first;
        m_currentCommand.ba = StdFunc::arrayFromNumber(quint8(blk));
        QByteArray ba = prepareBlock(m_currentCommand);
        emit writeDataAttempt(ba);
        break;
    }
        //    case Commands::ReadBlkDataA:
        //    {
        //        m_currentCommand.ba = StdFunc::arrayFromNumber(m_currentCommand.arg1.value<quint8>());
        //        QByteArray ba = prepareBlock(m_currentCommand);
        //        emit writeDataAttempt(ba);
        //    }
        //    case Commands::ReadBlkAC:
        //    {
        //        m_currentCommand.ba = StdFunc::arrayFromNumber(m_currentCommand.arg1.value<quint8>());
        //        QByteArray ba = prepareBlock(m_currentCommand);
        //        emit writeDataAttempt(ba);
        //    }
        //    case Commands::ReadBlkTech:
        //    {
        //        m_currentCommand.ba = StdFunc::arrayFromNumber(m_currentCommand.arg1.value<quint8>());
        //        QByteArray ba = prepareBlock(m_currentCommand);
        //        emit writeDataAttempt(ba);
        //    }
    case Commands::ReadFile:
    {
        // const Files::FilesEnum fileNumber = m_currentCommand.arg1.value<Files::FilesEnum>();
        fileHelper(m_currentCommand.arg1.value<DataTypes::FilesEnum>());
        break;
    }
    case Commands::WriteTime:
    {
        m_currentCommand.ba = StdFunc::arrayFromNumber(m_currentCommand.arg1.value<quint32>());
        QByteArray ba = prepareBlock(m_currentCommand);
        emit writeDataAttempt(ba);
        break;
    }
    case Commands::RawCommand:
    {
        emit writeDataAttempt(m_currentCommand.ba);
        break;
    }
    case Commands::WriteFile:
    {
    }

        //    case Commands::WriteMode:
        //    {
        //        m_currentCommand.ba = StdFunc::arrayFromNumber(m_currentCommand.arg1.value<quint8>());
        //        QByteArray ba = prepareBlock(m_currentCommand);
        //        emit writeDataAttempt(ba);
        //    }
        //    case Commands::WriteVariant:
        //    {
        //        m_currentCommand.ba = StdFunc::arrayFromNumber(m_currentCommand.arg1.value<quint8>());
        //        QByteArray ba = prepareBlock(m_currentCommand);
        //        emit writeDataAttempt(ba);
        //    }
        //    case Commands::Test:
        //    {
        //        m_currentCommand.ba = StdFunc::arrayFromNumber(m_currentCommand.arg1.value<quint8>());
        //        QByteArray ba = prepareBlock(m_currentCommand);
        //        emit writeDataAttempt(ba);
        //    }
    default:
    {
        if (isSplitted(m_currentCommand.ba.size()))
        {
            auto query = prepareLongBlk(m_currentCommand);
            while (!query.isEmpty())
                emit writeDataAttempt(query.dequeue());
        }
        else
        {
            if (m_currentCommand.arg1.isValid())
                m_currentCommand.ba.prepend(StdFunc::arrayFromNumber(m_currentCommand.arg1.value<quint8>()));
            QByteArray ba = prepareBlock(m_currentCommand);
            emit writeDataAttempt(ba);
        }
        break;
    }
    }
}

void ProtocomThread::parseResponse(QByteArray ba)
{
#ifdef PROTOCOM_DEBUG
    qDebug("Start parse response");
#endif
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
        // Потому что на эту команду модуль не отдает пустой ответ
        if (isOneSegment(size) || (cmd == ReadBlkStartInfo))
            handle(Commands(cmd));
        else
        {

            auto tba = prepareOk(false, cmd);
            Q_ASSERT(tba.size() == 4);
            emit writeDataAttempt(tba);
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
#ifdef PROTOCOM_DEBUG
    QByteArray tmpba = QByteArray(metaObject()->className());
    switch (dir)
    {
    case Proto::FromDevice:
        tmpba.append(": <-");
        break;
    case Proto::ToDevice:
        tmpba.append(": ->");
        break;
    default:
        tmpba.append(":  ");
        break;
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

bool isSplitted(unsigned len)
{
    using Proto::Limits::MaxSegmenthLength;
    if (len < MaxSegmenthLength)
        return false;
    return true;
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

QByteArray prepareBlock(CommandStruct &cmdStr, Proto::Starters startByte)
{
    QByteArray ba;
    ba.append(startByte);
    ba.append(cmdStr.cmd);
    appendInt16(ba, cmdStr.ba.size());
    //    if (!cmdStr.arg1.isNull())
    //        ba.append(cmdStr.arg1.toUInt());
    if (!cmdStr.ba.isEmpty())
        ba.append(cmdStr.ba);
    return ba;
}

QByteArray prepareBlock(Proto::Commands cmd, QByteArray &data, Proto::Starters startByte)
{
    QByteArray ba;
    ba.append(startByte);
    ba.append(cmd);
    appendInt16(ba, data.size());
    //    if (!cmdStr.arg1.isNull())
    //        ba.append(cmdStr.arg1.toUInt());
    if (!data.isEmpty())
        ba.append(data);
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
    //    if (cmdStr.arg1.isValid())
    //    {
    QByteArray tba;
    if (cmdStr.arg1.isValid())
        tba = StdFunc::arrayFromNumber(cmdStr.arg1.value<quint8>());
    tba.append(cmdStr.ba.left(MaxSegmenthLength - 1));
    //  }
    // CommandStruct temp { cmdStr.cmd, cmdStr.arg1, cmdStr.arg2, tba };
    bq << prepareBlock(cmdStr.cmd, tba);

    for (int pos = MaxSegmenthLength - 1; pos < cmdStr.ba.size(); pos += MaxSegmenthLength)
    {
        // temp = { cmdStr.cmd, cmdStr.arg1, cmdStr.arg2, cmdStr.ba.mid(pos, MaxSegmenthLength) };
        tba = cmdStr.ba.mid(pos, MaxSegmenthLength);
        bq << prepareBlock(cmdStr.cmd, tba, Proto::Starters::Continue);
        // bq << prepareBlock(temp, Proto::Starters::Continue);
    }
    return bq;
}

void handleBitString(const QByteArray &ba, quint16 sigAddr)
{
    Q_ASSERT(ba.size() == sizeof(quint32));

    quint32 value = *reinterpret_cast<const quint32 *>(ba.data());
    DataTypes::BitStringStruct resp { sigAddr, value, {} };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::BitString, resp);
}

void handleBitStringArray(const QByteArray &ba, QList<quint16> arr_addr)
{
    Q_ASSERT(ba.size() / 4 == arr_addr.size());
    for (int i = 0; i != arr_addr.size(); i++)
    {
        QByteArray temp = ba.mid(sizeof(qint32) * i, sizeof(qint32));
        handleBitString(temp, arr_addr.at(i));
    }
}

void handleFloat(const QByteArray &ba, quint32 sigAddr)
{
    Q_ASSERT(ba.size() == 4);
    float blk = *reinterpret_cast<const float *>(ba.data());
    DataTypes::FloatStruct resp { sigAddr, blk };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::Float, resp);
}

void handleFloatArray(const QByteArray &ba, quint32 sigAddr, quint32 sigCount)
{
    if (!sigCount)
        handleFloat(ba, sigAddr);
    Q_ASSERT(ba.size() == int(sigCount * 4));
    for (quint32 i = 0; i != sigCount; i++)
    {
        QByteArray temp = ba.mid(sizeof(qint32) * i, sizeof(qint32));
        handleFloat(temp, sigAddr + i);
    }
}

void handleSinglePoint(const QByteArray &ba, const quint16 addr)
{
    for (quint32 i = 0; i != quint32(ba.size()); ++i)
    {
        quint8 value = ba.at(i);
        DataTypes::SinglePointWithTimeStruct data { (addr + i), value, 0 };
        DataManager::addSignalToOutList(DataTypes::SinglePointWithTime, data);
    }
}

void handleFile(QByteArray &ba, DataTypes::FilesEnum addr, bool isShouldRestored)
{

    if (isShouldRestored)
    {
        DataTypes::ConfParametersListStruct outlist;
        Error::Msg error_code = S2::RestoreData(ba, outlist);
        if (error_code != Error::Msg::NoError)
        {
            qCritical() << error_code;
            return;
        }
        DataManager::addSignalToOutList(DataTypes::ConfParametersList, outlist);
        //        for (const auto &param : outlist)
        //            DataManager::addSignalToOutList(DataTypes::ConfParameter, param);
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
        // Module error code
        qCritical() << "Error size: " << errorSize << "Error code: " << QString::number(errorCode, 16);
    }
}

void handleRawBlock(const QByteArray &ba, quint32 blkNum)
{
    DataTypes::BlockStruct resp { blkNum, ba };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::Block, resp);
}

void handleCommand(const QByteArray &ba)
{
    qCritical("We should be here, something went wrong");
    qDebug() << ba.toHex();
}
