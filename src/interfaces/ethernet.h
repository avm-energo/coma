#ifndef ETHERNET_H
#define ETHERNET_H

#include "baseport.h"
#include "settingstypes.h"

#include <QByteArray>
#include <QMutex>
#include <QTcpSocket>
#include <QWaitCondition>
#include <gen/logclass.h>

#define PORT104 2404 // порт связи по протоколу МЭК 60870-5-104

class Ethernet : public BasePort
{
    Q_OBJECT

public:
    Ethernet(QObject *parent = nullptr);
    ~Ethernet();
    bool ClosePortAndFinishThread;
    QString IP;
    LogClass *Log;

    bool init(IEC104Settings settings);

signals:
    void error(QAbstractSocket::SocketError);
    void Connected();
    void Disconnected();

public slots:
    bool reconnect() override;
    void disconnect() override;
    bool writeData(const QByteArray &ba) override;
    void poll() override;

private slots:
    void CheckForData();
    void EthSetConnected();
    void EthStateChanged(QAbstractSocket::SocketState state);

private:
    quint16 m_baseadr;
    QString m_ip;
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
