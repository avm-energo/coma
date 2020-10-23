#pragma once

#include "../gen/logclass.h"
#include "../gen/modulebsi.h"
#include "../gen/s2.h"
#include "../gen/singleton.h"
#include "eusbworker.h"

#include <QEventLoop>
#include <QThread>
#include <QTimer>

class EProtocom final : public QObject, public Singleton<EProtocom>
{
    Q_OBJECT
public:
    explicit EProtocom(token);
    ~EProtocom();

    bool Connect(int devicePosition);
    bool Reconnect();
    void Disconnect();

    QList<QStringList> DevicesFound();
    //    QString deviceName() const;
    //    void setDeviceName(const QString &deviceName);
    // функция, разбивающая строку устройства и складывающая в соотв. структуру
    // void TranslateDeviceAndSave(const QString &str);

    EUsbWorker *usbWorker() const;
    QThread *workerThread();

    void SendCmd(unsigned char command, int parameter = 0);
    void SendIn(unsigned char command, char parameter, QByteArray &ba, qint64 maxdatasize);
    void SendOut(unsigned char command, char board_type, QByteArray &ba);
    void SendFile(unsigned char command, char board_type, int filenum, QByteArray &ba);

    // void Timeout();

    void usbStateChanged(void *message);

    static bool isWriteUSBLog();
    static void setWriteUSBLog(bool isWriteUSBLog);

    Error::Msg result() const;
    void setResult(const Error::Msg &result);

    //    int devicePosition() const;
    //    void setDevicePosition(int devicePosition);

    //    QString usbSerial() const;

private:
    static bool m_writeUSBLog;

    EUsbWorker *m_usbWorker;
    QThread m_workerThread;

    char BoardType;
    unsigned char Command;
    quint8 bStep;
    Error::Msg m_result;
    int FNum;
    quint32 ReadDataChunkLength, RDLength; // длина всей посылки
    quint32 WRLength; // длина всей посылки
    qint64 InDataSize;
    int SegLeft; // количество оставшихся сегментов
    int SegEnd; // номер последнего байта в ReadData текущего сегмента
    int m_devicePosition;

    // bool LastBlock; // признак того, что блок последний, и больше запрашивать не надо

    QByteArray InData, OutData;
    QByteArray ReadDataChunk;
    QByteArray WriteData;
    //    QString m_deviceName;
    QVector<DeviceConnectStruct> m_devices;

    LogClass *CnLog;
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
    int RawWrite(QByteArray &ba);
    void RawClose();

signals:
    // сигналы для прогрессбаров - отслеживание принятых данных, стёртых осциллограмм и т.п.
    void SetDataSize(int);
    void SetDataCount(int);
    void readbytessignal(QByteArray); // for TE updating
    void writebytessignal(QByteArray); // for TE updating
    void ShowError(QString message);
    void QueryFinished();
};
