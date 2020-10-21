#include "datamanager.h"
#include "files.h"
#include "s2.h"

QList<DataManager::SignalsStruct> DataManager::s_outputList;
QMutex DataManager::s_outQueueMutex;
QMutex DataManager::s_inQueueMutex;
static QQueue<DataManager::InputStruct> *s_inputQueue;

DataManager::DataManager(QObject *parent) : QObject(parent) { }

Error::Msg DataManager::getSignals(
    quint32 firstSignalAdr, quint32 lastSignalAdr, SignalTypes type, QList<SignalsStruct> &outlist)
{
    s_outQueueMutex.lock();
    if (s_outputList.isEmpty())
    {
        s_outQueueMutex.unlock();
        return Error::Msg::ResEmpty;
    }
    for (int i = 0; i < s_outputList.size(); ++i)
    {
        if (s_outputList.at(i).type == type)
        {
            QVariant signal = s_outputList.at(i).data;
            switch (type)
            {
            case SignalTypes::BitString:
            {
                DataTypes::BitString bs = qvariant_cast<DataTypes::BitString>(signal);
                if ((bs.sigAdr >= firstSignalAdr) && (bs.sigAdr <= lastSignalAdr))
                    outlist.append(s_outputList.takeAt(i));
                break;
            }
            case SignalTypes::FloatWithTime:
            {
                DataTypes::FloatWithTime fwt = qvariant_cast<DataTypes::FloatWithTime>(signal);
                if ((fwt.sigAdr >= firstSignalAdr) && (fwt.sigAdr <= lastSignalAdr))
                    outlist.append(s_outputList.takeAt(i));
                break;
            }
            case SignalTypes::SinglePointWithTime:
            {
                DataTypes::SinglePointWithTime sp = qvariant_cast<DataTypes::SinglePointWithTime>(signal);
                if ((sp.sigAdr >= firstSignalAdr) && (sp.sigAdr <= lastSignalAdr))
                    outlist.append(s_outputList.takeAt(i));
                break;
            }
            case SignalTypes::ByteArray:
            {
                DataTypes::File ba = qvariant_cast<DataTypes::File>(signal);
                if (ba.filenum == firstSignalAdr)
                    outlist.append(s_outputList.takeAt(i));
            }
            default:
                break;
            }
        }
    }
    s_outQueueMutex.unlock();
    if (outlist.isEmpty())
        return Error::Msg::ResEmpty;
    return Error::Msg::NoError;
}

Error::Msg DataManager::getFile(quint32 filenum, QByteArray &outba)
{
    s_outQueueMutex.lock();
    if (s_outputList.isEmpty())
    {
        s_outQueueMutex.unlock();
        return Error::Msg::ResEmpty;
    }
    for (int i = 0; i < s_outputList.size(); ++i)
    {
        if (s_outputList.at(i).type == SignalTypes::File)
        {
            DataTypes::File fl = qvariant_cast<DataTypes::File>(s_outputList.at(i).data);
            if (fl.filenum == filenum)
            {
                outba = fl.filedata;
                return Error::Msg::NoError;
            }
        }
    }
    return Error::Msg::ResEmpty;
}

Error::Msg DataManager::getConfig(quint32 firstID, quint32 lastID, QList<DataTypes::ConfParameter> &outlist)
{
    QByteArray ba;
    if (getFile(Files::Config, ba) != Error::Msg::ResEmpty)
    {
        if (S2::RestoreData(ba, outlist) == Error::Msg::NoError)
        {
            // check for needed IDs and move not mentioned into s_outputList
            foreach (DataTypes::ConfParameter cfp, outlist)
            {
                if ((cfp.ID < firstID) || (cfp.ID > lastID))
                    addSignalToQueue(SignalTypes::ConfParameter, cfp);
            }
        }
    }
    return Error::Msg::ResEmpty;
}
