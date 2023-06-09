#ifndef BASEPORT_H
#define BASEPORT_H

#include "baseinterface.h"

#include <QMutex>
#include <QObject>
#include <QTimer>
#include <gen/logclass.h>
#include <gen/stdfunc.h>

constexpr qint64 RECONNECTINTERVAL = 2000;

class BasePort : public QObject
{
    Q_OBJECT
public:
    enum PortErrors
    {
        Timeout,
        ReadError,
        NoData
    };

    explicit BasePort(const QString &logFilename, QObject *parent = nullptr);
    bool reconnect();

signals:
    void dataReceived(QByteArray ba);
    void started();
    void finished();
    void error(BasePort::PortErrors error);
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

    void writeLog(const QByteArray &ba, Interface::Direction dir = Interface::NoDirection);
    void writeLog(const Error::Msg msg, Interface::Direction dir = Interface::NoDirection);

    virtual QByteArray read(bool *status = nullptr) = 0;
    virtual bool connect() = 0;
    virtual void disconnect() = 0;

public slots:
    void poll();
    void closeConnection();
    virtual bool writeData(const QByteArray &ba) = 0;
};

#endif // BASEPORT_H
