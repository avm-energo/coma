#include "protocomthread.h"

#include "../s2/s2.h"

#include <QDebug>
#include <QQueue>

#ifdef Q_OS_LINUX
#include <time.h>
#endif

// typedef QQueue<QByteArray> ByteQueue;
using DataTypes::FileFormat;
using Interface::Direction;
using Proto::CommandStruct;
using Proto::Starters;
using namespace Interface;

ProtocomThread::ProtocomThread(QObject *parent) : BaseInterfaceThread(parent)
{
    isFirstBlock = true;
    m_longBlockChunks.clear();
}

ProtocomThread::~ProtocomThread()
{
}

void ProtocomThread::processReadBytes(QByteArray ba)
{
    QMutexLocker locker(&_mutex);
    if (!isValidIncomingData(ba))
    {
        finishCommand();
        return;
    }

    m_responseReceived = Proto::Commands(ba.at(1)); // received from device "command"
    auto size = quint16(ba.at(2));
    writeLog(ba, Interface::FromDevice);
    ba.remove(0, 4);
    ba.resize(size);
    m_readData.append(ba);

    // Если ответе меньше 60 байт или пришёл BSI
    if (isOneSegment(size) || (m_responseReceived == Proto::ReadBlkStartInfo))
    {
        m_parsingDataReady = true;
        progressFile(ba);    // Progress for big files
        isFirstBlock = true; // prepare bool for the next receive iteration
        wakeUp();
    }
    // Пришло 60 байт
    else
    {
        auto tba = prepareOk(false, m_responseReceived); // prepare "Ok" answer to the device
        Q_ASSERT(tba.size() == 4);
        progressFile(ba);         // Progress for big files
        isFirstBlock = false;     // there'll be another segment
        emit sendDataToPort(tba); // write "Ok" to the device
    }
}

void ProtocomThread::parseRequest(const CommandStruct &cmdStr)
{
    QByteArray ba;
#ifdef PROTOCOM_DEBUG
    qDebug("Start parse request");
#endif

    switch (cmdStr.command)
    {
        // commands requesting regs with addresses ("fake" read regs commands)
    case Commands::C_ReqAlarms:
    case Commands::C_ReqStartup:
    case Commands::C_ReqFloats:
    case Commands::C_ReqBitStrings:
    {
        quint8 block = blockByReg(cmdStr.arg1.toUInt());
        ba = prepareBlock(Proto::Commands::ReadBlkData, StdFunc::ArrayFromNumber(block));
        emit sendDataToPort(ba);
        break;
    }
        // commands without any arguments
    case Commands::C_ReqBSI:
    case Commands::C_ReqBSIExt:
    case Commands::C_ReqTime:
    case Commands::C_StartFirmwareUpgrade:
    case Commands::C_ReqProgress:
    case Commands::C_GetMode:
    {
        if (protoCommandMap.contains(cmdStr.command))
        {
            ba = prepareBlock(protoCommandMap.value(cmdStr.command));
            emit sendDataToPort(ba);
        }
        break;
    }
        // commands with only one uint8 parameter (blocknum or smth similar)
    case Commands::C_EraseTechBlock:
    case Commands::C_EraseJournals:
    case Commands::C_SetMode:
    case Commands::C_Reboot:
    case Commands::C_ReqTuningCoef:
    case Commands::C_ReqBlkData:
    case Commands::C_ReqBlkDataA:
    case Commands::C_ReqBlkDataTech:
    case Commands::C_ReqOscInfo:
    case Commands::C_Test:
    {
        if (protoCommandMap.contains(cmdStr.command))
        {
            ba = prepareBlock(
                protoCommandMap.value(cmdStr.command), StdFunc::ArrayFromNumber(cmdStr.arg1.value<quint8>()));
            emit sendDataToPort(ba);
        }
        break;
    }

        // file request: known file types should be download from disk and others must be taken from module by Protocom,
        // arg1 - file number
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
            ba = prepareBlock(Proto::Commands::ReadFile, StdFunc::ArrayFromNumber(cmdStr.arg1.value<quint16>()));
            emit sendDataToPort(ba);
            break;
        }
        break;
    }

        // commands with one bytearray argument arg2
    case Commands::C_WriteFile:
    {
        if (cmdStr.arg2.canConvert<QByteArray>())
        {
            ba = cmdStr.arg2.value<QByteArray>();
            writeBlock(Proto::Commands::WriteFile, ba);
        }
        break;
    }

        // write time command with different behaviour under different OS's
    case Commands::C_WriteTime:
    {
        QByteArray tba;
#ifdef Q_OS_LINUX
        if (cmdStr.arg1.canConvert<timespec>())
        {
            timespec time = cmdStr.arg1.value<timespec>();
            tba.push_back(StdFunc::ArrayFromNumber(quint32(time.tv_sec)));
            tba.push_back(StdFunc::ArrayFromNumber(quint32(time.tv_nsec)));
        }
        else
#endif
        {
            tba = StdFunc::ArrayFromNumber(cmdStr.arg1.value<quint32>());
        }
        ba = prepareBlock(Proto::Commands::WriteTime, tba);
        emit sendDataToPort(ba);
        break;
    }

        // block write, arg1 is BlockStruct of one quint32 (block ID) and one QByteArray (block contents)
    case Commands::C_WriteHardware:
    case Commands::C_WriteBlkDataTech:
    case Commands::C_SetNewConfiguration:
    case Commands::C_WriteTuningCoef:
    {
        if (cmdStr.arg1.canConvert<DataTypes::BlockStruct>())
        {
            DataTypes::BlockStruct bs = cmdStr.arg1.value<DataTypes::BlockStruct>();
            ba = StdFunc::ArrayFromNumber(static_cast<quint8>(bs.ID)); // сужающий каст
            ba.append(bs.data);
            writeBlock(protoCommandMap.value(cmdStr.command), ba);
        }
        break;
    }

        // QVariantList write
    case Commands::C_WriteUserValues:
    {
        if (cmdStr.arg1.canConvert<QVariantList>())
        {
            QVariantList vl = cmdStr.arg1.value<QVariantList>();
            const quint16 start_addr = vl.first().value<DataTypes::FloatStruct>().sigAdr;
            const auto blockNum = static_cast<quint8>(blockByReg(start_addr)); // сужающий каст
            ba = StdFunc::ArrayFromNumber(blockNum);
            for (const auto &i : vl)
            {
                const float value = i.value<DataTypes::FloatStruct>().sigVal;
                ba.append(StdFunc::ArrayFromNumber(value));
            }
            writeBlock(protoCommandMap.value(cmdStr.command), ba);
        }
        break;
    }

    // WS Commands
    case Commands::C_WriteSingleCommand:
    {
        if (cmdStr.arg1.canConvert<DataTypes::SingleCommand>())
        {
            DataTypes::SingleCommand scmd = cmdStr.arg1.value<DataTypes::SingleCommand>();
            ba = StdFunc::ArrayFromNumber((scmd.addr)) + StdFunc::ArrayFromNumber(scmd.value);
            ba = prepareBlock(Proto::WriteSingleCommand, ba);
            emit sendDataToPort(ba);
        }
        break;
    }

        // "WS" commands
    case Commands::C_ClearStartupError:
    case Commands::C_ClearStartupUnbounced:
    case Commands::C_ClearStartupValues:
    case Commands::C_SetStartupValues:
    case Commands::C_SetStartupPhaseA:
    case Commands::C_SetStartupPhaseB:
    case Commands::C_SetStartupPhaseC:
    case Commands::C_SetStartupUnbounced:
    case Commands::C_StartWorkingChannel:
    case Commands::C_SetTransOff:
    {
        ba = StdFunc::ArrayFromNumber(static_cast<uint24>(WSCommandMap[cmdStr.command]));
        ba.append(StdFunc::ArrayFromNumber(cmdStr.arg1.value<quint8>()));
        ba = prepareBlock(Proto::WriteSingleCommand, ba);
        emit sendDataToPort(ba);
        break;
    }

    default:
        qDebug() << "There's no such command";
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
    {
        if (m_currentCommand.command == C_WriteFile)
            setProgressCount(m_sentBytesCount);
        if (!m_longBlockChunks.isEmpty())
        {
            QByteArray ba = m_longBlockChunks.takeFirst();
            m_sentBytesCount += ba.size();
            emit sendDataToPort(ba);
            _waiter.wakeOne();
            return;
        }
        processOk();
        break;
    }

    case ResultError:
    {
        const quint8 errorCode = m_readData.front();
        processError(errorCode);
        break;
    }

    case ReadTime:
#ifdef Q_OS_LINUX
        if (m_readData.size() == sizeof(quint64))
        {
            processUnixTime(m_readData);
            break;
        }
#endif
        processU32(m_readData, addr);
        break;

    case ReadBlkStartInfo:
    case ReadBlkStartInfoExt:
        processU32(m_readData, addr);
        break;

    case ReadBlkAC:
    case ReadBlkDataA:
        // Ожидается что в addr хранится номер блока
        processBlock(m_readData, addr);
        break;

    case ReadBlkData:
        switch (m_currentCommand.command)
        {
        case C_ReqStartup:
        case C_ReqFloats:
            processFloat(m_readData, addr);
            break;
        case C_ReqAlarms:
            processSinglePoint(m_readData, addr);
            break;
        case C_ReqBitStrings:
            processU32(m_readData, addr);
            break;
        default:
            processBlock(m_readData, addr);
            break;
        }
        break;

    case ReadBlkTech:
        processTechBlock(m_readData, addr);
        break;

    case ReadProgress:
        processU32(m_readData, addr);
        break;

    case ReadFile:
        FilePostpone(m_readData, FilesEnum(addr), FileFormat(count));
        break;

    case ReadMode:
        processInt(m_readData.toInt());
        break;

    default:
        qCritical("We shouldn't be here, something went wrong");
        qDebug() << m_readData.toHex();
        break;
    }
    finishCommand();
}

void ProtocomThread::writeLog(const QByteArray &ba, Direction dir)
{
#ifdef PROTOCOM_DEBUG
    QString msg = metaObject()->className();
    switch (dir)
    {
    case Interface::FromDevice:
        msg += ": <- ";
        break;
    case Interface::ToDevice:
        msg += ": -> ";
        break;
    default:
        msg += ": ";
        break;
    }
    msg += ba.toHex();
    m_log->info(msg);
#else
    Q_UNUSED(ba);
    Q_UNUSED(dir);
#endif
}

void ProtocomThread::appendInt16(QByteArray &ba, quint16 data)
{
    ba.append(static_cast<char>(data % 0x100));
    ba.append(static_cast<char>(data / 0x100));
}

bool ProtocomThread::isOneSegment(quint16 length)
{
    // Если размер меньше MaxSegmenthLength то сегмент считается последним (единственным)
    Q_ASSERT(length <= Proto::MaxSegmenthLength);
    return (length != Proto::MaxSegmenthLength);
}

bool ProtocomThread::isSplitted(quint16 length)
{
    return !(length < Proto::MaxSegmenthLength);
}

bool ProtocomThread::isValidIncomingData(const QByteArray &data)
{
    // if there's no standard header
    if (data.size() >= 4)
    {
        // parsing protocom header
        auto startByte = Proto::Starters(data.at(0)); // start byte
        auto size = quint16(data.at(2));              // size of data
        // only response should be received from device
        if (startByte == Proto::Starters::Response)
        {
            // checking size limits
            if (size <= Proto::MaxSegmenthLength)
                return true;
            else
                qCritical() << Error::SizeError << size;
        }
        else
            qCritical() << Error::WrongCommandError << startByte;
    }
    else
        qCritical() << Error::HeaderSizeError << data.toHex();

    return false;
}

void ProtocomThread::progressFile(const QByteArray &data)
{
    // Progress for big files
    if (m_currentCommand.command == Commands::C_ReqFile)
    {
        if (isFirstBlock)
            setProgressRange(S2::GetFileSize(data)); // set progressbar max size
        m_progress += data.size();
        setProgressCount(m_progress); // set current progressbar position
    }
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
    appendInt16(tmpba, 0x0000);
    return tmpba;
}

QByteArray ProtocomThread::prepareError()
{
    QByteArray tmpba;
    tmpba.append(Proto::Starters::Request);
    tmpba.append(Proto::Commands::ResultError);
    appendInt16(tmpba, 0x0001);
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

void ProtocomThread::writeBlock(Proto::Commands cmd, const QByteArray &arg2)
{
    using Proto::MaxSegmenthLength;
    QByteArray ba = arg2;

    if (isSplitted(ba.size()))
    {
        // prepareLongBlk
        m_longBlockChunks.clear();
        // Количество сегментов
        quint64 segCount = (ba.size() + 1) // +1 Т.к. некоторые команды имеют в значимой части один дополнительный байт
                / MaxSegmenthLength        // Максимальная длинна сегмента
            + 1; // Добавляем еще один сегмент в него попадет последняя часть
        m_longBlockChunks.reserve(segCount);

        QByteArray tba;
        tba.append(ba.left(MaxSegmenthLength));

        m_longBlockChunks.append(prepareBlock(cmd, tba));

        for (int pos = MaxSegmenthLength; pos < ba.size(); pos += MaxSegmenthLength)
        {
            tba = ba.mid(pos, MaxSegmenthLength);
            m_longBlockChunks.append(prepareBlock(cmd, tba, Proto::Starters::Continue));
        }
        setProgressRange(ba.size() + segCount * 4);
        m_sentBytesCount = m_longBlockChunks.at(0).size();
        emit sendDataToPort(m_longBlockChunks.takeFirst()); // send first chunk
    }
    else
    {
        ba = prepareBlock(cmd, ba);
        emit sendDataToPort(ba);
    }
}

#ifdef Q_OS_LINUX
void ProtocomThread::processUnixTime(const QByteArray &ba)
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

void ProtocomThread::processU32(const QByteArray &ba, quint16 startAddr)
{
    Q_ASSERT(ba.size() % sizeof(quint32) == 0);
    Q_ASSERT(ba.size() >= 4);
    for (int i = 0; i != (ba.size() / sizeof(quint32)); i++)
    {
        QByteArray tba = ba.mid(sizeof(qint32) * i, sizeof(qint32));
        quint32 value = *reinterpret_cast<const quint32 *>(tba.data());
        DataTypes::BitStringStruct resp { startAddr++, value, DataTypes::Quality::Good };
        DataManager::GetInstance().addSignalToOutList(resp);
    }
}

void ProtocomThread::processFloat(const QByteArray &ba, quint32 startAddr)
{
    // NOTE Проблема со стартовыми регистрами, получим на один регистр больше чем по другим протоколам
    Q_ASSERT(ba.size() >= sizeof(float));     // должен быть хотя бы один флоат
    Q_ASSERT(ba.size() % sizeof(float) == 0); // размер кратен размеру флоат
    int bapos = 0;
    const int baendpos = ba.size();
    while (bapos != baendpos)
    {
        QByteArray tba = ba.mid(bapos, sizeof(float));
        float blk = *reinterpret_cast<const float *>(tba.data());
        DataTypes::FloatStruct resp { startAddr++, blk, DataTypes::Quality::Good };
        DataManager::GetInstance().addSignalToOutList(resp);
        bapos += sizeof(float);
    }
}

void ProtocomThread::processSinglePoint(const QByteArray &ba, const quint16 startAddr)
{
    for (quint32 i = 0; i != quint32(ba.size()); ++i)
    {
        quint8 value = ba.at(i);
        DataTypes::SinglePointWithTimeStruct data { (startAddr + i), value, 0, DataTypes::Quality::Good };
        DataManager::GetInstance().addSignalToOutList(data);
    }
}

void ProtocomThread::processInt(const byte num)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, num };
    DataManager::GetInstance().addSignalToOutList(resp);
}

void ProtocomThread::processOk()
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, 0 };
    DataManager::GetInstance().addSignalToOutList(resp);
}

void ProtocomThread::processError(int errorCode)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Error, static_cast<quint64>(errorCode) };
    DataManager::GetInstance().addSignalToOutList(resp);
    // Module error code
    qCritical() << "Error code: " << QString::number(errorCode, 16);
}

void ProtocomThread::processBlock(const QByteArray &ba, quint32 blkNum)
{
    DataTypes::BlockStruct resp { blkNum, ba };
    DataManager::GetInstance().addSignalToOutList(resp);
}

void ProtocomThread::processTechBlock(const QByteArray &ba, quint32 blkNum)
{
    switch (blkNum)
    {
        //  Блок наличия осциллограмм Bo
    case T_Oscillogram:
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
    case T_GeneralEvent:
    {
        qDebug("Блок текущих событий Be");
        break;
    }
        // Блок технологических событий BTe
    case T_TechEvent:
    {
        qDebug("Блок технологических событий BTe");
        break;
    }
    case T_SwitchJournal:
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
    case T_WorkArchive:
    {
        qDebug("Блок рабочего архива (Bra)");
        break;
    }
    default:
        qDebug() << ba;
        break;
    }
}
