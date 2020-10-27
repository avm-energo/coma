#ifndef BASEINTERFACE_H
#define BASEINTERFACE_H

#include "../gen/datatypes.h"
#include "../gen/logclass.h"
#include "../gen/s2.h"

#include <QObject>

class BaseInterface : public QObject
{
    Q_OBJECT
public:
    struct IEC104Settings
    {
        quint16 baseadr;
        QString ip;
    };

    struct SerialPortSettings
    {
        quint32 Baud;
        QString Parity;
        QString Stop;
        quint8 Address;
        QString Port;
    };

    struct ConnectStruct
    {
        QString name;
        int usbDevicePosition;
        IEC104Settings iec104st; // IEC104 settings
        SerialPortSettings serialst;
    };
    bool m_working;
    LogClass *Log;

    explicit BaseInterface(QObject *parent = nullptr);

    virtual bool start(const ConnectStruct &) = 0;
    virtual void reqStartup(quint32 sigAdr = 0, quint32 sigCount = 0) = 0;
    virtual void reqBSI() = 0;
    virtual void reqFile(quint32) = 0;
    virtual void writeFile(quint32, const QByteArray &) = 0;
    virtual void writeConfigFile(S2ConfigType *);
    virtual void reqTime() = 0;
    virtual void writeTime(quint32) = 0;
    virtual void writeCommand(Queries::Commands, QList<DataTypes::SignalsStruct> &) = 0;

    void reqAlarms(quint32 sigAdr = 0, quint32 sigCount = 0);
    virtual void reqFloats(quint32 sigAdr = 0, quint32 sigCount = 0) = 0;
    //    virtual void reqBitStrings(quint32 sigAdr = 0, quint32 sigCount = 0) = 0;

    bool isWorking() { return m_working; }

signals:
    void reconnect();

private:
public slots:
    virtual void stop() = 0;
};

#endif // BASEINTERFACE_H
