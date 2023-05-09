#include "protocomthread.h"

#include "../s2/s2.h"
#include "baseinterface.h"

#include <QDebug>
#include <QQueue>
#include <QStorageInfo>
#include <QThread>
#include <QtEndian>
#include <gen/datamanager/datamanager.h>
#include <gen/files.h>
#include <gen/helper.h>
#include <gen/logclass.h>
#include <gen/registers.h>
#include <gen/stdfunc.h>

#ifdef Q_OS_LINUX
#include <time.h>
#endif

typedef QQueue<QByteArray> ByteQueue;
using DataTypes::FileFormat;
using Proto::CommandStruct;
using Proto::Direction;
using Proto::Starters;
using namespace Interface;

ProtocomThread::ProtocomThread(QObject *parent) : BaseInterfaceThread(parent)
{
    isFirstBlock = true;
}

ProtocomThread::~ProtocomThread()
{
}

void ProtocomThread::processFileFromDisk(DataTypes::FilesEnum fileNum)
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
        qDebug() << "Wrong file type!"; // we should not be here
        return;
    }

    m_isCommandRequested = false;

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
    handleFile(ba, fileNum, DataTypes::FileFormat::Binary);
}

void ProtocomThread::parseRequest(const CommandStruct &cmdStr)
{
    QByteArray ba;
#ifdef PROTOCOM_DEBUG
    qDebug("Start parse request");
#endif

    switch (cmdStr.command)
    {
    case Commands::C_ReqAlarms:
    case Commands::C_ReqStartup:
    case Commands::C_ReqFloats:
    case Commands::C_ReqBitStrings:
    {
        quint8 block = blockByReg(cmdStr.arg1.toUInt());
        ba = prepareBlock(Proto::Commands::ReadBlkData, StdFunc::ArrayFromNumber(block));
        emit writeDataAttempt(ba);
        break;
    }
        // commands without any arguments
    case Commands::C_ReqBSI:
    case Commands::C_ReqBSIExt:
    case Commands::C_ReqTime:
    {
        if (protoCommandMap.contains(cmdStr.command))
        {
            ba = prepareBlock(protoCommandMap.value(cmdStr.command));
            emit writeDataAttempt(ba);
        }
        break;
    }
    case Commands::C_EraseTechBlock:
    case Commands::C_EraseJournals:
    case Commands::C_SetMode:
    {
        if (protoCommandMap.contains(cmdStr.command))
        {
            ba = prepareBlock(
                protoCommandMap.value(cmdStr.command), StdFunc::ArrayFromNumber(cmdStr.arg1.value<quint8>()));
            emit writeDataAttempt(ba);
        }
    }
    case Commands::C_ReqFile:
    {
        DataTypes::FilesEnum filetype = cmdStr.arg1.value<DataTypes::FilesEnum>();
        switch (filetype)
        {
        case DataTypes::JourSys:
        case DataTypes::JourWork:
        case DataTypes::JourMeas:
            processFileFromDisk(filetype);
            break;
        default:
            ba = prepareBlock(Proto::Commands::ReadFile, StdFunc::ArrayFromNumber(cmdStr.arg1.value<quint32>()));
            emit writeDataAttempt(ba);
            break;
        }
        break;
    }

    case Commands::C_WriteFile:
    {
        ba = prepareBlock(Proto::Commands::WriteFile, cmdStr.arg2.value<QByteArray>());
        emit writeDataAttempt(ba);
        break;
    }

    case Commands::C_WriteTime:
    {
#ifdef Q_OS_LINUX
        if (cmdStr.arg1.canConvert<timespec>())
        {
            timespec time = cmdStr.arg1.value<timespec>();
            ba.push_back(StdFunc::ArrayFromNumber(quint32(time.tv_sec)));
            ba.push_back(StdFunc::ArrayFromNumber(quint32(time.tv_nsec)));
            ba = prepareBlock(Proto::Commands::WriteTime, ba);
        }
        else
#endif
        {
            ba = StdFunc::ArrayFromNumber(cmdStr.arg1.value<quint32>());
        }
        QByteArray ba = prepareBlock(Proto::Commands::WriteTime, ba);
        emit writeDataAttempt(ba);
        break;
    }

    case Commands::C_WriteHardware:
    case Commands::C_WriteUserValues:
    {
        ba = cmdStr.arg2.value<QByteArray>();
        if (isSplitted(ba.size()))
        {
            auto query = prepareLongBlk(protoCommandMap.value(cmdStr.command), cmdStr.arg1.value<quint8>(), ba);
            while (!query.isEmpty())
                emit writeDataAttempt(
                    query.dequeue()); // неправильно это - не дожидаясь ответа от модуля пихать в него подряд данные
        }
        else
        {
            if (cmdStr.arg1.isValid())
            {
                ba.prepend(StdFunc::ArrayFromNumber(cmdStr.arg1.value<quint8>()));
                ba = prepareBlock(protoCommandMap.value(cmdStr.command), ba);
                emit writeDataAttempt(ba);
            }
        }
        break;
    }

    case Commands::C_WriteSingleCommand:
    {
        if (cmdStr.arg1.canConvert<DataTypes::SingleCommand>())
        {
            DataTypes::SingleCommand scmd = cmdStr.arg1.value<DataTypes::SingleCommand>();
            ba = StdFunc::ArrayFromNumber((scmd.addr)) + StdFunc::ArrayFromNumber(scmd.value);
            ba = prepareBlock(Proto::WriteSingleCommand, ba);
            emit writeDataAttempt(ba);
        }
        break;
    }

    default:
    {
        qDebug() << "There's no such command";
    }
    }
}

void ProtocomThread::parseResponse()
{
    using namespace Proto;
    using namespace DataTypes;
    quint32 addr = m_currentCommand.arg1.toUInt();
    quint32 count = m_currentCommand.arg2.toUInt();
    switch (m_responseReceived)
    {
    case ResultOk:
        //  GVar MS GMode MS
        if (!m_readData.isEmpty())
            handleInt(m_readData.front());
        else
            handleOk();
        break;

    case ResultError:
    {
        const quint8 errorCode = m_readData.front();
        handleError(errorCode);
        break;
    }
    case ReadTime:

#ifdef Q_OS_LINUX
        if (m_readData.size() == sizeof(quint64))
        {
            handleUnixTime(m_readData, addr);
            break;
        }
#endif
        handleBitString(m_readData, addr);
        break;

    case ReadBlkStartInfoExt:
        handleBitStringArray(m_readData, Regs::bsiExtStartReg);
        break;

    case ReadBlkStartInfo:
        handleBitStringArray(m_readData, Regs::bsiReg);
        break;

    case ReadBlkAC:
        // Ожидается что в addr хранится номер блока
        handleRawBlock(m_readData, addr);
        break;

    case ReadBlkDataA:
        // Ожидается что в addr хранится номер блока
        handleRawBlock(m_readData, addr);
        break;

    case ReadBlkData:
        switch (m_commandSent)
        {
        case FakeReadRegData:
            handleFloatArray(m_readData, addr, count);
            break;
        case FakeReadAlarms:
            handleSinglePoint(m_readData, addr);
            break;
        case FakeReadBitString:
            handleBitStringArray(m_readData, addr);
            break;
        default:
            handleRawBlock(m_readData, addr);
            break;
        }
        break;

    case ReadBlkTech:

        handleTechBlock(m_readData, addr);
        break;

    case ReadProgress:

        handleBitString(m_readData, addr);
        break;

    case ReadFile:

        handleFile(m_readData, FilesEnum(addr), FileFormat(count));
        break;

    default:

        handleCommand(m_readData);
        break;
    }
    m_isCommandRequested = false;
    m_readData.clear();
    _waiter.wakeOne();
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
    Q_ASSERT(len <= Proto::MaxSegmenthLength);
    return (len != Proto::MaxSegmenthLength);
}

bool ProtocomThread::isSplitted(unsigned len)
{
    return !(len < Proto::MaxSegmenthLength);
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

QByteArray ProtocomThread::prepareBlock(Proto::Commands cmd, const QByteArray &data, Proto::Starters startByte)
{
    QByteArray ba;
    ba.append(startByte);
    ba.append(cmd);
    appendInt16(ba, data.size());

    if (!data.isEmpty())
        ba.append(data);
    return ba;
}

ByteQueue ProtocomThread::prepareLongBlk(Proto::Commands cmd, quint8 arg1, const QByteArray &arg2)
{
    QByteArray ba;
    ByteQueue bq;
    using Proto::MaxSegmenthLength;
    ba = arg2;
    // Количество сегментов
    quint64 segCount = (ba.size() + 1) // +1 Т.к. некоторые команды имеют в значимой части один дополнительный байт
            / MaxSegmenthLength        // Максимальная длинна сегмента
        + 1; // Добавляем еще один сегмент в него попадет последняя часть
    bq.reserve(segCount);

    QByteArray tba;
    tba = StdFunc::ArrayFromNumber(arg1);
    tba.append(ba.left(MaxSegmenthLength - 1));

    bq << prepareBlock(cmd, tba);

    for (int pos = MaxSegmenthLength - 1; pos < ba.size(); pos += MaxSegmenthLength)
    {
        tba = ba.mid(pos, MaxSegmenthLength);
        bq << prepareBlock(cmd, tba, Proto::Starters::Continue);
    }
    return bq;
}

void ProtocomThread::handleBitString(const QByteArray &ba, quint16 sigAddr)
{
    Q_ASSERT(ba.size() == sizeof(quint32));

    quint32 value = *reinterpret_cast<const quint32 *>(ba.data());
    DataTypes::BitStringStruct resp { sigAddr, value, DataTypes::Quality::Good };
    DataManager::GetInstance().addSignalToOutList(resp);
}

#ifdef Q_OS_LINUX
void ProtocomThread::handleUnixTime(const QByteArray &ba, [[maybe_unused]] quint16 sigAddr)
{
    Q_ASSERT(ba.size() == sizeof(quint64));

    const quint32 secs = *reinterpret_cast<const quint32 *>(ba.data());
    const quint32 nsecs = *reinterpret_cast<const quint32 *>(ba.data() + sizeof(quint32));
    timespec resp;
    resp.tv_nsec = nsecs;
    resp.tv_sec = secs;
    DataManager::GetInstance().addSignalToOutList(resp);
}
#endif

template <std::size_t N>
void ProtocomThread::handleBitStringArray(const QByteArray &ba, std::array<quint16, N> arr_addr)
{
    Q_ASSERT(ba.size() / 4 == arr_addr.size());
    for (int i = 0; i != arr_addr.size(); i++)
    {
        QByteArray temp = ba.mid(sizeof(qint32) * i, sizeof(qint32));
        handleBitString(temp, arr_addr.at(i));
    }
}

void ProtocomThread::handleBitStringArray(const QByteArray &ba, quint16 start_addr)
{
    Q_ASSERT(ba.size() % sizeof(quint32) == 0);
    for (int i = 0; i != (ba.size() / sizeof(quint32)); i++)
    {
        QByteArray temp = ba.mid(sizeof(qint32) * i, sizeof(qint32));
        handleBitString(temp, start_addr + i);
    }
}

void ProtocomThread::handleFloat(const QByteArray &ba, quint32 sigAddr)
{
    Q_ASSERT(ba.size() == 4);
    float blk = *reinterpret_cast<const float *>(ba.data());
    DataTypes::FloatStruct resp { sigAddr, blk, DataTypes::Quality::Good };
    DataManager::GetInstance().addSignalToOutList(resp);
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
        DataTypes::SinglePointWithTimeStruct data { (addr + i), value, 0, DataTypes::Quality::Good };
        DataManager::GetInstance().addSignalToOutList(data);
    }
}

void ProtocomThread::handleFile(QByteArray &ba, DataTypes::FilesEnum addr, DataTypes::FileFormat format)
{
}

void ProtocomThread::handleInt(const byte num)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, num };
    DataManager::GetInstance().addSignalToOutList(resp);
}

void ProtocomThread::handleOk()
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, 0 };
    DataManager::GetInstance().addSignalToOutList(resp);
}

void ProtocomThread::handleError(int errorCode)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Error, static_cast<quint64>(errorCode) };
    DataManager::GetInstance().addSignalToOutList(resp);
    // Module error code
    qCritical() << "Error code: " << QString::number(errorCode, 16);
}

void ProtocomThread::handleProgress(const quint64 count)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataCount, count };
    DataManager::GetInstance().addSignalToOutList(resp);
}

void ProtocomThread::handleMaxProgress(const quint64 count)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataSize, count };
    DataManager::GetInstance().addSignalToOutList(resp);
}

void ProtocomThread::handleRawBlock(const QByteArray &ba, quint32 blkNum)
{
    DataTypes::BlockStruct resp { blkNum, ba };
    DataManager::GetInstance().addSignalToOutList(resp);
}

void ProtocomThread::handleCommand(const QByteArray &ba)
{
    qCritical("We shouldn't be here, something went wrong");
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
            DataManager::GetInstance().addSignalToOutList(oscInfo);
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
            DataManager::GetInstance().addSignalToOutList(swjInfo);
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

void ProtocomThread::processReadBytes(QByteArray &ba)
{
    QMutexLocker locker(&_mutex);
#ifdef PROTOCOM_DEBUG
    qDebug("Start parse response");
#endif

    // if there's no standard header
    if (ba.size() < 4)
    {
        qCritical() << Error::HeaderSizeError << ba.toHex();
        return;
    }

    // "command" that is received from device
    m_responseReceived = Proto::Commands(ba.at(1));

    // copy size from bytearray
    quint16 size;
    std::copy(&ba.constData()[2], &ba.constData()[3], &size);
    switch (ba.front())
    {
    // only Response we should receive from device
    case Proto::Response:
    {
        ba.remove(0, 4);
        ba.resize(size);
        m_readData.append(ba);

        // На команду ReadBSI модуль не отдает пустой ответ
        if (isOneSegment(size) || (m_responseReceived == Proto::ReadBlkStartInfo))
        {
            parseResponse(); // all data has been received, parse it
            // Progress for big files
            if (m_currentCommand.command == Commands::C_ReqFile)
            {
                if (isFirstBlock)
                    handleMaxProgress(S2::GetFileSize(ba)); // set progressbar max size
                m_progress += size;
                handleProgress(m_progress); // set current progressbar position
            }
            isFirstBlock = true; // prepare bool for the next receive iteration
            return;
        }
        else
        {
            auto tba = prepareOk(false, m_responseReceived); // prepare "Ok" answer to the device
            Q_ASSERT(tba.size() == 4);
            // Progress for big files
            if (m_currentCommand.command == Commands::C_ReqFile)
            {
                if (isFirstBlock)
                    handleMaxProgress(S2::GetFileSize(ba)); // set progressbar max size
                m_progress += Proto::MaxSegmenthLength;
                handleProgress(m_progress); // set current progressbar position
            }
            emit writeDataAttempt(tba); // write "Ok" to the device
        }
        isFirstBlock = false; // there'll be another segment
        break;
    }
    default:
        qCritical() << Error::WrongCommandError;
        break;
    }
}
