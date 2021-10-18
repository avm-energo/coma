#include "protocomthread.h"

//#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../gen/files.h"
#include "../gen/helper.h"
#include "../gen/logclass.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"
#include "baseinterface.h"

#include <QDebug>
#include <QStorageInfo>
#include <QtEndian>
//#include <QMetaEnum>
#include <QThread>

#ifdef Q_OS_LINUX
#include <time.h>
#endif

#ifdef Q_OS_WINDOWS
#define BUFFERSIZE 5

#include <Windows.h>
#include <fileapi.h>
#include <memoryapi.h>
#endif

typedef QQueue<QByteArray> ByteQueue;
using Proto::CommandStruct;
using Proto::Direction;
using Proto::Starters;
using Queries::FileFormat;

ProtocomThread::ProtocomThread(QObject *parent) : QObject(parent), m_currentCommand({})
{
    // QString tmps = "=== Log started ===";
    // log = new LogClass;
    // log->Init("canal.log");
    // writeLog(tmps.toUtf8());
}

void ProtocomThread::setReadDataChunk(const QByteArray &readDataChunk)
{
    QMutexLocker locker(&_mutex);
    m_readData = readDataChunk;
    _waiter.wakeOne();
    emit readyRead();
}

void ProtocomThread::appendReadDataChunk(const QByteArray &readDataChunk)
{
    QMutexLocker locker(&_mutex);
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
    while (BaseInterface::iface()->state() != BaseInterface::State::Stop)
    // while (Board::GetInstance().connectionState() != Board::ConnectionState::Closed)
    {
        QMutexLocker locker(&_mutex);
        if (!isCommandRequested)
            checkQueue();
        if (m_readData.isEmpty())
            _waiter.wait(&_mutex);
        else
        {
            parseResponse(m_readData);
            m_readData.clear();
        }
    }
}

void ProtocomThread::clear()
{
    QMutexLocker locker(&_mutex);
    isCommandRequested = false;
    m_readData.clear();
    progress = 0;
    m_currentCommand = Proto::CommandStruct();
    m_buffer.first = 0;
    m_buffer.second.clear();
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
}

void ProtocomThread::handle(const Proto::Commands cmd)
{
    using namespace Proto;
    using namespace DataTypes;
    quint32 addr = m_currentCommand.arg1.toUInt();
    quint32 count = m_currentCommand.arg2.toUInt();
    switch (cmd)
    {
    case Commands::ResultOk:

        // Ignore good replies to splitted packet
        // Не прибавляем никаких 1 или 2, надо будет проверить
        if (isSplitted(m_currentCommand.ba.size()))
        {
            // For first segment
            if (!progress)
                handleMaxProgress(m_currentCommand.ba.size());

            progress += Proto::Limits::MaxSegmenthLength;
            if (progress > m_currentCommand.ba.size())
            {
                // For last segment
                progress = m_currentCommand.ba.size();
                handleProgress(progress);
            }
            else
            {
                handleProgress(progress);
                return;
            }

            // break;
        }

        //  GVar MS GMode MS
        if (!m_buffer.second.isEmpty())
            handleInt(m_buffer.second.front());
        else
            handleBool();
        break;

    case Commands::ResultError:
    {
        const quint8 errorCode = m_buffer.second.front();

        handleBool(false, m_buffer.first, errorCode);
        emit errorOccurred(static_cast<Error::Msg>(errorCode));
        break;
    }
    case Commands::ReadTime:

#ifdef Q_OS_LINUX
        if (m_buffer.second.size() == sizeof(quint64))
        {
            handleUnixTime(m_buffer.second, addr);
            break;
        }
#endif
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

        switch (m_currentCommand.cmd)
        {
        case Commands::FakeReadRegData:
            handleFloatArray(m_buffer.second, addr, count);
            break;
        case Commands::FakeReadAlarms:
            handleSinglePoint(m_buffer.second, addr);
            break;
        case Commands::FakeReadBitString:
            handleBitString(m_buffer.second, addr);
            break;
        default:
            handleRawBlock(m_buffer.second, addr);
            break;
        }
        break;

    case Commands::ReadBlkTech:

        // handleFloatArray(m_buffer.second, addr, count);
        handleTechBlock(m_buffer.second, addr);
        break;

    case Commands::ReadProgress:

        handleBitString(m_buffer.second, addr);
        break;

    case Commands::ReadFile:

        handleFile(m_buffer.second, FilesEnum(addr), FileFormat(count));
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
    //    switch (inp.cmd)
    //    {
    //    default:
    isCommandRequested = true;
    progress = 0;
    m_currentCommand = inp;
    parseRequest(inp);
    //       break;
    //    }
}
#if defined(Q_OS_WINDOWS)
void ProtocomThread::fileHelper(DataTypes::FilesEnum fileNum)
{
    std::wstring fileToFind;
    switch (fileNum)
    {
    case DataTypes::JourSys:
    {

        fileToFind = L"system.dat";
        break;
    }
    case DataTypes::JourMeas:
    {
        fileToFind = L"measj.dat";
        break;
    }
    case DataTypes::JourWork:
    {
        fileToFind = L"workj.dat";
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
    isCommandRequested = false;

    QStringList drives = Files::Drives();
    if (drives.isEmpty())
    {
        qCritical() << Error::NoDeviceError;
        return;
    }
    QStringList files = Files::SearchForFile(drives, QString::fromStdWString(fileToFind));
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
    QStorageInfo storageInfo(JourFile);
    QString nativePath = storageInfo.rootPath().replace(('/'), ('\\'));
    std::wstring nativePathW(nativePath.toStdWString().c_str());
    nativePathW += fileToFind;
    HANDLE hFile;
    OVERLAPPED ol = { 0 };
    hFile = CreateFile(nativePathW.c_str(),                                        // file to open
        GENERIC_READ,                                                              // open for reading
        FILE_SHARE_READ,                                                           // share for reading
        NULL,                                                                      // default security
        OPEN_EXISTING,                                                             // existing file only
        FILE_ATTRIBUTE_NORMAL /*| FILE_FLAG_OVERLAPPED*/ | FILE_FLAG_NO_BUFFERING, // normal file
        NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        qCritical() << Error::FileNameError;
        return;
    }

    QByteArray buffer;
    buffer.resize(file.size());
    DWORD lpNumberOfBytesRead;
    if (FALSE == ReadFile(hFile, buffer.data(), buffer.size(), &lpNumberOfBytesRead, &ol))
    {
        DWORD dwLastError = GetLastError();

        if (dwLastError != 0)
        { // Don't want to see a "operation done successfully" error
            TCHAR lpBuffer[256] = L"?";
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,      // It´s a system error
                NULL,                                      // No string to be formatted needed
                dwLastError,                               // Hey Windows: Please explain this error!
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Do it in the standard language
                lpBuffer,                                  // Put the message here
                255,                                       // Number of bytes to store the message
                NULL);
            qCritical() << Error::FileOpenError << std::wstring(lpBuffer);
        }
        qDebug() << "Fail to read";
        CloseHandle(hFile);
        return;
    }
    CloseHandle(hFile);
    QByteArray ba = buffer;
    handleFile(ba, fileNum, FileFormat::Binary);
}
#else

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
    isCommandRequested = false;

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
    handleFile(ba, fileNum, Queries::FileFormat::Binary);
}

#endif
void ProtocomThread::parseRequest(const CommandStruct &cmdStr)
{
#ifdef PROTOCOM_DEBUG
    qDebug("Start parse request");
#endif
    Q_ASSERT(cmdStr.cmd != 0x00);
    // Предполагается не хранить текущую команду
    Q_UNUSED(cmdStr)
    using namespace Proto;

    switch (m_currentCommand.cmd)
    {
    case Commands::ReadBlkData:
    {
        const quint16 blk = m_currentCommand.arg1.value<quint16>();
        m_currentCommand.ba = StdFunc::arrayFromNumber(quint8(blk));
        QByteArray ba = prepareBlock(m_currentCommand);
        emit writeDataAttempt(ba);
        break;
    }
    case Commands::FakeReadAlarms:
    case Commands::FakeReadBitString:
    case Commands::FakeReadRegData:
    {

        QByteArray ba = prepareBlock(Commands::ReadBlkData, m_currentCommand.ba);
        emit writeDataAttempt(ba);
        break;
    }

    case Commands::ReadFile:
    {
        fileHelper(m_currentCommand.arg1.value<DataTypes::FilesEnum>());
        break;
    }
    case Commands::WriteTime:
    {
#ifdef Q_OS_LINUX
        if (m_currentCommand.arg1.canConvert<timespec>())
        {
            timespec time = m_currentCommand.arg1.value<timespec>();
            m_currentCommand.ba.push_back(StdFunc::arrayFromNumber(quint32(time.tv_sec)));
            m_currentCommand.ba.push_back(StdFunc::arrayFromNumber(quint32(time.tv_nsec)));
        }
        else
#endif
        {
            m_currentCommand.ba = StdFunc::arrayFromNumber(m_currentCommand.arg1.value<quint32>());
        }
        QByteArray ba = prepareBlock(m_currentCommand);
        emit writeDataAttempt(ba);
        break;
    }
    case Commands::RawCommand:
    {
        emit writeDataAttempt(m_currentCommand.ba);
        break;
    }
    case Commands::WriteSingleCommand:
    {
        assert(m_currentCommand.arg1.canConvert<uint24>());
        uint24 addr = m_currentCommand.arg1.value<uint24>();

        QByteArray buffer = StdFunc::arrayFromNumber((addr)) + m_currentCommand.ba;

        QByteArray ba = prepareBlock(Commands::WriteSingleCommand, buffer);
        emit writeDataAttempt(ba);
        break;
    }
    case Commands::WriteFile:
    {
    }

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
    // using namespace Proto;

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

    case Proto::Response:
    {
        ba.remove(0, 4);
        // TODO Проверять размер
        ba.resize(size);
        // Q_ASSERT(size == ba.size());
        m_buffer.first += size;
        m_buffer.second.append(ba);

        quint32 filenum = m_currentCommand.arg1.value<quint32>();

        // Потому что на эту команду модуль не отдает пустой ответ
        if (isOneSegment(size) || (cmd == Proto::ReadBlkStartInfo))
        {
            handle(Proto::Commands(cmd));
            // Progress for big files
            if (m_currentCommand.cmd == Proto::Commands::ReadFile)
            {
                if (filenum != DataTypes::Config)
                {
                    progress += size;
                    handleProgress(progress);
                }
            }
        }
        else
        {

            auto tba = prepareOk(false, cmd);
            Q_ASSERT(tba.size() == 4);
            emit writeDataAttempt(tba);
            // Progress for big files
            if (m_currentCommand.cmd == Proto::Commands::ReadFile)
            {
                if (filenum != DataTypes::Config)
                {
                    progress += Proto::Limits::MaxSegmenthLength;
                    handleProgress(progress);
                }
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
#if PROTOCOM_DEBUG
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
#else
    Q_UNUSED(ba);
    Q_UNUSED(dir);
#endif
}

quint16 ProtocomThread::extractLength(const QByteArray &ba)
{
    quint16 len = static_cast<quint8>(ba.at(2));
    len += static_cast<quint8>(ba.at(3)) * 256;
    return len;
}

void ProtocomThread::appendInt16(QByteArray &ba, quint16 size)
{
    ba.append(static_cast<char>(size % 0x100));
    ba.append(static_cast<char>(size / 0x100));
}

bool ProtocomThread::isCommandExist(int cmd)
{
    if (cmd == -1)
    {
        qCritical() << Error::WrongCommandError;
        return false;
    }
    return true;
}

bool ProtocomThread::isOneSegment(unsigned len)
{
    using Proto::Limits::MaxSegmenthLength;
    Q_ASSERT(len <= MaxSegmenthLength);
    return (len != MaxSegmenthLength);
}

bool ProtocomThread::isSplitted(unsigned len)
{
    using Proto::Limits::MaxSegmenthLength;
    return !(len < MaxSegmenthLength);
}

QByteArray ProtocomThread::prepareOk(bool isStart, byte cmd)
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

QByteArray ProtocomThread::prepareError()
{
    QByteArray tmpba;
    tmpba.append(Proto::Starters::Request);
    tmpba.append(Proto::Commands::ResultError);
    appendInt16(tmpba, 1);
    // модулю не нужны коды ошибок
    tmpba.append(static_cast<char>(Proto::NullByte));
    return tmpba;
}

QByteArray ProtocomThread::prepareBlock(CommandStruct &cmdStr, Proto::Starters startByte)
{

    /* QByteArray ba;
     ba.append(startByte);
     ba.append(cmdStr.cmd);
     appendInt16(ba, cmdStr.ba.size());*/
    //    if (!cmdStr.arg1.isNull())
    //        ba.append(cmdStr.arg1.toUInt());
    /* if (!cmdStr.ba.isEmpty())
         ba.append(cmdStr.ba);*/
    return /*ba*/ prepareBlock(cmdStr.cmd, cmdStr.ba, startByte);
    ;
}

QByteArray ProtocomThread::prepareBlock(Proto::Commands cmd, QByteArray &data, Proto::Starters startByte)
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

ByteQueue ProtocomThread::prepareLongBlk(CommandStruct &cmdStr)
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

void ProtocomThread::handleBitString(const QByteArray &ba, quint16 sigAddr)
{
    Q_ASSERT(ba.size() == sizeof(quint32));

    quint32 value = *reinterpret_cast<const quint32 *>(ba.data());
    DataTypes::BitStringStruct resp { sigAddr, value, {} };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::BitString, resp);
}
#ifdef __linux
void ProtocomThread::handleUnixTime(const QByteArray &ba, [[maybe_unused]] quint16 sigAddr)
{
    Q_ASSERT(ba.size() == sizeof(quint64));

    const quint32 secs = *reinterpret_cast<const quint32 *>(ba.data());
    const quint32 nsecs = *reinterpret_cast<const quint32 *>(ba.data() + sizeof(quint32));
    timespec resp;
    resp.tv_nsec = nsecs;
    resp.tv_sec = secs;
    DataManager::addSignalToOutList(DataTypes::SignalTypes::Timespec, resp);
}
#endif
void ProtocomThread::handleBitStringArray(const QByteArray &ba, QList<quint16> arr_addr)
{
    Q_ASSERT(ba.size() / 4 == arr_addr.size());
    for (int i = 0; i != arr_addr.size(); i++)
    {
        QByteArray temp = ba.mid(sizeof(qint32) * i, sizeof(qint32));
        handleBitString(temp, arr_addr.at(i));
    }
}

void ProtocomThread::handleFloat(const QByteArray &ba, quint32 sigAddr)
{
    Q_ASSERT(ba.size() == 4);
    float blk = *reinterpret_cast<const float *>(ba.data());
    DataTypes::FloatStruct resp { sigAddr, blk };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::Float, resp);
}

void ProtocomThread::handleFloatArray(const QByteArray &ba, quint32 sigAddr, quint32 sigCount)
{
    if (!sigCount)
        handleFloat(ba, sigAddr);
    // NOTE Проблема со стартовыми регистрами, получим на один регистр больше чем по другим протоколам
    Q_ASSERT(ba.size() >= int(sigCount * 4));
    for (quint32 i = 0; i != sigCount; i++)
    {
        QByteArray temp = ba.mid(sizeof(float) * i, sizeof(float));
        handleFloat(temp, sigAddr + i);
    }
}

void ProtocomThread::handleSinglePoint(const QByteArray &ba, const quint16 addr)
{
    for (quint32 i = 0; i != quint32(ba.size()); ++i)
    {
        quint8 value = ba.at(i);
        DataTypes::SinglePointWithTimeStruct data { (addr + i), value, 0 };
        DataManager::addSignalToOutList(DataTypes::SinglePointWithTime, data);
    }
}

void ProtocomThread::handleFile(QByteArray &ba, DataTypes::FilesEnum addr, Queries::FileFormat format)
{
    switch (format)
    {
    case FileFormat::Binary:
    {
        DataTypes::FileStruct resp { addr, ba };
        DataManager::addSignalToOutList(DataTypes::SignalTypes::File, resp);
        break;
    }
    case FileFormat::DefaultS2:
    {
        QList<DataTypes::DataRecV> outlistV;

        if (!S2::RestoreData(ba, outlistV))
            return;
        DataManager::addSignalToOutList(DataTypes::DataRecVList, outlistV);
        break;
    }
    case FileFormat::CustomS2:
    {
        DataTypes::S2FilePack outlist;
        if (!S2::RestoreData(ba, outlist))
        {
            DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Error,
                static_cast<quint64>(ba.size()) };
            DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
            return;
        }
        for (auto &&file : outlist)
        {
            DataTypes::FileStruct resp { DataTypes::FilesEnum(file.ID), file.data };
            DataManager::addSignalToOutList(DataTypes::SignalTypes::File, resp);
        }

        break;
    }
    }
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, static_cast<quint64>(ba.size()) };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
}

void ProtocomThread::handleInt(const byte num)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, num };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
}

void ProtocomThread::handleBool(const bool status, int errorSize, int errorCode)
{
    if (status)
    {
        DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, 0 };
        DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
    }
    else
    {
        quint64 buffer = errorCode;
        DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Error, buffer };
        DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
        // Module error code
        qCritical() << "Error size: " << errorSize << "Error code: " << QString::number(errorCode, 16);
    }
}

void ProtocomThread::handleProgress(const quint64 progress)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataCount, progress };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
}

void ProtocomThread::handleMaxProgress(const quint64 progress)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataSize, progress };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
}

void ProtocomThread::handleRawBlock(const QByteArray &ba, quint32 blkNum)
{
    DataTypes::BlockStruct resp { blkNum, ba };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::Block, resp);
}

void ProtocomThread::handleCommand(const QByteArray &ba)
{
    qCritical("We should be here, something went wrong");
    qDebug() << ba.toHex();
}

void ProtocomThread::handleTechBlock(const QByteArray &ba, quint32 blkNum)
{
    switch (blkNum)
    {
        //  Блок наличия осциллограмм Bo
    case 0x01:
    {
        Q_ASSERT(ba.size() % sizeof(S2DataTypes::OscInfo) == 0);
        for (int i = 0; i != ba.size(); i += sizeof(S2DataTypes::OscInfo))
        {
            QByteArray buffer = ba.mid(i, sizeof(S2DataTypes::OscInfo));

            S2DataTypes::OscInfo oscInfo;
            memcpy(&oscInfo, buffer.constData(), sizeof(S2DataTypes::OscInfo));
            DataManager::addSignalToOutList(DataTypes::SignalTypes::OscillogramInfo, oscInfo);
        }

        break;
    }
        //  Блок текущих событий Be
    case 0x02:
    {
        qDebug("Блок текущих событий Be");
        break;
    }
        // Блок технологических событий BTe
    case 0x03:
    {
        qDebug("Блок технологических событий BTe");
        break;
    }
    case 0x04:
    {
        qDebug("Блок наличия журналов переключения");
        Q_ASSERT(ba.size() % sizeof(S2DataTypes::SwitchJourInfo) == 0);
        for (int i = 0; i != ba.size(); i += sizeof(S2DataTypes::SwitchJourInfo))
        {
            QByteArray buffer = ba.mid(i, sizeof(S2DataTypes::SwitchJourInfo));

            S2DataTypes::SwitchJourInfo swjInfo;
            memcpy(&swjInfo, buffer.constData(), sizeof(S2DataTypes::SwitchJourInfo));
            DataManager::addSignalToOutList(DataTypes::SignalTypes::SwitchJournalInfo, swjInfo);
        }
        break;
    }

        // Блок рабочего архива (Bra)
    case 0x05:
    {
        qDebug("Блок рабочего архива (Bra)");
        break;
    }
    default:
        qDebug() << ba;
        break;
    }
}
