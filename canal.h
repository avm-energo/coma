#ifndef CANAL_H
#define CANAL_H

#include <QObject>
#include <QByteArray>
#include <QThread>
#include <QTimer>
#include <QLabel>

#include "publicclass.h"
#include "serialthread.h"

#define CANALER(a)       ERMSG(publicclass::ER_CANAL,__LINE__,a)

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
    bool Busy;
    quint32 RDSize;
    void Connect();
    void Disconnect();

signals:
    void stopall();
    void DataReady();
    void portopened();
    void error(int);
    void incomingdatalength(quint32);
    void bytesreceived(quint32);
    void writedatatoport(QByteArray);
    void gotsomedata(QByteArray);
    void somedatawritten(QByteArray);

public slots:
    void GetSomeData(QByteArray ba);
    void Timeout();

private slots:
    void DataWritten(QByteArray);
    void CanalReady();
    void CanalError(int);

private:
    char *outdata;
    QByteArray *ReadData;
    QByteArray *WriteData;
    QTimer *TTimer;
    QThread *thread;
    int bStep;
    int cmd;
    QLabel *lbl;
    quint32 fnum;
    quint8 ReconTry;
    quint32 RDLength; // длина всей посылки
    quint32 DLength; // длина данных
    quint32 WRLength; // длина всей посылки
    quint32 outdatasize; // размер приёмной области памяти
    quint32 SegLeft; // количество оставшихся сегментов
    quint32 SegEnd; // номер последнего байта в ReadData текущего сегмента
    publicclass::DataRec *DR; // ссылка на структуру DataRec, по которой собирать/восстанавливать S2
    bool LongBlock, ConnectedToPort, PortErrorDetected, ReconModeEnabled, ThreadStarted;

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
