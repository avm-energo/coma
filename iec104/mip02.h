#ifndef MIP02_H
#define MIP02_H

#include <QObject>
#include <QTcpSocket>

class mip02 : public QObject
{
    Q_OBJECT

public:
    mip02(QObject *parent = 0);

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
