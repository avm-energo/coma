#ifndef BASEPORT_H
#define BASEPORT_H

#include <QMutex>
#include <QObject>
#include <QTimer>
#include <atomic>
#include <gen/error.h>
#include <gen/logclass.h>
#include <gen/stdfunc.h>
#include <interfaces/types/common_types.h>

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
    void error(const BasePort::PortErrors error);
    void stateChanged(Interface::State);
    void clearQueries();

private:
    std::atomic<Interface::State> m_state;
    LogClass m_log;

protected:
    QMutex m_dataGuard;

    void setState(Interface::State state);
    Interface::State getState();

    void writeLog(const QByteArray &ba, Interface::Direction dir = Interface::NoDirection);
    void writeLog(const Error::Msg msg, Interface::Direction dir = Interface::NoDirection);

    virtual QByteArray read(bool *status = nullptr) = 0;
    virtual bool write(const QByteArray &ba) = 0;

public slots:
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    void poll();
    void writeDataSync(const QByteArray &ba);
    void closeConnection();
};

#endif // BASEPORT_H
