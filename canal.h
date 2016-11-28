#ifndef CANAL_H
#define CANAL_H

#include <QObject>
#include <QByteArray>
#include <QThread>
#include <QTimer>
#include <QLabel>
#include <QMutex>

#include "publicclass.h"
#include "serialthread.h"

// Канал связи с модулем

#define CN_TIMEOUT  2000 // таймаут по USB в мс
#define CN_OSCT     1000 // таймаут посылки запроса нестёртых осциллограмм

// Обмен с модулями
#define CN_ResOk   0x11 // ответ "всё в порядке"
#define CN_GBsi    0x21 // запрос блока стартовой информации
#define CN_Gac     0x22 // запрос настроечных коэффициентов
#define CN_Gda     0x23 // запрос текущих данных без настройки
#define CN_GBd     0x24 // запрос блока текущих данных
#define CN_GF      0x25 // запрос файла конфигурации
#define CN_GNosc   0x26 // запрос информации об осциллограммах
#define CN_GBosc   0x27 // запрос осциллограммы
#define CN_GBev    0x28 // запрос банка текущих событий
#define CN_GBdi    0x29 // запрос текущих данных, приведённых ко входу
#define CN_Wac     0x31 // запись настроечных коэффициентов
#define CN_WF      0x32 // запись файла конфигурации
#define CN_Start   0x3e // начало посылки
#define CN_MStart  0x3c // начало посылки модуля
#define CN_Cln     0x41 // зажечь светодиод
#define CN_Clf     0x42 // погасить светодиод
#define CN_Gip     0x43 // запрос IP-адреса
#define CN_Cps     0x44 // контроль 1PPS
#define CN_OscEr   0x45 // стирание всех осциллограмм из Flash-памяти
#define CN_OscPg   0x46 // запрос оставшегося для стирания по команде OscEr числа страниц
#define CN_Wsn     0x47 // запись серийного номера модуля
#define CN_WHv     0x48 // запись версии аппаратуры модуля
#define CN_Cnc     0x49 // команда перехода на новую конфигурацию
#define CN_GBte    0x4A // запрос блока технологических событий
#define CN_SegOk   0x55 // ответ "сегмент принят в порядке"
#define CN_ResErr  0x0f // ответ "ошибка"
#define CN_Unk     0xff // неизвестная команда
// определение ошибок

#define CN_OK               0 // ошибок нет
#define CN_PORTOPENERROR    1 // ошибка открытия порта
#define CN_NOPORTSERROR     2 // нет портов в системе
#define CN_SEGFAULTERROR    3 // ошибка при приёме сегмента данных на стороне модуля
#define CN_RCVDATAERROR     4 // ошибка при приёме данных (несовпадение длины, неправильный начальный символ и т.п.)
#define CN_TIMEOUTERROR     5 // таймаут при приёме данных
#define CN_RCVLENGTHERROR   6 // некорректная длина блока (принято больше, чем может вместить в себя приёмная переменная)
#define CN_UNKNOWNCMDERROR  7 // неизвестная команда
#define CN_NULLDATAERROR    12 // в канальную процедуру переданы некорректные данные

class canal : public QObject
{
    Q_OBJECT
public:
    explicit canal(QObject *parent = 0);
    ~canal();

    int result;
    QSerialPortInfo info;
    int baud;
    int ernum;
    bool FirstRun;
    bool NeedToSend, Busy, NeedToFinish;
    quint32 RDSize;
    QMutex ReadDataMtx, WriteDataMtx;

    void Connect();
    void Disconnect();
    void Send(int command, void *ptr=NULL, quint32 ptrsize=0, int filenum=0, publicclass::DataRec *DRptr=NULL);

signals:
    void stopall();
    void DataReady();
    void portopened();
    void incomingdatalength(quint32);
    void bytesreceived(quint32);
    void writedatatoport(QByteArray);
    void gotsomedata(QByteArray *);
    void somedatawritten(QByteArray *);
    void SendEnd();
    void OscEraseSize(quint32);
    void OscEraseRemaining(quint32);

public slots:
    void StopSThread();
    void GetSomeData(QByteArray ba);
    void Timeout();

private slots:
    void DataWritten(QByteArray);
    void CanalReady();
    void CanalError(int);
    void OscTimerTimeout();
    void SetStarted();
    void ClearStarted();

private:
    char *outdata;
    QByteArray *ReadData;
    QByteArray *WriteData;
    QTimer *TTimer, *OscTimer;
    quint16 OscNum;
    int bStep;
    int cmd;
    QLabel *lbl;
    quint32 fnum;
    quint32 RDLength; // длина всей посылки
    quint32 DLength; // длина данных
    quint32 WRLength; // длина всей посылки
    quint32 outdatasize; // размер приёмной области памяти
    quint32 SegLeft; // количество оставшихся сегментов
    quint32 SegEnd; // номер последнего байта в ReadData текущего сегмента
    publicclass::DataRec *DR; // ссылка на структуру DataRec, по которой собирать/восстанавливать S2
    bool LongBlock, ConnectedToPort, PortErrorDetected;
    SerialThread *SThread;
    bool SThreadStarted;

    void InitiateSend();
    void Finish(int ernum);
    void SetRDLength(int startpos);
    void SetWR(QByteArray, quint32 startpos);
    bool RDCheckForNextSegment();
    void WRCheckForNextSegment();
    void SendOk();
    void SendErr();
};

extern canal *cn;

#endif // CANAL_H
