#ifndef CANAL_H
#define CANAL_H

#include <QObject>
#include <QByteArray>
#include "publicclass.h"

class canal : public QObject
{
    Q_OBJECT
public:
    explicit canal(QObject *parent = 0);
    ~canal();
    void Send(int command, void *ptr=NULL, quint32 ptrsize=0, int filenum=0, publicclass::DataRec *DRptr=NULL);
    int result;

signals:
    void DataReady();

public slots:
    void GetSomeData(QByteArray ba);
    void Timeout();

private slots:
    void DataWritten(QByteArray);

private:
    unsigned char *outdata;
    QByteArray *ReadData;
    QByteArray *WriteData;
    int bStep;
    int cmd;
    int fnum;
    quint32 RDLength; // длина всей посылки
    quint32 DLength; // длина данных
    quint32 WRLength; // длина всей посылки
    quint32 outdatasize; // размер приёмной области памяти
    quint32 SegLeft; // количество оставшихся сегментов
    quint32 SegEnd; // номер последнего байта в ReadData текущего сегмента
    publicclass::DataRec *DR; // ссылка на структуру DataRec, по которой собирать/восстанавливать S2
    bool LongBlock;

    void SetRDLength(int startpos);
    void SetWR(QByteArray *, quint32 startpos);
    bool RDCheckForNextSegment();
    void WRCheckForNextSegment();
    void NoErrorDetected();
    void Finish(int ernum);
    void SendOk();
    void SendErr();
};

#endif // CANAL_H
