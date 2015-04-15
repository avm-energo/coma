#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QObject>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMutex>

class SerialThread : public QObject
{
    Q_OBJECT

public:
    explicit SerialThread(QObject *parent = 0);
    QSerialPortInfo portinfo;
    int baud;
    QByteArray OutDataBuf;
    bool ClosePortAndFinishThread;

    void InitiateWriteDataToPort(QByteArray *);

private:
    QSerialPort *port;
    QByteArray *DataToSend;
    QTimer *TimeoutTimer;
    quint16 RcvDataLength;
    QMutex OutDataBufMtx, ReadDataMtx;

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

private slots:
    void CheckForData();
    void Timeout();
};

#endif // SERIALTHREAD_H
