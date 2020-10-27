#pragma once

#include "../gen/logclass.h"
#include "../gen/modulebsi.h"
#include "../gen/s2.h"
#include "../gen/singleton.h"
#include "baseinterface.h"
#include "defines.h"
#include "eusbworker.h"

#include <QEventLoop>
#include <QThread>
#include <QTimer>
typedef unsigned char byte;
class EProtocom final : public BaseInterface, public Singleton<EProtocom>
{

    Q_OBJECT

public:
    struct CommandStruct
    {
        byte cmd;
        quint32 uintarg;
        float flarg;
        QByteArray ba;
    };
    explicit EProtocom(token, QWidget *parent = nullptr);
    ~EProtocom();

    bool Connect(int devicePosition);
    bool Reconnect();
    void Disconnect();

    QList<QStringList> DevicesFound();

    EUsbWorker *usbWorker() const;

    bool start(const ConnectStruct &st) override;
    void reqStartup() override;
    void reqTime() override;
    void reqFile(quint32 filenum) override;
    void writeFile(quint32 filenum, const QByteArray &file) override;
    void writeTime(quint32 time) override;
    void writeCommand(Queries::Commands cmd, QList<DataTypes::SignalsStruct> &list) override;

    void SendCmd(unsigned char command, int parameter = 0);
    // read
    void SendIn(unsigned char command, char parameter, QByteArray &ba, qint64 maxdatasize);
    // write
    void SendOut(unsigned char command, char board_type, QByteArray &ba);
    // write file
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
    inline bool isWorkerRunning() const
    {
        return m_workerStatus;
    };
    inline bool isParserRunning() const
    {
        return m_parserStatus;
    };

public slots:
    void stop() override;

private:
    static bool m_writeUSBLog;

    EUsbWorker *m_usbWorker;

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
    QByteArray ReadDataChunk;
    QByteArray WriteData;
    //    QString m_deviceName;
    QVector<DeviceConnectStruct> m_devices;

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

    static byte translate(const Queries::Commands cmd);
    static Queries::Commands translate(const byte cmd);

    const static QMap<Queries::Commands, byte> m_dict;

signals:
    // сигналы для прогрессбаров - отслеживание принятых данных, стёртых осциллограмм и т.п.
    void SetDataSize(int);
    void SetDataCount(int);
    void readbytessignal(QByteArray);  // for TE updating
    void writebytessignal(QByteArray); // for TE updating
    void ShowError(QString message);
    void QueryFinished();
};
