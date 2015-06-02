#ifndef CANAL_H
#define CANAL_H

#include <QObject>
#include <QByteArray>
#include <QThread>

#include "publicclass.h"

class canal : public QObject
{
    Q_OBJECT
public:
    explicit canal(QObject *parent = 0);
    ~canal();
    void Send(int command, void *ptr=NULL, quint32 ptrsize=0, int filenum=0, publicclass::DataRec *DRptr=NULL);
    int result;
    QSerialPortInfo info;
    int baud;
    int ernum;
    bool FirstRun;

    void Connect();
    void Disconnect();
signals:
    void stopall();
    void DataReady();
    void timeout();
    void portopened();
    void error(int);

public slots:
    void GetSomeData(QByteArray ba);
    void Timeout();

private slots:
    void DataWritten(QByteArray);
    void StartReconnect();
    void Reconnect();
    void TryOnceMore();
    void CanalReady();
    void CanalError(int);
    void KillSThread();
    void SetErNum(int);

private:
    unsigned char *outdata;
    QByteArray *ReadData;
    QByteArray *WriteData;
    QThread *thread;
    int bStep;
    int cmd;
    int fnum;
    bool nda_gsd, t_t; // признаки наличия связи сигналов newdataarrived|timeout с соотв. слотами GetSomeData|Timeout
    quint8 ReconTry;
    quint32 RDLength; // длина всей посылки
    quint32 DLength; // длина данных
    quint32 WRLength; // длина всей посылки
    quint32 outdatasize; // размер приёмной области памяти
    quint32 SegLeft; // количество оставшихся сегментов
    quint32 SegEnd; // номер последнего байта в ReadData текущего сегмента
    publicclass::DataRec *DR; // ссылка на структуру DataRec, по которой собирать/восстанавливать S2
    bool LongBlock;

    void Finish(int ernum);
    void SetRDLength(int startpos);
    void SetWR(QByteArray *, quint32 startpos);
    bool RDCheckForNextSegment();
    void WRCheckForNextSegment();
    void NoErrorDetected();
    void SendOk();
    void SendErr();
};

extern canal *cn;

#endif // CANAL_H
