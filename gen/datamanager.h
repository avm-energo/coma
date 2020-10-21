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
    enum SignalTypes
    {
        BitString,
        FloatWithTime,
        SinglePointWithTime,
        ByteArray,
        File,
        ConfParameter
    };

    struct SignalsStruct
    {
        SignalTypes type;
        QVariant data;
    };

    explicit DataManager(QObject *parent = nullptr);

    static Error::Msg getSignals(quint32 firstSignalAdr, quint32 lastSignalAdr, DataManager::SignalTypes type,
        QList<DataManager::SignalsStruct> &outlist);
    static Error::Msg getFile(quint32 filenum, QByteArray &outba);
    static Error::Msg getConfig(quint32 firstID, quint32 lastID, QList<DataTypes::ConfParameter> &outlist);
    static void setConfig(S2ConfigType *s2config);
    template <typename T> static void addSignalToOutList(SignalTypes type, T signal)
    {
        SignalsStruct str;
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
    static Error::Msg deQueue104(Queries::Command104 &cmd);
    static Error::Msg deQueueMBS(Queries::CommandMBS &cmd);
    static QQueue<QVariant> s_inputQueue;
    static QList<SignalsStruct> s_outputList;
    static QMutex s_outListMutex;
    static QMutex s_inQueueMutex;

signals:
};

#endif // DATAMANAGER_H
