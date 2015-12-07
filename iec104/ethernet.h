#ifndef ETHERNET_H
#define ETHERNET_H

#include <QThread>
#include <QTcpSocket>
#include <QMutex>
#include <QByteArray>

#define PORT104     2404 // порт связи по протоколу МЭК 60870-5-104

class ethernet : public QThread
{
    Q_OBJECT

public:
    ethernet(QObject *parent = 0);
    quint16 ASDU;
    bool ClosePortAndFinishThread;

public slots:
    void stop();
    void InitiateWriteDataToPort(QByteArray);

signals:
    void error(int);
    void connected();
    void disconnected();
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

protected:
    void run();
};

#endif // ETHERNET_H
