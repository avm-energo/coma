#ifndef MIP02_H
#define MIP02_H

#include <QObject>
#include <QTcpSocket>
#include <QMutex>
#include <QByteArray>

#define PORT104     2404 // порт связи по протоколу МЭК 60870-5-104

class mip02 : public QObject
{
    Q_OBJECT

public:
    mip02(QObject *parent = 0);
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
    void PacketReadyToParse();

private slots:
    void newdataarrived(QByteArray);
    void Parse104();
    void seterr(QAbstractSocket::SocketError);

private:
    QTcpSocket *sock;
    QMutex OutDataBufMtx;
    QByteArray *DataToSend;
    QByteArray OutDataBuf;
    void SendData();
};

#endif // MIP02_H
