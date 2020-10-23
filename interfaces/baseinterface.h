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
    bool m_working;
    LogClass *Log;

    explicit BaseInterface(QObject *parent = nullptr);

    virtual bool start(const ConnectStruct &) = 0;
    virtual void reqStartup() = 0;
    virtual void reqFile(quint32) = 0;
    virtual void writeFile(quint32, const QByteArray &) = 0;
    virtual void writeConfigFile(S2ConfigType *s2config);
    virtual void reqTime() = 0;
    virtual void writeTime() = 0;
    virtual void writeCommand(Queries::Commands, QList<DataTypes::SignalsStruct> &) = 0;

    bool isWorking() { return m_working; }

signals:
    void reconnect();

private:
public slots:
    virtual void stop() = 0;
};

#endif // BASEINTERFACE_H
