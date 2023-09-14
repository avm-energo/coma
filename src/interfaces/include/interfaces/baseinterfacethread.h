#ifndef BASEINTERFACETHREAD_H
#define BASEINTERFACETHREAD_H

#include <QObject>
#include <QWaitCondition>
#include <gen/datatypes.h>
#include <interfaces/baseinterface.h>
#include <s2/filestruct.h>

namespace Interface
{

class BaseInterfaceThread : public QObject
{
    Q_OBJECT
public:
    explicit BaseInterfaceThread(QObject *parent = nullptr);

    void clear();
    void wakeUp();

    quint16 blockByReg(const quint32 regAddr)
    {
        return BaseInterface::iface()->settings()->dictionary().value(regAddr).block.value<quint16>();
    }

    void FilePostpone(QByteArray &ba, S2::FilesEnum addr, DataTypes::FileFormat format);
    void checkQueue();
    void finishCommand();
    virtual void parseRequest(const CommandStruct &cmdStr) = 0;

    bool m_isCommandRequested = false;
    bool m_parsingDataReady = false;
    quint64 m_progress = 0;
    CommandStruct m_currentCommand;
    QMutex _mutex;
    QByteArray m_readData;
    QWaitCondition _waiter;
    UniquePointer<LogClass> m_log;
    const QMap<Interface::Commands, CommandRegisters> WSCommandMap {
        { C_StartWorkingChannel, StartWorkingChannel },
        { C_SetStartupValues, SetStartupValues },
        { C_SetStartupPhaseA, SetStartupPhaseA },
        { C_SetStartupPhaseB, SetStartupPhaseB },
        { C_SetStartupPhaseC, SetStartupPhaseC },
        { C_SetStartupUnbounced, SetStartupUnbounced },
        { C_SetTransOff, SetTransOff },
        { C_ClearStartupValues, ClearStartupValues },
        { C_ClearStartupUnbounced, ClearStartupUnbounced },
        { C_ClearStartupError, ClearStartupSetError },
    };

    void setProgressCount(const quint64 count);
    void setProgressRange(const quint64 count);

protected:
    using FileFormat = DataTypes::FileFormat;

signals:
    void finished();
    void sendDataToPort(const QByteArray &ba);

public slots:
    void run();
    virtual void processReadBytes(QByteArray ba) = 0;
    virtual void parseResponse() = 0;
};

}

#endif // BASEINTERFACETHREAD_H
