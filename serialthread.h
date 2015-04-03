#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QObject>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class SerialThread : public QObject
{
    Q_OBJECT

public:
    explicit SerialThread(QObject *parent = 0);
    QSerialPortInfo portinfo;
    QSerialPort *port;
    int baud;
    QByteArray OutDataBuf;

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
    void finished();
    void error(int);

public slots:
    void run();
    void WriteData();
    void SetPort(QString str);
    void SetBaud(QString str);

private slots:
    void CheckForData();
    void Timeout();
};

#endif // SERIALTHREAD_H
