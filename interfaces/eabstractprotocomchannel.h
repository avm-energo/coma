#ifndef EEAbstractProtocomChannel_H
#define EEAbstractProtocomChannel_H

#include "../gen/logclass.h"
#include "../gen/modulebsi.h"
#include "../gen/s2.h"

#include <QByteArray>
#include <QMutex>
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QWaitCondition>

//#define NOTIMEOUT

class EAbstractProtocomChannel : public QObject
{
    Q_OBJECT
public:
    explicit EAbstractProtocomChannel(QObject *parent = nullptr);
    ~EAbstractProtocomChannel();

    struct DeviceConnectStruct
    {
        unsigned short vendor_id;
        unsigned short product_id;
        wchar_t serial[20];
    };

    qint32 Result;
    bool NeedToSend, Busy, NeedToFinish;

    LogClass *CnLog;
    DeviceConnectStruct UsbPort;

    virtual bool Connect() = 0;
    virtual QByteArray RawRead(int bytes) = 0;
    virtual int RawWrite(QByteArray &ba) = 0;
    virtual void RawClose() = 0;

    void SendCmd(char command, int parameter = 0);
    void SendIn(char command, char parameter, QByteArray &ba, qint64 maxdatasize);
    void SendOut(char command, char board_type, QByteArray &ba);
    void SendFile(unsigned char command, char board_type, int filenum, QByteArray &ba);
    static void SetWriteUSBLog(bool bit);
    static bool IsWriteUSBLog();
    // функция, возвращающая список найденных устройств (COM-портов, устройств USB)
    virtual QStringList DevicesFound() const = 0;
    // функция, разбивающая строку устройства и складывающая в соотв. структуру
    void TranslateDeviceAndSave(const QString &str);

    bool isConnected() const;
    void setConnected(bool isConnected);

signals:
    // сигналы для прогрессбаров - отслеживание принятых данных, стёртых осциллограмм и т.п.
    void SetDataSize(int);
    void SetDataCount(int);
    void readbytessignal(QByteArray);  // for TE updating
    void writebytessignal(QByteArray); // for TE updating
    void ShowError(QString message);
    void QueryFinished();
    void Finished();
    void ReconnectSignal();

public slots:
    void Timeout();
    virtual void Disconnect();
    void ParseIncomeData(QByteArray ba);

private slots:
    void CheckForData();
    void OscTimerTimeout();

private:
    QByteArray InData, OutData;
    QByteArray ReadDataChunk; //, ReadData;
    QByteArray WriteData;
    QTimer *TTimer, *OscTimer;
    //    quint16 OscNum;
    quint8 bStep;
    char Command;
    int FNum;
    qint32 ReadDataChunkLength, RDLength; // длина всей посылки
    int WRLength;                         // длина всей посылки
    qint64 InDataSize;
    int SegLeft; // количество оставшихся сегментов
    int SegEnd;  // номер последнего байта в ReadData текущего сегмента
    bool LastBlock; // признак того, что блок последний, и больше запрашивать не надо
    char BoardType;
    static bool WriteUSBLog;
    bool m_connected;

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
};

#endif // EEAbstractProtocomChannel_H
