#ifndef ETHERNET_H
#define ETHERNET_H

#include "../gen/logclass.h"

#include <QByteArray>
#include <QMutex>
#include <QTcpSocket>
#include <QWaitCondition>
#define PORT104 2404 // порт связи по протоколу МЭК 60870-5-104

class Ethernet : public QObject
{
    Q_OBJECT

public:
    Ethernet(QObject *parent = nullptr);
    ~Ethernet();
    bool ClosePortAndFinishThread;
    QString IP;
    LogClass *Log;

signals:
    void error(QAbstractSocket::SocketError);
    void Connected();
    void Disconnected();
    void finished();
    void NewDataArrived(QByteArray);

public slots:
    void Run();
    void Stop();
    void InitiateWriteDataToPort(QByteArray);

private slots:
    void CheckForData();
    void seterr(QAbstractSocket::SocketError error);
    void EthSetConnected();
    void EthStateChanged(QAbstractSocket::SocketState state);

private:
    QTcpSocket *sock;
    QMutex OutDataBufMtx;
    QByteArray *DataToSend;
    QByteArray OutDataBuf;
    bool EthConnected;
    QWaitCondition _waiter;
    void SendData();

protected:
};

#endif // ETHERNET_H
