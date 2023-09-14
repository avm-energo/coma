#ifndef BASEPORT_H
#define BASEPORT_H

#include <QMutex>
#include <QObject>
#include <QTimer>
#include <gen/logclass.h>
#include <gen/stdfunc.h>
#include <interfaces/baseinterface.h>

constexpr qint64 RECONNECTINTERVAL = 10000;

class BasePort : public QObject
{
    Q_OBJECT
public:
    enum class PortErrors
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
    void clearQueries();

private:
    Interface::State m_state;
    UniquePointer<LogClass> m_log;
    QMutex m_stateGuard;

protected:
    QMutex m_dataGuard;

    void setState(Interface::State state);
    Interface::State getState();

    void writeLog(const QByteArray &ba, Interface::Direction dir = Interface::NoDirection);
    void writeLog(const Error::Msg msg, Interface::Direction dir = Interface::NoDirection);

    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual QByteArray read(bool *status = nullptr) = 0;
    virtual bool write(const QByteArray &ba) = 0;

public slots:
    void poll();
    void writeDataSync(const QByteArray &ba);
    void closeConnection();
};

#endif // BASEPORT_H
