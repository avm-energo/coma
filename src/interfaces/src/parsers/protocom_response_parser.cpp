#include "interfaces/parsers/protocom_response_parser.h"

#include <gen/error.h>

namespace Interface
{

ProtocomResponseParser::ProtocomResponseParser(QObject *parent) : BaseResponseParser(parent)
{
}

bool ProtocomResponseParser::isValid(const QByteArray &response)
{
    // if there's no standard header
    if (response.size() >= 4)
    {
        // parsing protocom header
        auto startByte = Proto::Starters(response.at(0)); // start byte
        auto size = quint16(response.at(2));              // size of data
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
        qCritical() << Error::HeaderSizeError << response.toHex();
    return false;
}

void ProtocomResponseParser::parse(QByteArray &response)
{
    // Убираем заголовок Protocom, подготовка
    m_receivedCommand = Proto::Commands(response.at(1));
    auto size = quint16(response.at(2));
    response.remove(0, 4);
    response.resize(size);

    quint32 addr = m_request.arg1.toUInt();
    switch (m_receivedCommand)
    {
    case Proto::Commands::ResultOk:
    {
        /// TODO
        //        if (m_request.command == Commands::C_WriteFile)
        //            setProgressCount(m_sentBytesCount);
        //        if (!m_longBlockChunks.isEmpty())
        //        {
        //            QByteArray ba = m_longBlockChunks.takeFirst();
        //            m_sentBytesCount += ba.size();
        //            emit sendDataToInterface(ba);
        //            return;
        //        }
        processOk();
        break;
    }
    case Proto::Commands::ResultError:
    {
        const quint8 errorCode = response.front();
        processError(errorCode);
        break;
    }
    case Proto::Commands::ReadTime:
#ifdef Q_OS_LINUX
        if (response.size() == sizeof(quint64))
            processUnixTime(m_readData);
        else
#endif
            processU32(response, addr);
        break;
    case Proto::Commands::ReadBlkStartInfo:
    case Proto::Commands::ReadBlkStartInfoExt:
        if (boardType.isEmpty())
        {
            boardType.mTypeB = response[0];
            boardType.mTypeM = response[4];
        }
        processU32(response, addr);
        break;
    case Proto::Commands::ReadBlkAC:
    case Proto::Commands::ReadBlkDataA:
        processBlock(response, addr); // Ожидается что в addr хранится номер блока
        break;
    case Proto::Commands::ReadBlkData:
        switch (m_request.command)
        {
        case Commands::C_ReqStartup:
        case Commands::C_ReqFloats:
            processFloat(response, addr);
            break;
        case Commands::C_ReqAlarms:
            processSinglePoint(response, addr);
            break;
        case Commands::C_ReqBitStrings:
            processU32(response, addr);
            break;
        default:
            processBlock(response, addr);
            break;
        }
        break;
    case Proto::Commands::ReadBlkTech:
        processTechBlock(response, addr);
        break;
    case Proto::Commands::ReadProgress:
        processU32(response, addr);
        break;
    case Proto::Commands::ReadFile:
        /// TODO
        // filePostpone(response, S2::FilesEnum(addr), DataTypes::FileFormat(m_request.arg2.toUInt()));
        break;
    case Proto::Commands::ReadMode:
        processInt(response.toInt());
        break;
    default:
        qCritical("We shouldn't be here, something went wrong");
        qDebug() << response.toHex();
        break;
    }
}

#ifdef Q_OS_LINUX
void ProtocomResponseParser::processUnixTime(const QByteArray &ba)
{
    Q_ASSERT(ba.size() == sizeof(quint64));
    const quint32 secs = *reinterpret_cast<const quint32 *>(ba.data());
    const quint32 nsecs = *reinterpret_cast<const quint32 *>(ba.data() + sizeof(quint32));
    timespec resp;
    resp.tv_nsec = nsecs;
    resp.tv_sec = secs;
    emit responseParsed(resp);
}
#endif

void ProtocomResponseParser::processU32(const QByteArray &data, quint16 startAddr)
{
    Q_ASSERT(data.size() % sizeof(quint32) == 0);
    Q_ASSERT(data.size() >= 4);
    for (int i = 0; i != (data.size() / sizeof(quint32)); i++)
    {
        QByteArray tba = data.mid(sizeof(qint32) * i, sizeof(qint32));
        quint32 value = *reinterpret_cast<const quint32 *>(tba.data());
        DataTypes::BitStringStruct resp { startAddr++, value, DataTypes::Quality::Good };
        emit responseParsed(resp);
    }
}

void ProtocomResponseParser::processFloat(const QByteArray &data, quint32 startAddr)
{
    // NOTE Проблема со стартовыми регистрами, получим на один регистр больше чем по другим протоколам
    Q_ASSERT(data.size() >= sizeof(float));     // должен быть хотя бы один флоат
    Q_ASSERT(data.size() % sizeof(float) == 0); // размер кратен размеру флоат
    const int baendpos = data.size();
    constexpr auto step = sizeof(float);
    for (int bapos = 0; bapos < baendpos; bapos += step)
    {
        QByteArray tba = data.mid(bapos, step);
        float blk = *reinterpret_cast<const float *>(tba.data());
        DataTypes::FloatStruct resp { startAddr++, blk, DataTypes::Quality::Good };
        emit responseParsed(resp);
    }
}

void ProtocomResponseParser::processInt(const byte num)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, num };
    emit responseParsed(resp);
}

void ProtocomResponseParser::processSinglePoint(const QByteArray &data, const quint16 startAddr)
{
    for (quint32 i = 0; i != quint32(data.size()); ++i)
    {
        quint8 value = data.at(i);
        DataTypes::SinglePointWithTimeStruct data { (startAddr + i), value, 0, DataTypes::Quality::Good };
        emit responseParsed(data);
    }
}

void ProtocomResponseParser::processBlock(const QByteArray &data, quint32 blockNum)
{
    DataTypes::BlockStruct resp { blockNum, data };
    emit responseParsed(resp);
}

void ProtocomResponseParser::processTechBlock(const QByteArray &data, quint32 blockNum)
{
    switch (blockNum)
    {
    case TechBlocks::T_Oscillogram:
    {
        qDebug("Блок наличия осциллограмм Bo");
        Q_ASSERT(data.size() % sizeof(S2::OscInfo) == 0);
        constexpr auto step = sizeof(S2::OscInfo);
        for (int i = 0; i != data.size(); i += step)
        {
            QByteArray buffer = data.mid(i, step);
            S2::OscInfo oscInfo;
            memcpy(&oscInfo, buffer.constData(), step);
            emit responseParsed(oscInfo);
        }
        break;
    }
    case TechBlocks::T_GeneralEvent:
    {
        qDebug("Блок текущих событий Be");
        break;
    }
    case TechBlocks::T_TechEvent:
    {
        qDebug("Блок технологических событий BTe");
        break;
    }
    case TechBlocks::T_SwitchJournal:
    {
        qDebug("Блок наличия журналов переключения");
        Q_ASSERT(data.size() % sizeof(S2::SwitchJourInfo) == 0);
        constexpr auto step = sizeof(S2::SwitchJourInfo);
        for (int i = 0; i != data.size(); i += step)
        {
            QByteArray buffer = data.mid(i, step);
            S2::SwitchJourInfo swjInfo;
            memcpy(&swjInfo, buffer.constData(), step);
            emit responseParsed(swjInfo);
        }
        break;
    }
    case TechBlocks::T_WorkArchive:
    {
        qDebug("Блок рабочего архива (Bra)");
        break;
    }
    default:
        qDebug() << data;
        break;
    }
}

} // namespace Interface
