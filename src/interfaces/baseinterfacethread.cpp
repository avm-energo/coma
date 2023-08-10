#include "baseinterfacethread.h"

#include "../s2/s2util.h"

#include <QCoreApplication>
#include <gen/datamanager/typesproxy.h>
#include <thread>

using namespace Interface;

BaseInterfaceThread::BaseInterfaceThread(QObject *parent) : QObject(parent), m_log(new LogClass(this))
{
}

void BaseInterfaceThread::clear()
{
    // QMutexLocker locker(&_mutex);
    m_progress = 0;
    m_currentCommand = CommandStruct();
    finishCommand();
}

void BaseInterfaceThread::finishCommand()
{
    m_isCommandRequested = false;
    m_readData.clear();
    wakeUp();
}

void BaseInterfaceThread::wakeUp()
{
    _waiter.wakeOne();
}

void BaseInterfaceThread::FilePostpone(QByteArray &ba, S2::FilesEnum addr, DataTypes::FileFormat format)
{
    auto &manager = DataManager::GetInstance();
    switch (format)
    {
    case FileFormat::Binary:
    {
        DataTypes::GeneralResponseStruct genResp {
            DataTypes::GeneralResponseTypes::Ok, //
            static_cast<quint64>(ba.size())      //
        };
        manager.addSignalToOutList(genResp);
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
                    manager.addSignalToOutList(s2bFile);
            }
            break;
        }
        default:
        {
            S2::FileStruct resp { addr, ba };
            manager.addSignalToOutList(resp);
            break;
        }
        }
        break;
    }
    case FileFormat::DefaultS2:
    {
        manager.addSignalToOutList(ba);
        break;
    }
    case FileFormat::CustomS2:
    {
        DataTypes::S2FilePack outlist;
        if (!S2Util::RestoreData(ba, outlist))
        {
            DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Error,
                static_cast<quint64>(ba.size()) };
            manager.addSignalToOutList(resp);
            return;
        }
        DataTypes::GeneralResponseStruct genResp { DataTypes::GeneralResponseTypes::Ok,
            static_cast<quint64>(ba.size()) };
        manager.addSignalToOutList(genResp);
        for (auto &&file : outlist)
        {
            S2::FileStruct resp { S2::FilesEnum(file.ID), file.data };
            manager.addSignalToOutList(resp);
        }
        break;
    }
    }
}

void BaseInterfaceThread::checkQueue()
{
    CommandStruct inp;
    if (DataManager::GetInstance().deQueue(inp) != Error::Msg::NoError)
        return;

    m_isCommandRequested = true;
    m_progress = 0;
    m_currentCommand = inp;
    parseRequest(inp);
}

void BaseInterfaceThread::run()
{
    auto classname = QString(metaObject()->className()) + ".log";
    if (classname.contains("::"))
        classname = classname.split("::").last();
    m_log->Init(classname);
    m_log->info(logStart);
    while (BaseInterface::iface()->state() != State::Disconnect)
    {
        QMutexLocker locker(&_mutex);
        if (!m_isCommandRequested)
            checkQueue();
        _waiter.wait(&_mutex, 100);
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

void BaseInterfaceThread::setProgressCount(const quint64 count)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataCount, count };
    DataManager::GetInstance().addSignalToOutList(resp);
}

void BaseInterfaceThread::setProgressRange(const quint64 count)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataSize, count };
    DataManager::GetInstance().addSignalToOutList(resp);
}
