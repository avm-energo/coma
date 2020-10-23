#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "datatypes.h"
#include "error.h"
#include "s2.h"

#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QVariant>

class DataManager : public QObject
{
    Q_OBJECT
public:
    explicit DataManager(QObject *parent = nullptr);

    static Error::Msg getSignals(quint32 firstSignalAdr, quint32 lastSignalAdr, DataTypes::SignalTypes type,
        QList<DataTypes::SignalsStruct> &outlist);
    static Error::Msg getFile(quint32 filenum, QByteArray &outba);
    static Error::Msg getConfig(quint32 firstID, quint32 lastID, QList<DataTypes::ConfParameterStruct> &outlist);
    static Error::Msg getResponse(DataTypes::GeneralResponseTypes type, DataTypes::GeneralResponseStruct &response);
    static void setConfig(S2ConfigType *s2config);
    static void reqStartup();
    template <typename T> static void addSignalToOutList(DataTypes::SignalTypes type, T signal)
    {
        DataTypes::SignalsStruct str;
        str.type = type;
        str.data.setValue(signal);
        s_outListMutex.lock();
        s_outputList.append(str);
        s_outListMutex.unlock();
    }
    template <typename T> static void addToInQueue(T data)
    {
        QVariant var;
        var.setValue(data);
        s_inQueueMutex.lock();
        s_inputQueue.enqueue(var);
        s_inQueueMutex.unlock();
    }
    static Error::Msg deQueue(Queries::Command &cmd);
    //    static Error::Msg deQueueMBS(Queries::CommandMBS &cmd);
    static QQueue<QVariant> s_inputQueue;
    static QList<DataTypes::SignalsStruct> s_outputList;
    static QMutex s_outListMutex;
    static QMutex s_inQueueMutex;

signals:
};

#endif // DATAMANAGER_H
