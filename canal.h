#ifndef CANAL_H
#define CANAL_H

#include <QObject>
#include <QByteArray>
#include <QTimer>
#include <QLabel>
#include <QMutex>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "publicclass.h"

// Канал связи с модулем

#define CN_TIMEOUT  2000 // таймаут по USB в мс
#define CN_OSCT     1000 // таймаут посылки запроса нестёртых осциллограмм
#define CN_MAXFILESIZE  30000 // максимальный размер выходного файла
#define CN_MAXSEGMENTLENGTH 768 // максимальная длина одного сегмента (0x300)
#define CN_MAINLOOP_DELAY   100 // 100 ms main loop sleep

// Обмен с модулями
#define CN_BYTE0    '\x00'
#define CN_ResOk    0x11 // ответ "всё в порядке"
#define CN_GBsi     0x21 // чтение блока стартовой информации
#define CN_GBac     0x22 // чтение настроечных коэффициентов
#define CN_GBda     0x23 // чтение текущих данных без настройки
#define CN_GBd      0x24 // чтение блока текущих данных
#define CN_GF       0x25 // чтение файла
#define CN_GBo      0x26 // чтение осциллограмм
#define CN_GBe      0x28 // чтение банка текущих событий
#define CN_GBTe     0x29 // чтение банка технологических событий
#define CN_WBac     0x31 // запись настроечных коэффициентов
#define CN_WF       0x32 // запись файла
#define CN_IP       0x43 // чтение IP-адреса
#define CN_OscEr    0x45 // стирание всех осциллограмм из Flash-памяти
#define CN_OscPg    0x46 // запрос оставшегося для стирания по команде OscEr числа страниц
#define CN_CtEr     0x47 // стирание счётчиков дискретных состояний
#define CN_WHv      0x48 // запись версии аппаратуры модуля/серийного номера/типа платы
#define CN_ResErr   '\xF0' // ответ "ошибка"
#define CN_Unk      '\xFF' // неизвестная команда

#define CN_MS       0x3e // начало посылки
#define CN_MS3      0x23 // продолжение посылки
#define CN_SS       0x3c // начало посылки модуля
#define CN_L2       0x02 // length is 2 bytes

// определение ошибок модуля - см. publicclass.h (USO_)

#define CN_MER_UNKN_ERR     0xFF // неизвестная ошибка

#define MAXLENGTH   0x300   // максимальный размер блока 768 байт

#define BT_NONE     0
#define BT_BASE     1
#define BT_MEZONIN  2

class Canal : public QObject
{
    Q_OBJECT
public:
    explicit Canal(QObject *parent = 0);
    ~Canal();

/*    enum board_types
    {
        BT_NONE,
        BT_BASE,
        BT_MEZONIN
    }; */

    int result;
    QSerialPortInfo info;
    int baud;
    int ernum;
    bool FirstRun;
    bool NeedToSend, Busy, NeedToFinish;
    bool Connected;

    bool Connect();
    void Disconnect();
    void Send(int command, int board_type=BT_NONE, void *ptr=NULL, quint32 ptrsize=0, quint16 filenum=0, \
              QVector<publicclass::DataRec> *DRptr=0);

signals:
    void writedatalength(quint32);
    void writedatapos(quint32);
    void bytesreceived(quint32);
    void incomingdatalength(quint32);
    void readbytessignal(QByteArray &);
    void writebytessignal(QByteArray &);
    void sendend();
    void oscerasesize(quint32);
    void osceraseremaining(quint32);

public slots:
    void Timeout();

private slots:
    void CheckForData();
    void OscTimerTimeout();
    void PortCloseTimeout();
    void Error(QSerialPort::SerialPortError);

private:
    char *outdata;
    QByteArray ReadData, ReadDataChunk;
    QByteArray WriteData;
    QTimer *TTimer, *OscTimer;
    quint16 OscNum;
    int bStep;
    int cmd;
    QLabel *lbl;
    quint16 fnum;
    quint32 RDLength; // длина всей посылки
    quint32 WRLength; // длина всей посылки
    quint32 outdatasize; // размер приёмной области памяти
    quint32 SegLeft; // количество оставшихся сегментов
    quint32 SegEnd; // номер последнего байта в ReadData текущего сегмента
    bool FirstSegment; // признак того, что передаётся первый сегмент (начало посылки MS, а не MS3)
    bool LastBlock; // признак того, что блок последний, и больше запрашивать не надо
    QVector<publicclass::DataRec> *DR; // ссылка на структуру DataRec, по которой собирать/восстанавливать S2
    quint8 BoardType;
    bool PortCloseTimeoutSet;
    QSerialPort *port;

    bool InitializePort(QSerialPortInfo &pinfo, int baud);
    void ClosePort();
    void InitiateSend();
    void WriteDataToPort(QByteArray &ba);
    void ParseIncomeData(QByteArray &ba);
    void Finish(int ernum);
    void SetWRSegNum();
    void WRCheckForNextSegment();
    void AppendSize(QByteArray &ba, quint16 size);
    void SendOk(bool cont=false); // cont = 1 -> send CN_MS3 instead CN_MS
    void SendErr();
    bool GetLength(bool ok=true); // ok = 1 -> обработка посылки вида SS OK ..., ok = 0 -> вида SS c L L ... возвращаемое значение = false -> неправильная длина
};

extern Canal *cn;

#endif // CANAL_H
