#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QTimer>

class SerialThread : public QObject
{
    Q_OBJECT

public:
    explicit SerialThread(QSerialPort *, QObject *parent = 0);
    QSerialPort *port;

    QByteArray data();
    void InitiateWriteDataToPort(QByteArray ba);
private:
    QByteArray ReadData;
    QTimer *TimeoutTimer;
    bool NothingReceived;

signals:
    void newdataarrived(QByteArray);
    void datawritten(QByteArray);
    void timeout();
    void receivecompleted();

public slots:
    void run();
    void WriteData(QByteArray);

private slots:
    void CheckForData();
    void Timeout();
};

#endif // SERIALTHREAD_H
