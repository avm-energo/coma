#ifndef ETHERNET_H
#define ETHERNET_H

#include <QObject>
#include <QTcpSocket>
#include <QMutex>
#include <QByteArray>
#include "iec104.h"

#define PORT104     2404 // порт связи по протоколу МЭК 60870-5-104

class Ethernet : public QObject
{
    Q_OBJECT

public:
    Ethernet(QObject *parent = nullptr);
    ~Ethernet();
    quint16 ASDU;
    bool ClosePortAndFinishThread;
//    quint8 sendStatus;
//    quint8 stateMessage;
//    quint8 mutexflag;
    QString IP;
//    quint8 disconflag, timerstart;
signals:
    void ethNoconnection();
    void error(int);
    void Connected();
    void Disconnected();
    void Finished();
    void NewDataArrived(QByteArray);

public slots:
    void Run();
    void Stop();
    void InitiateWriteDataToPort(QByteArray);


private slots:
    void CheckForData();
    void seterr(QAbstractSocket::SocketError);
//    void SetFlag();
    void EthSetConnected();

private:
    QTcpSocket *sock;
    QMutex OutDataBufMtx;
    QByteArray *DataToSend;
    QByteArray OutDataBuf;
    bool EthConnected;

    void SendData();

protected:
};

#endif // ETHERNET_H
