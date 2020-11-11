#pragma once

#include "../gen/error.h"
#include "defines.h"

#include <QReadWriteLock>
class LogClass;
class ProtocomThread : public QObject
{
    Q_OBJECT
public:
    explicit ProtocomThread(QObject *parent = nullptr);
    ~ProtocomThread();

    void setReadDataChunk(const QByteArray &readDataChunk);
    void appendReadDataChunk(const QByteArray &readDataChunk);

    void parse();

private:
    QByteArray InData, OutData;
    QByteArray m_writeData, m_readData;

    void finish(Error::Msg msg);

    void parseResponse(QByteArray ba);
    void parseRequest(const CommandStruct &cmdStr);
    void handle(const CN::Commands cmd);

    LogClass *log;

    QReadWriteLock m_rwLocker;
    void writeLog(QByteArray ba, Direction dir = NoDirection);
    void writeLog(Error::Msg msg, Direction dir = NoDirection)
    {
        writeLog(QVariant::fromValue(msg).toByteArray(), dir);
    }

    CommandStruct m_currentCommand;
    QPair<quint64, QByteArray> m_buffer;
    void checkQueue();

signals:
    void writeDataAttempt(const QByteArray);
    void errorOccurred(Error::Msg);
    /// Like a QIODevice::readyRead()
    void readyRead();
};
