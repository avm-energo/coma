#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "datatypes.h"
#include "error.h"
#include "singleton.h"

#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QVariant>
#include <queue>

#define INQUEUEMAXSIZE 100

class DataManager : public QObject, public Singleton<DataManager>
{
    Q_OBJECT
public:
    explicit DataManager(token, QObject *parent = nullptr);

    //    static Error::Msg getSignals(quint32 firstSignalAdr, quint32 lastSignalAdr, DataTypes::SignalTypes type,
    //        QList<DataTypes::SignalsStruct> &outlist);
    //    static Error::Msg getFile(quint32 filenum, QByteArray &outba);
    //    static Error::Msg getConfig(quint32 firstID, quint32 lastID, QList<DataTypes::ConfParameterStruct> &outlist);
    //    static Error::Msg getResponse(DataTypes::GeneralResponseTypes type, DataTypes::GeneralResponseStruct
    //    &response); static void setConfig(S2ConfigType *s2config);
    //    static void reqStartup();
    void checkTypeAndSendSignals(DataTypes::SignalsStruct &str);
    template <typename T> static void addSignalToOutList(DataTypes::SignalTypes type, T signal)
    {
        DataTypes::SignalsStruct str;
        str.type = type;
        str.data.setValue(signal);
        GetInstance().checkTypeAndSendSignals(str);
        //        s_outListMutex.lock();
        //        s_outputList.append(str);
        //        s_outListMutex.unlock();
    }
    template <typename T> static void addToInQueue(T data)
    {
        QVariant var;
        var.setValue(data);
        s_inQueueMutex.lock();
        s_inputQueue.push(var);
        s_inQueueMutex.unlock();
    }
    template <typename T> static Error::Msg deQueue(T &cmd)
    {
        if (!s_inputQueue.empty())
        {
            s_inQueueMutex.lock();
            QVariant inp = s_inputQueue.front();
            if (inp.canConvert<T>())
            {
                s_inputQueue.pop();
                s_inQueueMutex.unlock();
                cmd = qvariant_cast<T>(inp);
                return Error::Msg::NoError;
            }
            s_inQueueMutex.unlock();
        }
        return Error::Msg::ResEmpty;
    }

private:
    static std::queue<QVariant> s_inputQueue;
    //    static QList<DataTypes::SignalsStruct> s_outputList;
    //    static QMutex s_outListMutex;
    static QMutex s_inQueueMutex;

signals:
    void dataReceived(const DataTypes::SignalsStruct &);
    void bitStringReceived(const DataTypes::BitStringStruct &);
    void singlePointReceived(const DataTypes::SinglePointWithTimeStruct &);
    void floatReceived(const DataTypes::FloatStruct &);
    void fileReceived(const DataTypes::FileStruct &);
    void confParametersReceived(const DataTypes::ConfParametersListStruct &);
    void responseReceived(const DataTypes::GeneralResponseStruct &);
    void blockReceived(const DataTypes::BlockStruct &);
};

#endif // DATAMANAGER_H
