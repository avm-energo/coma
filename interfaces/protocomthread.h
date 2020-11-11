#pragma once

#include "../gen/logclass.h"
#include "usbhidport.h"

#include <QEventLoop>
#include <QReadWriteLock>
#include <QTimer>
class ProtocomThread : public QObject
{
    Q_OBJECT
public:
    explicit ProtocomThread(QObject *parent = nullptr);
    ~ProtocomThread();

    bool start(const int &devPos);
    void stop();

    void SendCmd(unsigned char command, int parameter = 0);
    // read
    void SendIn(unsigned char command, char parameter, QByteArray &ba, qint64 maxdatasize);
    // write
    void SendOut(unsigned char command, char board_type, QByteArray &ba);
    // write file
    void SendFile(unsigned char command, char board_type, int filenum, QByteArray &ba);

    Error::Msg result() const;
    void setResult(const Error::Msg &result);

    inline bool isWorkerRunning() const
    {
        return m_workerStatus;
    };
    inline bool isParserRunning() const
    {
        return m_parserStatus;
    };

    void setReadDataChunk(const QByteArray &readDataChunk);
    void appendReadDataChunk(const QByteArray &readDataChunk);

    void parse();

private:
    bool m_workerStatus, m_parserStatus;

    char BoardType;
    byte Command;
    quint8 bStep;
    Error::Msg m_result;
    int FNum;
    quint32 ReadDataChunkLength, RDLength; // длина всей посылки
    // quint32 WRLength;                      // длина всей посылки
    qint64 InDataSize;
    int SegLeft; // количество оставшихся сегментов
    int SegEnd;  // номер последнего байта в ReadData текущего сегмента
    int m_devicePosition;

    QByteArray InData, OutData;
    QByteArray m_writeData;

    void Send(char command, char parameter, QByteArray &ba, qint64 datasize);
    void InitiateSend();
    void WriteDataToPort(QByteArray &ba);
    void Finish(Error::Msg msg);
    void SetWRSegNum(quint32 WRLength);
    QByteArray WRCheckForNextSegment(int first);

    void OscTimerTimeout();
    void ParseIncomeData(QByteArray ba);

    void initiateReceive(QByteArray ba);
    void initiateSend(const CommandStruct &cmdStr);
    void handle(const CN::Commands cmd);

    LogClass *log;

    QReadWriteLock m_rwLocker;
    void writeLog(QByteArray ba, Direction dir = NoDirection);
    void writeLog(Error::Msg msg, Direction dir = NoDirection)
    {
        writeLog(QVariant::fromValue(msg).toByteArray(), dir);
    }

    CommandStruct m_currentCommand;
    void checkWriteQueue();
    QPair<quint64, QByteArray> m_buffer;
    void checkQueue();

signals:

    void ShowError(QString message);
    void QueryFinished();
    void writeDataAttempt(const QByteArray);

    void errorOccurred(Error::Msg);
    /// Like a QIODevice::readyRead()
    void readyRead();
};
