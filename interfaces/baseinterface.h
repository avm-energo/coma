#ifndef BASEINTERFACE_H
#define BASEINTERFACE_H

#include "../gen/logclass.h"
#include "iec104.h"
#include "serialport.h"
#include <QObject>

class BaseInterface : public QObject
{
    Q_OBJECT
public:
    struct ConnectStruct
    {
        QString name;
        int usbDevicePosition;
        IEC104::Settings iec104st; // IEC104 settings
        SerialPort::Settings serialst;
    };

    explicit BaseInterface(QObject *parent = nullptr);

    virtual bool start(const ConnectStruct &st) = 0;
    virtual void reqStartup() = 0;
    virtual void reqFile() = 0;
    virtual void writeFile() = 0;
    virtual void reqTime() = 0;
    virtual void writeTime() = 0;
    virtual void writeCommand(quint32 cmd) = 0;
    bool isWorking() { return m_working; }

signals:
    void reconnect();

private:
    bool m_working;
    LogClass *Log;

public slots:
    void stopThread();
};

#endif // BASEINTERFACE_H
