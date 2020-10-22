#include "datamanager.h"

#include "files.h"

QList<DataManager::SignalsStruct> DataManager::s_outputList;
QMutex DataManager::s_outListMutex;
QMutex DataManager::s_inQueueMutex;
QQueue<QVariant> s_inputQueue;

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
                    s_outListMutex.unlock();
                    return Error::Msg::NoError;
                }
            }
        }
    }
    s_outListMutex.unlock();
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

Error::Msg DataManager::getResponse(DataTypes::GeneralResponseStruct &response)
{
    s_outListMutex.lock();
    foreach (SignalsStruct sig, s_outputList)
    {
        if (sig.type == SignalTypes::GeneralResponse)
        {
            if (sig.data.canConvert<DataTypes::GeneralResponseStruct>())
            {
                response = qvariant_cast<DataTypes::GeneralResponseStruct>(sig.data);
                s_outputList.removeOne(sig);
                s_outListMutex.unlock();
                return Error::Msg::NoError;
            }
        }
    }
    s_outListMutex.unlock();
    return Error::Msg::ResEmpty;
}

void DataManager::setConfig(S2ConfigType *s2config)
{
    Queries::Command104 inp;
    inp.ba.resize(65535);
    S2::StoreDataMem(&inp.ba.data()[0], s2config, 0x0001);
    quint32 basize = static_cast<quint8>(inp.ba.data()[4]);
    basize += static_cast<quint8>(inp.ba.data()[5]) * 256;
    basize += static_cast<quint8>(inp.ba.data()[6]) * 65536;
    basize += static_cast<quint8>(inp.ba.data()[7]) * 16777216;
    basize += sizeof(S2DataTypes::FileHeader); // baHeader
    inp.ba.resize(basize);
    inp.cmd = Queries::Commands104::CM104_FILEREADY;
    addToInQueue(inp);
}

void DataManager::reqStartup()
{
    Queries::Command104 inp { Queries::Commands104::CM104_CORREADREQUEST, 0, 0, {} };
    DataManager::addToInQueue(inp);
}

Error::Msg DataManager::deQueue104(Queries::Command104 &cmd)
{
    if (!s_inputQueue.isEmpty())
    {
        s_inQueueMutex.lock();
        QVariant inp = s_inputQueue.first();
        if (inp.canConvert<Queries::Command104>())
        {
            s_inputQueue.dequeue();
            s_inQueueMutex.unlock();
            cmd = qvariant_cast<Queries::Command104>(inp);
            return Error::Msg::NoError;
        }
        s_inQueueMutex.unlock();
    }
    return Error::Msg::ResEmpty;
}

Error::Msg DataManager::deQueueMBS(Queries::CommandMBS &cmd)
{
    if (!s_inputQueue.isEmpty())
    {
        s_inQueueMutex.lock();
        QVariant inp = s_inputQueue.first();
        if (inp.canConvert<Queries::CommandMBS>())
        {
            s_inputQueue.dequeue();
            s_inQueueMutex.unlock();
            cmd = qvariant_cast<Queries::CommandMBS>(inp);
            return Error::Msg::NoError;
        }
        s_inQueueMutex.unlock();
    }
    return Error::Msg::ResEmpty;
}
