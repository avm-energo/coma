#ifndef EEAbstractProtocomChannel_H
#define EEAbstractProtocomChannel_H

#include <QObject>
#include <QByteArray>
#include <QTimer>
#include <QMutex>
#include <QWaitCondition>

#include "s2.h"
#include "log.h"

//#ifdef COMPORTENABLE
//#define CN_MAXSEGMENTLENGTH 764 // 768-4 ('<',cmd,L,L) максимальная длина одного сегмента (0x300)
//#endif
//#ifdef USBENABLE
#define CN_MAXSEGMENTLENGTH 60 // 64-4 ('<',cmd,L,L)
//#endif

// Канал связи с модулем

#define CN_TIMEOUT  5000 // таймаут по USB в мс
#define CN_OSCT     1000 // таймаут посылки запроса нестёртых осциллограмм
#define CN_MAXFILESIZE  300000 // максимальный размер выходного файла
#define CN_MAINLOOP_DELAY   100 // 100 ms main loop sleep

// Обмен с модулями
#define CN_BYTE0    '\x00'
#define CN_ResOk    0x11 // ответ "всё в порядке"
#define CN_GBsi     0x21 // чтение блока стартовой информации
#define CN_GBac     0x22 // чтение настроечных коэффициентов
#define CN_GBda     0x23 // чтение текущих данных без настройки
#define CN_GBd      0x24 // чтение блока текущих данных
#define CN_GF       0x25 // чтение файла
#define CN_GBt      0x26 // чтение технологического блока
#define CN_GVar     0x27 // чтение номера варианта использования
#define CN_GMode    0x28 // чтение текущего режима работы
#define CN_GTime    0x29 // чтение времени из МНК в формате юникс
#define CN_WTime    0x2A // запись времени в МНК в формате юникс
#define CN_WBt      0x2B // запись технологического блока
#define CN_WBac     0x31 // запись настроечных коэффициентов
#define CN_WF       0x32 // запись файла
#define CN_WBc      0x33 // посылка блока выходных состояний
#define CN_WBd      0x34 // посылка блока данных
#define CN_WCom     0x35 // посылка команды
#define	CN_VPO		0x40 // переход на новое ПО
#define CN_SMode    0x43 // задание текущего режима работы
#define CN_NVar     0x44 // задание варианта использования
#define CN_Ert      0x45 // стирание технологического блока
#define CN_ErPg     0x46 // запрос текущего прогресса
#define CN_CtEr     0x47 // стирание счётчиков дискретных состояний
#define CN_WHv      0x48 // запись версии аппаратуры модуля/серийного номера/типа платы
#define	CN_STest	0x49 // запуск, остановка теста
#define CN_ResErr   '\xF0' // ответ "ошибка"
#define CN_Unk      '\xFF' // неизвестная команда

#define CN_MS       0x3e // начало посылки
#define CN_MS3      0x23 // продолжение посылки
#define CN_SS       0x3c // начало посылки модуля
#define CN_L2       0x02 // length is 2 bytes

#define CN_MINOSCID 1000 // минимальный ИД осциллограмм, нужно, т.к. файлы осциллограмм обрабатываются по-своему
#define CN_MAXOSCID 2999 // максимальный ИД осциллограмм

#define MINJOURID 4 // минимальный ИД журналов
#define MAXJOURID 6 // максимальный ИД журналов

#define WHV_SIZE_ONEBOARD   17
#define WHV_SIZE_TWOBOARDS  33

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

    qint32 result;
    int baud;
    int ernum;
    bool NeedToSend, Busy, NeedToFinish;
    QMutex BusyMutex;
    QWaitCondition BusyWC;
    bool Connected, Cancelled;
    int RDSize;
    Log *CnLog;
    QString ComPort;
    DeviceConnectStruct UsbPort;

    virtual bool Connect() = 0;
    virtual QByteArray RawRead(int bytes) = 0;
    virtual int RawWrite(QByteArray &ba) = 0;
    virtual void RawClose() = 0;

    void Send(char command, char board_type=0, void *ptr=nullptr, int ptrsize=0, int filenum=0, \
              QVector<S2::DataRec> *DRptr=nullptr);
    static void SetWriteUSBLog(bool bit);
    static bool IsWriteUSBLog();
    virtual QStringList DevicesFound() = 0; // функция, возвращающая список найденных устройств (COM-портов, устройств USB)
    void TranslateDeviceAndSave(const QString &str); // функция, разбивающая строку устройства и складывающая в соотв. структуру

signals:
    void SetDataSize(int); // сигналы для прогрессбаров - отслеживание принятых данных, стёртых осциллограмм и т.п.
    void SetDataCount(int);
    void readbytessignal(QByteArray ); // for TE updating
    void writebytessignal(QByteArray ); // for TE updating
    void Retry();
    void ShowError(QString message);
    void QueryFinished();

public slots:
    void Timeout();
    void Disconnect();
    void ParseIncomeData(QByteArray ba);
    void SetCancelled();

private slots:
    void CheckForData();
    void OscTimerTimeout();

private:
    char *outdata;
    QByteArray ReadData, ReadDataChunk;
    QByteArray WriteData;
    QTimer *TTimer, *OscTimer;
    quint16 OscNum;
    quint8 bStep;
    char cmd;
    int fnum;
    int ReadDataChunkLength, RDLength; // длина всей посылки
    int WRLength; // длина всей посылки
    int outdatasize; // размер приёмной области памяти
    int SegLeft; // количество оставшихся сегментов
    int SegEnd; // номер последнего байта в ReadData текущего сегмента
    bool LastBlock; // признак того, что блок последний, и больше запрашивать не надо
    QVector<S2::DataRec> *DR; // ссылка на структуру DataRec, по которой собирать/восстанавливать S2
    QVector<S2::DataRec> DRosc;
    char BoardType;
    static bool WriteUSBLog;

    void InitiateSend();
    void WriteDataToPort(QByteArray &ba);
    void Finish(int ernum);
    void SetWRSegNum();
    void WRCheckForNextSegment(int first);
    void AppendSize(QByteArray &ba, int size);
    void SendOk(bool cont=false); // cont = 1 -> send CN_MS3 instead CN_MS
    void SendErr();
    bool GetLength(); // ok = 1 -> обработка посылки вида SS OK ..., ok = 0 -> вида SS c L L ... возвращаемое значение = false -> неправильная длина
};

#endif // EEAbstractProtocomChannel_H
