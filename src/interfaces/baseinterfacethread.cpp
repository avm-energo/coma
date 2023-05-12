#include "baseinterfacethread.h"

#include "../s2/s2.h"

#include <gen/datamanager/typesproxy.h>
#include <gen/files.h>

using namespace Interface;

BaseInterfaceThread::BaseInterfaceThread(QObject *parent) : QObject(parent), log(new LogClass(this))
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

void BaseInterfaceThread::processFileFromDisk(DataTypes::FilesEnum fileNum)
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
    FilePostpone(ba, fileNum, DataTypes::FileFormat::Binary);
}

void BaseInterfaceThread::FilePostpone(QByteArray &ba, DataTypes::FilesEnum addr, DataTypes::FileFormat format)
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
    log->Init(QString(metaObject()->className()) + ".log");
    log->info(logStart);
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
    emit finished();
}
