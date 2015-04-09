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
    bool ClosePortAndFinishThread;

    QByteArray data();
    void InitiateWriteDataToPort(QByteArray *);
private:
    QByteArray *ReadData;
    QByteArray *DataToSend;
    QTimer *TimeoutTimer;
    bool NothingReceived;
    bool ThereIsDataToSend;

signals:
    void newdataarrived(QByteArray);
    void datawritten(QByteArray);
    void timeout();
    void receivecompleted();
    void finished();
    void error(int);

public slots:
    void run();
    void stop();
    void WriteData();
    void SetPort(QString str);
    void SetBaud(QString str);

private slots:
    void CheckForData();
    void Timeout();
};

#endif // SERIALTHREAD_H
