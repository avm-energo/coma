#ifndef ETHERNET_H
#define ETHERNET_H

#include <QByteArray>
#include <QMutex>
#include <QTcpSocket>
#include <QWaitCondition>
#include <gen/logclass.h>
#include <interfaces/ports/baseport.h>
#include <interfaces/types/settingstypes.h>

#define PORT104 2404 // порт связи по протоколу МЭК 60870-5-104

class Ethernet final : public BasePort
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
    void reconnect() override;
    void disconnect() override;
    // bool writeData(const QByteArray &ba) override;
    // void poll();
    QByteArray read(bool *status = nullptr) override;
    virtual bool write(const QByteArray &ba) override;

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
