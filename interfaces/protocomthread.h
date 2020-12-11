#pragma once

#include "../gen/datatypes.h"
#include "../gen/error.h"
#include "protocomprivate.h"

#include <QMutex>
#include <QReadWriteLock>
#include <QWaitCondition>
class LogClass;
class ProtocomThread : public QObject
{
    Q_OBJECT
public:
    explicit ProtocomThread(QObject *parent = nullptr);
    ~ProtocomThread();

    void setReadDataChunk(const QByteArray &readDataChunk);
    void appendReadDataChunk(const QByteArray &readDataChunk);
    void wakeUp();

    void parse();

    void clear();

private:
    // QByteArray InData, OutData;
    QByteArray /*m_writeData,*/ m_readData;
    bool isCommandRequested = false;
    quint64 progress = 0;
    void finish(Error::Msg msg);

    void parseResponse(QByteArray ba);
    void parseRequest(const Proto::CommandStruct &cmdStr);
    void handle(const Proto::Commands cmd);

    // LogClass *log;

    QMutex _mutex;
    // QMutex _mutex;
    QWaitCondition _waiter;
    void writeLog(QByteArray ba, Proto::Direction dir = Proto::NoDirection);
    void writeLog(Error::Msg msg, Proto::Direction dir = Proto::NoDirection)
    {
        writeLog(QVariant::fromValue(msg).toByteArray(), dir);
    }

    Proto::CommandStruct m_currentCommand;
    QPair<quint64, QByteArray> m_buffer;
    void checkQueue();

    void fileHelper(DataTypes::FilesEnum fileNum);

signals:
    void writeDataAttempt(const QByteArray);
    void errorOccurred(Error::Msg);
    /// Like a QIODevice::readyRead()
    void readyRead();
};
