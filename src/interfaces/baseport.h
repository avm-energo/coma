#ifndef BASEPORT_H
#define BASEPORT_H

#include "baseinterface.h"

#include <QObject>
#include <gen/logclass.h>
#include <gen/stdfunc.h>

constexpr qint64 RECONNECTINTERVAL = 2000;

class BasePort : public QObject
{
    Q_OBJECT
public:
    // Temporary in comment
    //    enum PortErrors
    //    {
    //    };

    explicit BasePort(const QString &logFilename, QObject *parent = nullptr);

    UniquePointer<LogClass> Log;
    Interface::State m_state;

signals:
    void dataReceived(QByteArray ba);
    void started();
    void finished();
    void stateChanged(Interface::State);

protected:
    void setState(Interface::State state);

public slots:
    virtual void poll() = 0;
    virtual bool connect() = 0;
    virtual bool reconnect();
    virtual void disconnect() = 0;
    virtual bool writeData(const QByteArray &ba) = 0;
};

#endif // BASEPORT_H
