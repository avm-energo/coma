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
// Обмен с модулями
namespace CN
{
typedef unsigned char byte;
namespace Limits
{
    // 64-4 ('<',cmd,L,L)
    constexpr unsigned MaxSegmenthLength = 60;
    constexpr unsigned MaxMemoryFileSize = 65535;
    // максимальный размер выходного файла
    constexpr unsigned MaxFileSize = 300000;
    constexpr unsigned MaxGetFileSize = 16777215;

    // максимальный ИД осциллограмм
    constexpr unsigned MaxOscillogramId = 2999;
    // минимальный ИД осциллограмм, нужно, т.к. файлы осциллограмм обрабатываются по-своему
    constexpr unsigned MinOscillogramId = 1000;

    // максимальный ИД журналов
    constexpr byte MaxJournalId = 6;
    // минимальный ИД журналов
    constexpr byte MinJournalId = 4;
}
// Канал связи с модулем
// таймаут по USB в мс
constexpr unsigned Timeout = 10000;
// таймаут посылки запроса нестёртых осциллограмм
constexpr unsigned TimeoutOscillogram = 1000;
// 100 ms main loop sleep
constexpr unsigned MainLoopDelay = 100;
//'\x00'
constexpr byte NullByte = 0x00;
// ответ "всё в порядке"
constexpr byte ResultOk = 0x11;
// запуск, остановка теста
constexpr byte Test = 0x49;
// ответ "ошибка"
constexpr byte ResultError = 0xf0;
// неизвестная команда
constexpr byte Unknown = 0xff;
namespace Read
{
    // чтение блока стартовой информации
    constexpr byte BlkStartInfo = 0x21;
    // чтение настроечных коэффициентов
    constexpr byte BlkAC = 0x22;
    // чтение текущих данных без настройки
    constexpr byte BlkDataA = 0x23;
    // чтение блока текущих данных
    constexpr byte BlkData = 0x24;
    // чтение технологического блока
    constexpr byte BlkTech = 0x26;
    // чтение файла
    constexpr byte File = 0x25;
    // чтение номера варианта использования
    constexpr byte Variant = 0x27;
    // чтение текущего режима работы
    constexpr byte Mode = 0x28;
    // чтение времени из МНК в формате UNIX
    constexpr byte Time = 0x29;
    // запрос текущего прогресса
    constexpr byte Progress = 0x46;

}
namespace Write
{
    // запись настроечных коэффициентов
    constexpr byte BlkAC = 0x31;
    // посылка блока данных
    constexpr byte BlkData = 0x34;
    // посылка команды
    constexpr byte BlkCmd = 0x35;
    // запись технологического блока
    constexpr byte BlkTech = 0x2B;
    // запись файла
    constexpr byte File = 0x32;
    // задание варианта использования
    constexpr byte Variant = 0x44;
    // задание текущего режима работы
    constexpr byte Mode = 0x43;
    // запись времени в МНК в формате UNIX
    constexpr byte Time = 0x2A;
    // переход на новое ПО
    constexpr byte Upgrade = 0x40;
    // стирание технологического блока
    constexpr byte EraseTech = 0x45;
    // стирание счётчиков дискретных состояний
    constexpr byte EraseCnt = 0x47;
    // запись версии аппаратуры модуля/серийного номера/типа платы
    constexpr byte Hardware = 0x48;
}
namespace Message
{
    // начало посылки
    constexpr byte Start = 0x3e;
    // продолжение посылки
    constexpr byte Continue = 0x23;
    // начало посылки модуля
    constexpr byte Module = 0x3c;
    // length is 2 bytes
    constexpr byte Length2Byte = 0x02;
}
}

// Обмен с модулями
//#define CN_BYTE0
//#define CN_ResOk 0x11
//#define CN_GBsi 0x21
//#define CN_GBac 0x22
//#define CN_GBda 0x23
//#define CN_GBd 0x24
//#define CN_GF 0x25
//#define CN_GBt 0x26
//#define CN_GVar 0x27
//#define CN_GMode 0x28
//#define CN_GTime 0x29
//#define CN_WTime 0x2A
//#define CN_WBt 0x2B
//#define CN_WBac 0x31
//#define CN_WF 0x32
//#define CN_WBd 0x34
//#define CN_WCom 0x35
//#define CN_VPO 0x40
//#define CN_SMode 0x43
//#define CN_NVar 0x44
//#define CN_Ert 0x45
//#define CN_ErPg 0x46
//#define CN_CtEr 0x47
//#define CN_WHv 0x48
//#define CN_STest 0x49
//#define CN_ResErr '\xF0'
//#define CN_Unk '\xFF'

//#define CN_MS 0x3e
//#define CN_MS3 0x23
//#define CN_SS 0x3c
//#define CN_L2 0x02

#define WHV_SIZE_ONEBOARD 17
#define WHV_SIZE_TWOBOARDS 33

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
    //    int ernum;
    bool NeedToSend, Busy, NeedToFinish;
    bool Connected;
    //    qint64 RDSize;
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

signals:
    // сигналы для прогрессбаров - отслеживание принятых данных, стёртых осциллограмм и т.п.
    void SetDataSize(int);
    void SetDataCount(int);
    void readbytessignal(QByteArray); // for TE updating
    void writebytessignal(QByteArray); // for TE updating
    void ShowError(QString message);
    void QueryFinished();
    void Finished();
    void ReconnectSignal();

public slots:
    void Timeout();
    void Disconnect();
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
    int WRLength; // длина всей посылки
    qint64 InDataSize;
    int SegLeft; // количество оставшихся сегментов
    int SegEnd; // номер последнего байта в ReadData текущего сегмента
    bool LastBlock; // признак того, что блок последний, и больше запрашивать не надо
    char BoardType;
    static bool WriteUSBLog;

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
