#ifndef EUSBCOM_H
#define EUSBCOM_H

#include <QByteArray>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "eabstractprotocomchannel.h"

class EUsbCom : public EAbstractProtocomChannel
{
    Q_OBJECT
public:
    explicit EUsbCom(QObject *parent = 0);
    ~EUsbCom();

    QSerialPortInfo info;
    int baud;

    bool Connect();
    QByteArray RawRead(int bytes);
    int RawWrite(QByteArray &ba);
    void RawClose();
    QStringList DevicesFound();

signals:

public slots:

private slots:
    void Error(QSerialPort::SerialPortError);

private:
    bool FirstPass;
    QSerialPort *port;

    bool InitializePort(QSerialPortInfo &pinfo, int baud);
    void ShowConnectDialog();
    bool SetPort(QSerialPortInfo &info);
};

#endif // EUSBCOM_H
