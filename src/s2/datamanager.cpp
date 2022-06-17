#include "datamanager.h"

#include "../gen/helper.h"

QMutex DataManager::s_inQueueMutex;
std::queue<QVariant> DataManager::s_inputQueue;

DataManager::DataManager(Singleton::token, QObject *parent) : QObject(parent)
{
}

void DataManager::checkTypeAndSendSignals(DataTypes::SignalsStruct &str)
{
    using namespace DataTypes;
    emit dataReceived(str);
    switch (str.type)
    {
    case BitString:
    {
        Q_ASSERT(str.data.canConvert<BitStringStruct>());
        if (str.data.canConvert<BitStringStruct>())
        {
            BitStringStruct bs = str.data.value<BitStringStruct>();
            insertRegister(bs.sigAdr, bs);
            emit bitStringReceived(bs);
        }
        break;
    }
    case SinglePointWithTime:
    {
        Q_ASSERT(str.data.canConvert<SinglePointWithTimeStruct>());
        if (str.data.canConvert<SinglePointWithTimeStruct>())
        {
            SinglePointWithTimeStruct sp = str.data.value<SinglePointWithTimeStruct>();
            insertRegister(sp.sigAdr, sp);
            emit singlePointReceived(sp);
        }
        break;
    }
    case Float:
    {
        Q_ASSERT(str.data.canConvert<FloatStruct>());
        if (str.data.canConvert<FloatStruct>())
        {
            FloatStruct fl = str.data.value<FloatStruct>();
            insertRegister(fl.sigAdr, fl);
            emit floatReceived(fl);
        }
        break;
    }
    case FloatWithTime:
    {
        Q_ASSERT(str.data.canConvert<FloatWithTimeStruct>());
        if (str.data.canConvert<FloatWithTimeStruct>())
        {
            FloatWithTimeStruct flt = str.data.value<FloatWithTimeStruct>();
            FloatStruct fl;
            fl.sigAdr = flt.sigAdr;
            fl.sigVal = flt.sigVal;
            insertRegister(fl.sigAdr, fl);
            emit floatReceived(fl);
        }
        break;
    }
    case File:
    {
        Q_ASSERT(str.data.canConvert<FileStruct>());
        if (str.data.canConvert<FileStruct>())
        {
            FileStruct fl = str.data.value<FileStruct>();
            emit fileReceived(fl);
        }
        break;
    }
    case DataRecVList:
    {
        Q_ASSERT(str.data.canConvert<QList<DataRecV>>());
        if (str.data.canConvert<QList<DataRecV>>())
        {
            const auto list = str.data.value<QList<DataRecV>>();
            emit dataRecVListReceived(list);
        }
        break;
    }
    case Block:
    {
        Q_ASSERT(str.data.canConvert<BlockStruct>());
        if (str.data.canConvert<BlockStruct>())
        {
            BlockStruct bs = str.data.value<BlockStruct>();
            emit blockReceived(bs);
        }
        break;
    }
    case GeneralResponse:
    {
        Q_ASSERT(str.data.canConvert<GeneralResponseStruct>());
        if (str.data.canConvert<GeneralResponseStruct>())
        {
            GeneralResponseStruct gr = str.data.value<GeneralResponseStruct>();
            emit responseReceived(gr);
        }
        break;
    }
    case OscillogramInfo:
    {
        Q_ASSERT(str.data.canConvert<S2DataTypes::OscInfo>());
        if (str.data.canConvert<S2DataTypes::OscInfo>())
        {
            S2DataTypes::OscInfo oscInfo = str.data.value<S2DataTypes::OscInfo>();
            emit oscInfoReceived(oscInfo);
        }
        break;
    }
    case SwitchJournalInfo:
    {
        Q_ASSERT(str.data.canConvert<S2DataTypes::SwitchJourInfo>());
        if (str.data.canConvert<S2DataTypes::SwitchJourInfo>())
        {
            S2DataTypes::SwitchJourInfo swjInfo = str.data.value<S2DataTypes::SwitchJourInfo>();
            emit swjInfoReceived(swjInfo);
        }
        break;
    }
#ifdef __linux__
    case Timespec:
    {
        Q_ASSERT(str.data.canConvert<timespec>());
        if (str.data.canConvert<timespec>())
        {
            timespec time = str.data.value<timespec>();
            emit timeReceived(time);
        }
        break;
    }
#endif
    default:
        break;
    }
}
