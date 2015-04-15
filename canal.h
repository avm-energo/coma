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

private:
    unsigned char *outdata;
    QByteArray *ReadData;
    int bStep;
    int cmd;
    int fnum;
    int RDLength; // длина всей посылки
    int outdatasize; // размер приёмной области памяти
    int SegLeft; // количество оставшихся сегментов
    int SegEnd; // номер последнего байта в ReadData текущего сегмента

    void SetRDLength(int startpos);
    void NoErrorDetected();
    void ErrorDetected();
};

#endif // CANAL_H
