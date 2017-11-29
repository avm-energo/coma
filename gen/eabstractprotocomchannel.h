#ifndef EEAbstractProtocomChannel_H
#define EEAbstractProtocomChannel_H

#include <QObject>
#include <QByteArray>
#include <QTimer>

#include "publicclass.h"
#include "log.h"

#ifdef COMPORTENABLE
#define CN_MAXSEGMENTLENGTH 764 // 768-4 ('<',cmd,L,L) максимальная длина одного сегмента (0x300)
#endif
#ifdef USBENABLE
#define CN_MAXSEGMENTLENGTH 60 // 64-4 ('<',cmd,L,L)
#endif

// Канал связи с модулем

#define CN_TIMEOUT  2000 // таймаут по USB в мс
#define CN_OSCT     1000 // таймаут посылки запроса нестёртых осциллограмм
#define CN_MAXFILESIZE  30000 // максимальный размер выходного файла
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
#define CN_WBt      0x2B // запись технологического блока
#define CN_WBac     0x31 // запись настроечных коэффициентов
#define CN_WF       0x32 // запись файла
#define CN_WBc      0x33 // посылка блока выходных состояний
#define CN_NVar     0x44 // задание варианта использования
#define CN_Ert      0x45 // стирание технологического блока
#define CN_ErPg     0x46 // запрос текущего прогресса
#define CN_CtEr     0x47 // стирание счётчиков дискретных состояний
#define CN_WHv      0x48 // запись версии аппаратуры модуля/серийного номера/типа платы
#define CN_ResErr   '\xF0' // ответ "ошибка"
#define CN_Unk      '\xFF' // неизвестная команда

#define CN_MS       0x3e // начало посылки
#define CN_MS3      0x23 // продолжение посылки
#define CN_SS       0x3c // начало посылки модуля
#define CN_L2       0x02 // length is 2 bytes

#define CN_MINOSCID 1000 // минимальный ИД осциллограмм, нужно, т.к. файлы осциллограмм обрабатываются по-своему
#define CN_MAXOSCID 2999 // максимальный ИД осциллограмм

#define NOTIMEOUT

class EAbstractProtocomChannel : public QObject
{
    Q_OBJECT
public:
    explicit EAbstractProtocomChannel(QObject *parent = 0);
    ~EAbstractProtocomChannel();

    qint32 result;
    int baud;
    int ernum;
    bool NeedToSend, Busy, NeedToFinish;
    bool Connected, Cancelled;
    quint32 RDSize;
    Log *log;

    virtual bool Connect() = 0;
    virtual QByteArray RawRead(int bytes) = 0;
    virtual qint64 RawWrite(QByteArray &ba) = 0;
    virtual void RawClose() = 0;

    void Send(int command, int board_type=BT_NONE, void *ptr=NULL, quint32 ptrsize=0, quint16 filenum=0, \
              QVector<publicclass::DataRec> *DRptr=0);

signals:
    void SetDataSize(quint32); // сигналы для прогрессбаров - отслеживание принятых данных, стёртых осциллограмм и т.п.
    void SetDataCount(quint32);
    void readbytessignal(QByteArray &); // for TE updating
    void writebytessignal(QByteArray &); // for TE updating
    void Retry();
    void ShowError(QString message);

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
    int bStep;
    int cmd;
    quint16 fnum;
    quint32 ReadDataChunkLength, RDLength; // длина всей посылки
    quint32 WRLength; // длина всей посылки
    quint32 outdatasize; // размер приёмной области памяти
    quint32 SegLeft; // количество оставшихся сегментов
    quint32 SegEnd; // номер последнего байта в ReadData текущего сегмента
    bool LastBlock; // признак того, что блок последний, и больше запрашивать не надо
    QVector<publicclass::DataRec> *DR; // ссылка на структуру DataRec, по которой собирать/восстанавливать S2
    quint8 BoardType;

    void InitiateSend();
    void WriteDataToPort(QByteArray &ba);
    void Finish(int ernum);
    void SetWRSegNum();
    void WRCheckForNextSegment(int first);
    void AppendSize(QByteArray &ba, quint16 size);
    void SendOk(bool cont=false); // cont = 1 -> send CN_MS3 instead CN_MS
    void SendErr();
    bool GetLength(); // ok = 1 -> обработка посылки вида SS OK ..., ok = 0 -> вида SS c L L ... возвращаемое значение = false -> неправильная длина
};

#endif // EEAbstractProtocomChannel_H