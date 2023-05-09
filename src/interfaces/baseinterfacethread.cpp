#include "baseinterfacethread.h"

#include "../s2/s2.h"

#include <gen/datamanager/typesproxy.h>

using namespace Interface;

BaseInterfaceThread::BaseInterfaceThread(QObject *parent) : QObject(parent), m_currentCommand({})
{
}

void BaseInterfaceThread::clear()
{
    QMutexLocker locker(&_mutex);
    m_readData.clear();
    m_isCommandRequested = false;
    m_progress = 0;
    m_currentCommand = CommandStruct();
}

void BaseInterfaceThread::wakeUp()
{
    _waiter.wakeOne();
}

void BaseInterfaceThread::FilePostpone(QByteArray &ba, DataTypes::FilesEnum addr, Datatypes::FileFormat format)
{
    switch (format)
    {
    case FileFormat::Binary:
    {
        DataTypes::GeneralResponseStruct genResp { DataTypes::GeneralResponseTypes::Ok,
            static_cast<quint64>(ba.size()) };
        DataManager::GetInstance().addSignalToOutList(genResp);
        DataTypes::FileStruct resp { addr, ba };
        DataManager::GetInstance().addSignalToOutList(resp);
        break;
    }
    case FileFormat::DefaultS2:
    {
        QList<DataTypes::DataRecV> outlistV;

        if (!S2::RestoreData(ba, outlistV))
        {
            DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Error,
                static_cast<quint64>(ba.size()) };
            DataManager::GetInstance().addSignalToOutList(resp);
            return;
        }
        DataTypes::GeneralResponseStruct genResp { DataTypes::GeneralResponseTypes::Ok,
            static_cast<quint64>(ba.size()) };
        DataManager::GetInstance().addSignalToOutList(genResp);
        DataManager::GetInstance().addSignalToOutList(outlistV);
        break;
    }
    case FileFormat::CustomS2:
    {
        DataTypes::S2FilePack outlist;
        if (!S2::RestoreData(ba, outlist))
        {
            DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Error,
                static_cast<quint64>(ba.size()) };
            DataManager::GetInstance().addSignalToOutList(resp);
            return;
        }
        DataTypes::GeneralResponseStruct genResp { DataTypes::GeneralResponseTypes::Ok,
            static_cast<quint64>(ba.size()) };
        DataManager::GetInstance().addSignalToOutList(genResp);
        for (auto &&file : outlist)
        {
            DataTypes::FileStruct resp { DataTypes::FilesEnum(file.ID), file.data };
            DataManager::GetInstance().addSignalToOutList(resp);
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
    while (BaseInterface::iface()->state() != State::Stop)
    {
        QMutexLocker locker(&_mutex);
        if (!m_isCommandRequested)
            checkQueue();
        _waiter.wait(&_mutex);
        if (!m_readData.isEmpty())
        {
            parseResponse();
            m_readData.clear();
        }
    }
}
