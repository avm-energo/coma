#ifndef MIP02_H
#define MIP02_H

#include <QObject>
#include <QTcpSocket>

#define PORT104     2404 // порт связи по протоколу МЭК 60870-5-104

class mip02 : public QObject
{
    Q_OBJECT

public:
    mip02(QObject *parent = 0);
    QString Host;
    quint16 ASDU;

public slots:
    void run();

signals:
    void PacketReadyToParse();

private slots:
    void Parse104();

private:
    QTcpSocket *sock;
};

#endif // MIP02_H
