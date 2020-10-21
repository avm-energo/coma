#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "error.h"
#include <QMutex>
#include <QObject>
#include <QQueue>
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

namespace Queries
{
enum Commands104
{
    CM104_INTERROGATETIMEGR15,
    CM104_COM51WRITETIME,
    CM104_COM45,
    CM104_COM50,
    CM104_CORREADREQUEST,
    CM104_SELECTFILE,
    CM104_FILEREADY
};
enum CommandsMBS
{
    MBS_READHOLDINGREGISTERS = 0x03,
    MBS_READINPUTREGISTER = 0x04,
    MBS_WRITEMULTIPLEREGISTERS = 0x10
};

struct Command104
{
    Commands104 cmd;
    QList<QVariant> args;
};
struct CommandMBS
{
    CommandsMBS command;
    quint16 address;
    quint16 quantity;
    quint8 sizeBytes;
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

    template <typename T> static void addSignalToOutList(SignalTypes type, T signal)
    {
        SignalsStruct str;
        str.type = type;
        str.data.setValue(signal);
        s_outListMutex.lock();
        s_outputList.append(str);
        s_outListMutex.unlock();
    }
    template <typename T> static void addToInQueue(DataTypes::Commands cmd, QList<QVariant> args)
    {
        DataTypes::Command outp;
        outp.cmd = cmd;
        outp.args = args;
        s_inQueueMutex.lock();
        s_inputQueue.enqueue(outp);
        s_inQueueMutex.unlock();
    }
    static QQueue<DataTypes::Command> s_inputQueue;
    static QList<SignalsStruct> s_outputList;
    static QMutex s_outListMutex;
    static QMutex s_inQueueMutex;

signals:
};

#endif // DATAMANAGER_H
