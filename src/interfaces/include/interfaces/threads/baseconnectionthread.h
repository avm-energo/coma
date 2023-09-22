#ifndef BASECONNECTIONTHREAD_H
#define BASECONNECTIONTHREAD_H

#include <QObject>
#include <QWaitCondition>
#include <gen/datatypes.h>
#include <interfaces/conn/baseconnection.h>
#include <s2/filestruct.h>

namespace Interface
{

class BaseConnectionThread : public QObject
{
    Q_OBJECT
protected:
    static const QMap<Interface::Commands, CommandRegisters> WSCommandMap;

    bool m_isCommandRequested = false;
    bool m_parsingDataReady = false;
    quint64 m_progress = 0;
    QMutex m_mutex;
    QByteArray m_readData;
    QWaitCondition m_waiter;
    UniquePointer<LogClass> m_log;
    RequestQueue &m_queue;

public:
    CommandStruct m_currentCommand;

    explicit BaseConnectionThread(RequestQueue &queue, QObject *parent = nullptr);

    void clear();
    void wakeUp();

    quint16 blockByReg(const quint32 regAddr);
    void FilePostpone(QByteArray &ba, S2::FilesEnum addr, DataTypes::FileFormat format);
    void checkQueue();
    void finishCommand();
    virtual void parseRequest(const CommandStruct &cmdStr) = 0;

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

#endif // BASECONNECTIONTHREAD_H
