#include "datamanager.h"

// QList<DataTypes::SignalsStruct> DataManager::s_outputList;
// QMutex DataManager::s_outListMutex;
QMutex DataManager::s_inQueueMutex;
std::queue<QVariant> DataManager::s_inputQueue;

DataManager::DataManager(Singleton::token, QObject *parent) : QObject(parent)
{
}

// Error::Msg DataManager::getSignals(quint32 firstSignalAdr, quint32 lastSignalAdr, DataTypes::SignalTypes type,
//    QList<DataTypes::SignalsStruct> &outlist)
//{
//    s_outListMutex.lock();
//    if (s_outputList.isEmpty())
//    {
//        s_outListMutex.unlock();
//        return Error::Msg::ResEmpty;
//    }
//    foreach (DataTypes::SignalsStruct sig, s_outputList)
//    {
//        if (sig.type == type)
//        {
//            QVariant signal = sig.data;
//            switch (type)
//            {
//            case DataTypes::SignalTypes::BitString:
//            {
//                if (signal.canConvert<DataTypes::BitStringStruct>())
//                {
//                    DataTypes::BitStringStruct bs = qvariant_cast<DataTypes::BitStringStruct>(signal);
//                    if ((bs.sigAdr >= firstSignalAdr) && (bs.sigAdr <= lastSignalAdr))
//                    {
//                        outlist.append(sig);
//                        s_outputList.removeAll(sig);
//                    }
//                }
//                break;
//            }
//            case DataTypes::SignalTypes::FloatWithTime:
//            {
//                if (signal.canConvert<DataTypes::FloatWithTimeStruct>())
//                {
//                    DataTypes::FloatWithTimeStruct fwt = qvariant_cast<DataTypes::FloatWithTimeStruct>(signal);
//                    if ((fwt.sigAdr >= firstSignalAdr) && (fwt.sigAdr <= lastSignalAdr))
//                    {
//                        outlist.append(sig);
//                        s_outputList.removeAll(sig);
//                    }
//                }
//                break;
//            }
//            case DataTypes::SignalTypes::Float:
//            {
//                if (signal.canConvert<DataTypes::FloatStruct>())
//                {
//                    DataTypes::FloatStruct fwt = qvariant_cast<DataTypes::FloatStruct>(signal);
//                    if ((fwt.sigAdr >= firstSignalAdr) && (fwt.sigAdr <= lastSignalAdr))
//                    {
//                        outlist.append(sig);
//                        s_outputList.removeAll(sig);
//                    }
//                }
//                break;
//            }
//            case DataTypes::SignalTypes::SinglePointWithTime:
//            {
//                if (signal.canConvert<DataTypes::SinglePointWithTimeStruct>())
//                {
//                    DataTypes::SinglePointWithTimeStruct sp
//                        = qvariant_cast<DataTypes::SinglePointWithTimeStruct>(signal);
//                    if ((sp.sigAdr >= firstSignalAdr) && (sp.sigAdr <= lastSignalAdr))
//                    {
//                        outlist.append(sig);
//                        s_outputList.removeAll(sig);
//                    }
//                }
//                break;
//            }
//            default:
//                break;
//            }
//        }
//    }
//    s_outListMutex.unlock();
//    if (outlist.isEmpty())
//        return Error::Msg::ResEmpty;
//    return Error::Msg::NoError;
//}

// Error::Msg DataManager::getFile(quint32 filenum, QByteArray &outba)
//{
//    s_outListMutex.lock();
//    if (s_outputList.isEmpty())
//    {
//        s_outListMutex.unlock();
//        return Error::Msg::ResEmpty;
//    }
//    foreach (DataTypes::SignalsStruct sig, s_outputList)
//    {
//        if (sig.type == DataTypes::SignalTypes::File)
//        {
//            if (sig.data.canConvert<DataTypes::FileStruct>())
//            {
//                DataTypes::FileStruct fl = qvariant_cast<DataTypes::FileStruct>(sig.data);
//                if (fl.filenum == filenum)
//                {
//                    outba = fl.filedata;
//                    s_outputList.removeAll(sig);
//                    s_outListMutex.unlock();
//                    return Error::Msg::NoError;
//                }
//            }
//        }
//    }
//    s_outListMutex.unlock();
//    return Error::Msg::ResEmpty;
//}

// Error::Msg DataManager::getConfig(quint32 firstID, quint32 lastID, QList<DataTypes::ConfParameterStruct> &outlist)
//{
//    QByteArray ba;
//    assert(firstID <= lastID);
//    // check needed IDs for exist in s_outputList
//    s_outListMutex.lock();
//    foreach (DataTypes::SignalsStruct sig, s_outputList)
//    {
//        if (sig.type == DataTypes::SignalTypes::ConfParameter)
//        {
//            if (sig.data.canConvert<DataTypes::ConfParameterStruct>())
//            {
//                DataTypes::ConfParameterStruct cfp = qvariant_cast<DataTypes::ConfParameterStruct>(sig.data);
//                if ((cfp.ID >= firstID) && (cfp.ID <= lastID))
//                {
//                    outlist.append(cfp);
//                    s_outputList.removeOne(sig);
//                }
//            }
//        }
//    }
//    s_outListMutex.unlock();
//    if (getFile(Files::Config, ba) != Error::Msg::ResEmpty)
//    {
//        if (S2::RestoreData(ba, outlist) == Error::Msg::NoError)
//        {
//            // check for needed IDs and move not mentioned into s_outputList
//            foreach (DataTypes::ConfParameterStruct cfp, outlist)
//            {
//                if ((cfp.ID < firstID) || (cfp.ID > lastID))
//                    addSignalToOutList(DataTypes::SignalTypes::ConfParameter, cfp);
//            }
//        }
//    }
//    return Error::Msg::ResEmpty;
//}

// Error::Msg DataManager::getResponse(DataTypes::GeneralResponseTypes type, DataTypes::GeneralResponseStruct &response)
//{
//    s_outListMutex.lock();
//    foreach (DataTypes::SignalsStruct sig, s_outputList)
//    {
//        if (sig.type == DataTypes::SignalTypes::GeneralResponse)
//        {
//            if (sig.data.canConvert<DataTypes::GeneralResponseStruct>())
//            {
//                response = qvariant_cast<DataTypes::GeneralResponseStruct>(sig.data);
//                if (response.type == type)
//                    s_outputList.removeOne(sig);
//                else
//                    continue;
//                s_outListMutex.unlock();
//                return Error::Msg::NoError;
//            }
//        }
//    }
//    s_outListMutex.unlock();
//    return Error::Msg::ResEmpty;
//}

void DataManager::checkTypeAndSendSignals(DataTypes::SignalsStruct &str)
{
    using namespace DataTypes;
    emit dataReceived(str);
    switch (str.type)
    {
    case BitString:
    {
        if (str.data.canConvert<BitStringStruct>())
        {
            BitStringStruct bs = str.data.value<BitStringStruct>();
            emit bitStringReceived(bs);
        }
        break;
    }
    case SinglePointWithTime:
    {
        if (str.data.canConvert<SinglePointWithTimeStruct>())
        {
            SinglePointWithTimeStruct sp = qvariant_cast<SinglePointWithTimeStruct>(str.data);
            emit singlePointReceived(sp);
        }
        break;
    }
    case Float:
    case FloatWithTime:
    {
        if (str.data.canConvert<FloatWithTimeStruct>())
        {
            FloatWithTimeStruct flt = qvariant_cast<FloatWithTimeStruct>(str.data);
            FloatStruct fl;
            fl.sigAdr = flt.sigAdr;
            fl.sigVal = flt.sigVal;
            emit floatReceived(fl);
        }
        else if (str.data.canConvert<FloatStruct>())
        {
            FloatStruct fl = qvariant_cast<FloatStruct>(str.data);
            emit floatReceived(fl);
        }
        break;
    }
    case File:
    {
        if (str.data.canConvert<FileStruct>())
        {
            FileStruct fl = qvariant_cast<FileStruct>(str.data);
            emit fileReceived(fl);
        }
        break;
    }
    case ConfParametersList:
    {
        if (str.data.canConvert<ConfParametersListStruct>())
        {
            ConfParametersListStruct cp = qvariant_cast<ConfParametersListStruct>(str.data);
            emit confParametersReceived(cp);
        }
        break;
    }
    case Block:
    {
        if (str.data.canConvert<BlockStruct>())
        {
            BlockStruct bs = qvariant_cast<BlockStruct>(str.data);
            emit blockReceived(bs);
        }
        break;
    }
    case GeneralResponse:
    {
        if (str.data.canConvert<GeneralResponseStruct>())
        {
            GeneralResponseStruct gr = qvariant_cast<GeneralResponseStruct>(str.data);
            emit responseReceived(gr);
        }
        break;
    }
    default:
        break;
    }
}

// void DataManager::setConfig(S2ConfigType *s2config)
//{
//    Queries::Command inp;
//    inp.ba.resize(65535);
//    S2::StoreDataMem(&inp.ba.data()[0], s2config, 0x0001);
//    quint32 basize = static_cast<quint8>(inp.ba.data()[4]);
//    basize += static_cast<quint8>(inp.ba.data()[5]) * 256;
//    basize += static_cast<quint8>(inp.ba.data()[6]) * 65536;
//    basize += static_cast<quint8>(inp.ba.data()[7]) * 16777216;
//    basize += sizeof(S2DataTypes::FileHeader); // baHeader
//    inp.ba.resize(basize);
//    inp.cmd = Queries::Commands::WRITEFILE;
//    addToInQueue(inp);
//}

// Error::Msg DataManager::deQueueMBS(Queries::CommandMBS &cmd)
//{
//    if (!s_inputQueue.isEmpty())
//    {
//        s_inQueueMutex.lock();
//        QVariant inp = s_inputQueue.first();
//        if (inp.canConvert<Queries::CommandMBS>())
//        {
//            s_inputQueue.dequeue();
//            s_inQueueMutex.unlock();
//            cmd = qvariant_cast<Queries::CommandMBS>(inp);
//            return Error::Msg::NoError;
//        }
//        s_inQueueMutex.unlock();
//    }
//    return Error::Msg::ResEmpty;
//}
