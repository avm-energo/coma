#ifndef BASEPROTOCOLPARSER_H
#define BASEPROTOCOLPARSER_H

#include <QObject>
#include <QWaitCondition>
#include <gen/datatypes.h>
#include <interfaces/connection.h>
#include <s2/filestruct.h>

namespace Interface
{

class BaseProtocolParser : public QObject
{
    Q_OBJECT
protected:
    using FileFormat = DataTypes::FileFormat;
    static const QMap<Interface::Commands, CommandRegisters> WSCommandMap;

    bool m_isCommandRequested = false;
    bool m_parsingDataReady = false;
    quint64 m_progress = 0;
    QMutex m_mutex;
    QByteArray m_readData;
    QWaitCondition m_waiter;
    LogClass m_log;
    std::atomic<State> m_state;
    RequestQueue &m_queue;

public:
    CommandStruct m_currentCommand;

    explicit BaseProtocolParser(RequestQueue &queue, QObject *parent = nullptr);

    State getState() const;
    void setState(const State newState);
    void clear();
    void wakeUp();

    quint16 blockByReg(const quint32 regAddr);
    void FilePostpone(QByteArray &ba, S2::FilesEnum addr, DataTypes::FileFormat format);
    void checkQueue();
    void finishCommand();
    virtual void parseRequest(const CommandStruct &cmdStr) = 0;

    void setProgressCount(const quint64 count);
    void setProgressRange(const quint64 count);

signals:
    void finished();
    void responseSend(const Interface::DeviceResponse &resp);
    void sendDataToPort(const QByteArray &ba);

public slots:
    void run();
    virtual void processReadBytes(QByteArray ba) = 0;
    virtual void parseResponse() = 0;
};

}

#endif // BASEPROTOCOLPARSER_H
