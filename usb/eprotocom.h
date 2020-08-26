#pragma once

#include "../gen/logclass.h"
#include "../gen/modulebsi.h"
#include "../gen/s2.h"
#include "eusbworker.h"

#include <QThread>

//#define NOTIMEOUT

class EProtocom : public QObject
{
    Q_OBJECT
public:
    ~EProtocom();

    static EProtocom *GetInstance(QObject *parent = nullptr);

    bool Connect();
    void Disconnect();

    QStringList DevicesFound() const;
    QString deviceName() const;
    void setDeviceName(const QString &deviceName);
    // функция, разбивающая строку устройства и складывающая в соотв. структуру
    void TranslateDeviceAndSave(const QString &str);

    EUsbWorker *usbWorker() const;
    QThread *workerThread();

    void SendCmd(char command, int parameter = 0);
    void SendIn(char command, char parameter, QByteArray &ba, qint64 maxdatasize);
    void SendOut(char command, char board_type, QByteArray &ba);
    void SendFile(unsigned char command, char board_type, int filenum, QByteArray &ba);

    // void Timeout();

    bool isConnected() const;
    void setConnected(bool isConnected);

    static bool isWriteUSBLog();
    static void setWriteUSBLog(bool isWriteUSBLog);

    qint32 result() const;
    void setResult(const qint32 &result);

protected:
    explicit EProtocom(QObject *parent = nullptr);
    EProtocom(EProtocom &) = delete;

    void operator=(const EProtocom &) = delete;

private:
    static EProtocom *pinstance_;
    static QMutex mutex_;
    static bool m_writeUSBLog;

    EUsbWorker *m_usbWorker;
    QThread m_workerThread;

    char BoardType;
    char Command;
    quint8 bStep;
    qint32 m_result;
    int FNum;
    qint32 ReadDataChunkLength, RDLength; // длина всей посылки
    int WRLength;                         // длина всей посылки
    qint64 InDataSize;
    int SegLeft; // количество оставшихся сегментов
    int SegEnd;  // номер последнего байта в ReadData текущего сегмента

    // bool LastBlock; // признак того, что блок последний, и больше запрашивать не надо

    bool m_connected;

    QByteArray InData, OutData;
    QByteArray ReadDataChunk;
    QByteArray WriteData;
    QString m_deviceName;
    DeviceConnectStruct UsbPort;

    LogClass *CnLog;
    QTimer *TTimer, *OscTimer;

    quint16 OscNum;

    void Send(char command, char parameter, QByteArray &ba, qint64 datasize);
    void InitiateSend();
    void WriteDataToPort(QByteArray &ba);
    void Finish(int ernum);
    void SetWRSegNum();
    void WRCheckForNextSegment(int first);
    void AppendSize(QByteArray &ba, int size);
    void SendOk(bool cont = false); // cont = 1 -> send CN_MS3 instead CN_MS
    void SendErr();
    bool GetLength(); // ok = 1 -> обработка посылки вида SS OK ..., ok = 0 -> вида SS c L L ... возвращаемое
                      // значение = false -> неправильная длина
    void ClosePort();
    void CheckForData();
    void OscTimerTimeout();
    void ParseIncomeData(QByteArray ba);
    QByteArray RawRead(int bytes);
    int RawWrite(QByteArray &ba);
    void RawClose();

signals:
    // сигналы для прогрессбаров - отслеживание принятых данных, стёртых осциллограмм и т.п.
    void SetDataSize(int);
    void SetDataCount(int);
    void readbytessignal(QByteArray);  // for TE updating
    void writebytessignal(QByteArray); // for TE updating
    void ShowError(QString message);
    void QueryFinished();
    //  void Finished();
    void ReconnectSignal();
};
