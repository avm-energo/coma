#ifndef ETHERNET_H
#define ETHERNET_H

#include <QObject>
#include <QTcpSocket>
#include <QMutex>
#include <QByteArray>

#define PORT104     2404 // порт связи по протоколу МЭК 60870-5-104

class ethernet : public QObject
{
    Q_OBJECT

public:
    ethernet(QObject *parent = 0);
    quint16 ASDU;
    bool ClosePortAndFinishThread;

    void InitiateWriteDataToPort(QByteArray *);

public slots:
    void run();
    void stop();

signals:
    void error(int);
    void connected();
    void finished();
    void newdataarrived(QByteArray);

private slots:
    void CheckForData();
    void seterr(QAbstractSocket::SocketError);

private:
    QTcpSocket *sock;
    QMutex OutDataBufMtx;
    QByteArray *DataToSend;
    QByteArray OutDataBuf;
    void SendData();
};

#endif // ETHERNET_H
