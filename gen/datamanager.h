#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "error.h"
#include <QMutex>
#include <QObject>
#include <QVariant>

namespace DataTypes
{
struct BitString
{
    //    quint8 sigAdr[3];
    quint32 sigAdr;
    quint32 sigVal;
    quint8 sigQuality;
}; // первое - номера сигналов, второе - их значения ("" ~ недостоверное
   // значение), третье - метка времени

struct FloatWithTime
{
    quint32 sigAdr;
    float sigVal;
    quint8 sigQuality;
    quint64 CP56Time;
};

struct Float
{
    quint32 sigAdr;
    float sigVal;
};

struct SinglePointWithTime
{
    quint32 sigAdr;
    quint8 sigVal;
    quint64 CP56Time;
};

struct File
{
    quint32 filenum;
    QByteArray filedata;
};

struct ConfParameter
{
    quint32 ID;
    QByteArray data;
};
}

Q_DECLARE_METATYPE(DataTypes::BitString)
Q_DECLARE_METATYPE(DataTypes::FloatWithTime)
Q_DECLARE_METATYPE(DataTypes::SinglePointWithTime)
Q_DECLARE_METATYPE(DataTypes::File)
Q_DECLARE_METATYPE(DataTypes::ConfParameter)

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

    enum CommandsEnum
    {
        CM104_INTERROGATETIMEGR15,
        CM104_COM51WRITETIME,
        CM104_COM45,
        CM104_COM50,
        CM104_CORREADREQUEST,
        CM104_SELECTFILE,
        CM104_FILEREADY
    };

    struct CommandsArgs
    {
        quint32 uintarg;
        float flarg;
        void *ptrarg;
    };

    struct InputStruct
    {
        CommandsEnum cmd;
        CommandsArgs args;
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

    template <typename T> void addSignalToQueue(SignalTypes type, T signal)
    {
        SignalsStruct str;
        str.type = type;
        str.data.setValue(signal);
        s_outQueueMutex.lock();
        s_outputList.append(str);
        s_outQueueMutex.unlock();
    }
    static QQueue<InputStruct> *s_inputQueue;
    static QList<SignalsStruct> s_outputList;
    static QMutex s_outQueueMutex;
    static QMutex s_inQueueMutex;

signals:
};

#endif // DATAMANAGER_H
