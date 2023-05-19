#ifndef BASEPORT_H
#define BASEPORT_H

#include "baseinterface.h"

#include <QObject>
#include <gen/logclass.h>
#include <gen/stdfunc.h>

constexpr int RECONNECTINTERVAL = 2000;

class BasePort : public QObject
{
    Q_OBJECT
public:
    enum PortErrors
    {

    };

    explicit BasePort(const QString &logFilename, QObject *parent = nullptr);

    UniquePointer<LogClass> Log;
    Interface::State m_state;

signals:
    void dataReceived(QByteArray ba);
    void finished();
    void started();
    void stateChanged(Interface::State);
    // void tryToReconnect();

public slots:
    virtual void poll() = 0;
    void setState(Interface::State state);
    virtual bool connect() = 0;
    virtual bool reconnect();
    virtual void disconnect() = 0;
    virtual bool writeData(const QByteArray &ba) = 0;
};

#endif // BASEPORT_H
