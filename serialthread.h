#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

class SerialThread : public QObject
{
    Q_OBJECT
public:
    explicit SerialThread(QSerialPort *, QObject *parent = 0);
    void run();
    QSerialPort *port;

signals:
    void newdataarrived(QByteArray);
    void datawritten(int);

public slots:
    void WriteData(QByteArray);

private slots:
    void CheckForData();
};

#endif // SERIALTHREAD_H
