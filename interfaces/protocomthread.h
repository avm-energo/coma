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

    bool Connect(int devicePosition);
    bool Reconnect();
    void Disconnect();

    static QList<DeviceConnectStruct> DevicesFound(quint16 vid = 0xC251);

    UsbHidPort *usbWorker() const;
    bool start(const int &devPos);

    void SendCmd(unsigned char command, int parameter = 0);
    // read
    void SendIn(unsigned char command, char parameter, QByteArray &ba, qint64 maxdatasize);
    // write
    void SendOut(unsigned char command, char board_type, QByteArray &ba);
    // write file
    void SendFile(unsigned char command, char board_type, int filenum, QByteArray &ba);

    void usbStateChanged(void *message);

    static bool isWriteUSBLog();
    static void setWriteUSBLog(bool isWriteUSBLog);

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
    unsigned char Command;
    quint8 bStep;
    Error::Msg m_result;
    int FNum;
    quint32 ReadDataChunkLength, RDLength; // длина всей посылки
    quint32 WRLength;                      // длина всей посылки
    qint64 InDataSize;
    int SegLeft; // количество оставшихся сегментов
    int SegEnd;  // номер последнего байта в ReadData текущего сегмента
    int m_devicePosition;

    // bool LastBlock; // признак того, что блок последний, и больше запрашивать не надо

    QByteArray InData, OutData;
    QByteArray m_readDataChunk;
    QByteArray m_writeData;
    //    QString m_deviceName;
    static QVector<DeviceConnectStruct> m_devices;

    QTimer *OscTimer;
    QTimer *m_waitTimer;
    QEventLoop m_loop;

    quint16 OscNum;

    void Send(char command, char parameter, QByteArray &ba, qint64 datasize);
    void InitiateSend();
    void WriteDataToPort(QByteArray &ba);
    void Finish(Error::Msg msg);
    void SetWRSegNum();
    void WRCheckForNextSegment(int first);
    void AppendSize(QByteArray &ba, int size);
    void SendOk(bool cont = false); // cont = 1 -> send CN_MS3 instead CN_MS
    void SendErr();
    bool GetLength(); // ok = 1 -> обработка посылки вида SS OK ..., ok = 0 -> вида SS c L L ... возвращаемое
                      // значение = false -> неправильная длина
    void CheckForData();
    void OscTimerTimeout();
    void ParseIncomeData(QByteArray ba);
    QByteArray RawRead(int bytes);
    void RawClose();

    LogClass *Log;

    QReadWriteLock m_rwLocker;

signals:

    void ShowError(QString message);
    void QueryFinished();
    void writeDataAttempt(const QByteArray);
};
