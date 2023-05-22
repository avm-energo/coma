#ifndef BASEPORT_H
#define BASEPORT_H

#include "baseinterface.h"

#include <QMutex>
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
    bool reconnect();
    // virtual void init();

signals:
    void dataReceived(QByteArray ba);
    void started();
    void finished();
    void error();
    void timeout();
    void stateChanged(Interface::State);

private:
    Interface::State m_state;
    QMutex m_stateGuard;

protected:
    QMutex m_dataGuard;
    UniquePointer<LogClass> m_log;
    UniquePointer<QTimer> m_timeoutTimer;

    void setState(Interface::State state);
    Interface::State getState();
    virtual QByteArray readData() = 0;
    // virtual void finish();

public slots:
    void poll(bool ok);
    virtual void poll() = 0;
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual bool writeData(const QByteArray &ba) = 0;
};

#endif // BASEPORT_H
