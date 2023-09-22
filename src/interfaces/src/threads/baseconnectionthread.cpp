#include "interfaces/threads/baseconnectionthread.h"

#include <QCoreApplication>
#include <interfaces/utils/typesproxy.h>
#include <s2/s2util.h>
#include <thread>

using namespace Interface;

const QMap<Interface::Commands, CommandRegisters> BaseConnectionThread::WSCommandMap {
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

BaseConnectionThread::BaseConnectionThread(RequestQueue &queue, QObject *parent)
    : QObject(parent), m_log(new LogClass(this)), m_queue(queue)
{
}

void BaseConnectionThread::clear()
{
    // QMutexLocker locker(&_mutex);
    m_progress = 0;
    m_currentCommand = CommandStruct();
    finishCommand();
}

void BaseConnectionThread::finishCommand()
{
    m_isCommandRequested = false;
    m_readData.clear();
    wakeUp();
}

void BaseConnectionThread::wakeUp()
{
    m_waiter.wakeOne();
}

quint16 BaseConnectionThread::blockByReg(const quint32 regAddr)
{
    return BaseConnection::iface()->settings()->dictionary().value(regAddr).block.value<quint16>();
}

void BaseConnectionThread::FilePostpone(QByteArray &ba, S2::FilesEnum addr, DataTypes::FileFormat format)
{
    // auto &manager = DataManager::GetInstance();
    switch (format)
    {
    case FileFormat::Binary:
    {
        DataTypes::GeneralResponseStruct genResp {
            DataTypes::GeneralResponseTypes::Ok, //
            static_cast<quint64>(ba.size())      //
        };
        // manager.addSignalToOutList(genResp);
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
                {
                    // manager.addSignalToOutList(s2bFile);
                    emit responseSend(s2bFile);
                }
            }
            break;
        }
        default:
        {
            S2::FileStruct resp { addr, ba };
            // manager.addSignalToOutList(resp);
            emit responseSend(resp);
            break;
        }
        }
        break;
    }
    case FileFormat::DefaultS2:
    {
        // manager.addSignalToOutList(ba);
        emit responseSend(ba);
        break;
    }
    case FileFormat::CustomS2:
    {
        DataTypes::S2FilePack outlist;
        if (!S2Util::RestoreData(ba, outlist))
        {
            DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Error,
                static_cast<quint64>(ba.size()) };
            // manager.addSignalToOutList(resp);
            emit responseSend(resp);
            return;
        }
        DataTypes::GeneralResponseStruct genResp { DataTypes::GeneralResponseTypes::Ok,
            static_cast<quint64>(ba.size()) };
        // manager.addSignalToOutList(genResp);
        emit responseSend(genResp);
        for (auto &&file : outlist)
        {
            S2::FileStruct resp { S2::FilesEnum(file.ID), file.data };
            // manager.addSignalToOutList(resp);
            emit responseSend(resp);
        }
        break;
    }
    }
}

void BaseConnectionThread::checkQueue()
{
    auto opt = m_queue.deQueue();
    if (opt.has_value())
    {
        m_isCommandRequested = true;
        m_progress = 0;
        m_currentCommand = opt.value();
        parseRequest(opt.value());
    }
}

void BaseConnectionThread::run()
{
    auto classname = QString(metaObject()->className()) + ".log";
    if (classname.contains("::"))
        classname = classname.split("::").last();
    m_log->Init(classname);
    m_log->info(logStart);
    while (BaseConnection::iface()->state() != State::Disconnect)
    {
        QMutexLocker locker(&m_mutex);
        if (!m_isCommandRequested)
            checkQueue();
        m_waiter.wait(&m_mutex, 100);
        if (m_parsingDataReady)
        {
            parseResponse();
            m_readData.clear();
            m_parsingDataReady = false;
        }
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    emit finished();
}

void BaseConnectionThread::setProgressCount(const quint64 count)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataCount, count };
    // DataManager::GetInstance().addSignalToOutList(resp);
    emit responseSend(resp);
}

void BaseConnectionThread::setProgressRange(const quint64 count)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataSize, count };
    // DataManager::GetInstance().addSignalToOutList(resp);
    emit responseSend(resp);
}