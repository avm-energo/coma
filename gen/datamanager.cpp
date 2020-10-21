#include "datamanager.h"
#include "files.h"
#include "s2.h"

QList<DataManager::SignalsStruct> DataManager::s_outputList;
QMutex DataManager::s_outListMutex;
QMutex DataManager::s_inQueueMutex;
QQueue<DataTypes::Command> s_inputQueue;

DataManager::DataManager(QObject *parent) : QObject(parent) { }

Error::Msg DataManager::getSignals(
    quint32 firstSignalAdr, quint32 lastSignalAdr, SignalTypes type, QList<SignalsStruct> &outlist)
{
    s_outListMutex.lock();
    if (s_outputList.isEmpty())
    {
        s_outListMutex.unlock();
        return Error::Msg::ResEmpty;
    }
    foreach (SignalsStruct sig, s_outputList)
    {
        if (sig.type == type)
        {
            QVariant signal = sig.data;
            switch (type)
            {
            case SignalTypes::BitString:
            {
                if (signal.canConvert<DataTypes::BitString>())
                {
                    DataTypes::BitString bs = qvariant_cast<DataTypes::BitString>(signal);
                    if ((bs.sigAdr >= firstSignalAdr) && (bs.sigAdr <= lastSignalAdr))
                        outlist.removeAll(sig);
                }
                break;
            }
            case SignalTypes::FloatWithTime:
            {
                if (signal.canConvert<DataTypes::FloatWithTime>())
                {
                    DataTypes::FloatWithTime fwt = qvariant_cast<DataTypes::FloatWithTime>(signal);
                    if ((fwt.sigAdr >= firstSignalAdr) && (fwt.sigAdr <= lastSignalAdr))
                        outlist.removeAll(sig);
                }
                break;
            }
            case SignalTypes::SinglePointWithTime:
            {
                if (signal.canConvert<DataTypes::SinglePointWithTime>())
                {
                    DataTypes::SinglePointWithTime sp = qvariant_cast<DataTypes::SinglePointWithTime>(signal);
                    if ((sp.sigAdr >= firstSignalAdr) && (sp.sigAdr <= lastSignalAdr))
                        outlist.removeAll(sig);
                }
                break;
            }
            default:
                break;
            }
        }
    }
    s_outListMutex.unlock();
    if (outlist.isEmpty())
        return Error::Msg::ResEmpty;
    return Error::Msg::NoError;
}

Error::Msg DataManager::getFile(quint32 filenum, QByteArray &outba)
{
    s_outListMutex.lock();
    if (s_outputList.isEmpty())
    {
        s_outListMutex.unlock();
        return Error::Msg::ResEmpty;
    }
    foreach (SignalsStruct sig, s_outputList)
    {
        if (sig.type == SignalTypes::File)
        {
            if (sig.data.canConvert<DataTypes::File>())
            {
                DataTypes::File fl = qvariant_cast<DataTypes::File>(sig.data);
                if (fl.filenum == filenum)
                {
                    outba = fl.filedata;
                    return Error::Msg::NoError;
                }
            }
        }
    }
    return Error::Msg::ResEmpty;
}

Error::Msg DataManager::getConfig(quint32 firstID, quint32 lastID, QList<DataTypes::ConfParameter> &outlist)
{
    QByteArray ba;
    assert(firstID <= lastID);
    // check needed IDs for exist in s_outputList
    s_outListMutex.lock();
    foreach (SignalsStruct sig, s_outputList)
    {
        if (sig.type == SignalTypes::ConfParameter)
        {
            if (sig.data.canConvert<DataTypes::ConfParameter>())
            {
                DataTypes::ConfParameter cfp = qvariant_cast<DataTypes::ConfParameter>(sig.data);
                if ((cfp.ID >= firstID) && (cfp.ID <= lastID))
                {
                    outlist.append(cfp);
                    s_outputList.removeOne(sig);
                }
            }
        }
    }
    s_outListMutex.unlock();
    if (getFile(Files::Config, ba) != Error::Msg::ResEmpty)
    {
        if (S2::RestoreData(ba, outlist) == Error::Msg::NoError)
        {
            // check for needed IDs and move not mentioned into s_outputList
            foreach (DataTypes::ConfParameter cfp, outlist)
            {
                if ((cfp.ID < firstID) || (cfp.ID > lastID))
                    addSignalToOutList(SignalTypes::ConfParameter, cfp);
            }
        }
    }
    return Error::Msg::ResEmpty;
}
