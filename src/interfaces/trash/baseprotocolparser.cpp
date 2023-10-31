#include "interfaces/parsers/baseprotocolparser.h"

#include <QCoreApplication>
#include <s2/s2util.h>
#include <thread>

using namespace Interface;

const QMap<Interface::Commands, CommandRegisters> BaseProtocolParser::WSCommandMap {
    { Commands::C_StartWorkingChannel, StartWorkingChannel },
    { Commands::C_SetStartupValues, SetStartupValues },
    { Commands::C_SetStartupPhaseA, SetStartupPhaseA },
    { Commands::C_SetStartupPhaseB, SetStartupPhaseB },
    { Commands::C_SetStartupPhaseC, SetStartupPhaseC },
    { Commands::C_SetStartupUnbounced, SetStartupUnbounced },
    { Commands::C_SetTransOff, SetTransOff },
    { Commands::C_ClearStartupValues, ClearStartupValues },
    { Commands::C_ClearStartupUnbounced, ClearStartupUnbounced },
    { Commands::C_ClearStartupError, ClearStartupSetError },
};

BaseProtocolParser::BaseProtocolParser(RequestQueue &queue, QObject *parent)
    : QObject(parent), m_state(ParserState::Starting), m_queue(queue)
{
}

ParserState BaseProtocolParser::getState() const noexcept
{
    return m_state.load();
}

void BaseProtocolParser::setState(const ParserState newState) noexcept
{
    if (newState != getState())
    {
        m_state.store(newState);
        emit stateChanged(newState);
    }
}

void BaseProtocolParser::clear()
{
    m_progress = 0;
    m_currentCommand = CommandStruct();
    finishCommand();
}

void BaseProtocolParser::finishCommand()
{
    // m_isCommandRequested = false;
    m_readData.clear();
    wakeUp();
}

void BaseProtocolParser::wakeUp()
{
    m_waiter.wakeOne();
}

void BaseProtocolParser::activate() noexcept
{
    setState(ParserState::RequestParsing);
}

void BaseProtocolParser::filePostpone(QByteArray &ba, S2::FilesEnum addr, DataTypes::FileFormat format)
{
    switch (format)
    {
    case FileFormat::Binary:
    {
        DataTypes::GeneralResponseStruct genResp {
            DataTypes::GeneralResponseTypes::Ok, //
            static_cast<quint64>(ba.size())      //
        };
        emit responseSend(genResp);
        switch (addr)
        {
        // По модбасу уже получили файл в формате S2B
        case S2::FilesEnum::JourSys:
        case S2::FilesEnum::JourWork:
        case S2::FilesEnum::JourMeas:
        {
            if (!ba.isEmpty())
            {
                S2Util util;
                S2::S2BFile s2bFile {};
                auto errCode = util.parseS2B(ba, s2bFile);
                if (errCode == Error::Msg::NoError)
                    emit responseSend(s2bFile);
                else
                    qCritical() << QVariant::fromValue(errCode).toString();
            }
            break;
        }
        default:
        {
            S2::FileStruct resp { addr, ba };
            emit responseSend(resp);
            break;
        }
        }
        break;
    }
    case FileFormat::DefaultS2:
    {
        emit responseSend(ba);
        break;
    }
    case FileFormat::CustomS2:
    {
        DataTypes::S2FilePack outlist;
        if (!S2Util::RestoreData(ba, outlist))
        {
            DataTypes::GeneralResponseStruct resp {
                DataTypes::GeneralResponseTypes::Error, //
                static_cast<quint64>(ba.size())         //
            };
            emit responseSend(resp);
            return;
        }
        DataTypes::GeneralResponseStruct genResp {
            DataTypes::GeneralResponseTypes::Ok, //
            static_cast<quint64>(ba.size())      //
        };
        emit responseSend(genResp);
        for (auto &&file : outlist)
        {
            S2::FileStruct resp { S2::FilesEnum(file.ID), file.data };
            emit responseSend(resp);
        }
        break;
    }
    }
}

void BaseProtocolParser::checkQueue() noexcept
{
    auto opt = m_queue.getFromQueue();
    if (opt.has_value())
    {
        // m_isCommandRequested = true;
        m_progress = 0;
        m_currentCommand = opt.value();
        setState(ParserState::CommandRequested);
        // TODO: эта функция должна возвращать QByteArray, который мы отсюда отправляем интерфейсу
        parseRequest(m_currentCommand);
        // TODO: тут эмитим сигнал sendDataToInterface
    }
}

void BaseProtocolParser::run()
{
    auto classname = QString(metaObject()->className()) + ".log";
    if (classname.contains("::"))
        classname = classname.split("::").last();
    m_log.init(classname);
    m_log.info(logStart);

    auto currentState = getState();
    while (currentState != ParserState::Stopping)
    {
        switch (currentState)
        {
        case ParserState::RequestParsing:
            checkQueue();
            break;
        case ParserState::ResponseParsing:
            parseResponse(); // парсим ответ
            m_readData.clear();
            setState(ParserState::RequestParsing);
            break;
        case ParserState::CommandRequested:
            // Ничего не делаем, ждём изменения состояния
            break;
        case ParserState::Pending:
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            currentState = getState();
            continue;
        default:
            break;
        }
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        currentState = getState();
    }

    //    while (getState() != State::Disconnect)
    //    {
    //        // Если интерфейс находится в состоянии реконнекта, то ничего не отправляем
    //        if (getState() == State::Reconnect)
    //        {
    //            QCoreApplication::processEvents(QEventLoop::AllEvents);
    //            continue;
    //        }
    //        QMutexLocker locker(&m_mutex);
    //        if (!m_isCommandRequested)
    //            checkQueue();
    //        m_waiter.wait(&m_mutex, 100);
    //        if (m_parsingDataReady)
    //        {
    //            parseResponse();
    //            m_readData.clear();
    //            m_parsingDataReady = false;
    //        }
    //        QCoreApplication::processEvents(QEventLoop::AllEvents);
    //    }
    emit finished();
}

void BaseProtocolParser::setProgressCount(const quint64 count)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataCount, count };
    emit responseSend(resp);
}

void BaseProtocolParser::setProgressRange(const quint64 count)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataSize, count };
    emit responseSend(resp);
}

void BaseProtocolParser::processOk()
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, 0 };
    emit responseSend(resp);
}

void BaseProtocolParser::processError(int errorCode)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Error, static_cast<quint64>(errorCode) };
    emit responseSend(resp);
    qCritical() << "Device error code: " << QString::number(errorCode, 16);
}
