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

/// TODO: Вынести в настройки
constexpr qint64 RECONNECTINTERVAL = 10000;

class BasePort : public QObject
{
    Q_OBJECT
public:
    enum class PortErrors : quint16
    {
        Timeout,
        ReadError,
        WriteError
    };

    explicit BasePort(const QString &logFilename, QObject *parent = nullptr);
    bool reconnectCycle();

signals:
    void dataReceived(QByteArray ba);
    void started();
    void finished();
    void error(const BasePort::PortErrors error);
    void stateChanged(Interface::State);
    void clearQueries();

protected:
    std::atomic<Interface::State> m_state;
    std::atomic<bool> m_reconnectLoopFlag;
    LogClass m_log;
    QMutex m_dataGuard;

    void setState(const Interface::State state) noexcept;
    Interface::State getState() const noexcept;

    bool getReconnectLoopFlag() const noexcept;
    void setReconnectLoopFlag(const bool flag) noexcept;

    void writeLog(const QByteArray &ba, Interface::Direction dir = Interface::NoDirection);
    void writeLog(const Error::Msg msg, Interface::Direction dir = Interface::NoDirection);

    virtual QByteArray read(bool *status = nullptr) = 0;
    virtual bool write(const QByteArray &ba) = 0;

public slots:
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual void reconnect() = 0;
    void poll();
    void writeData(const QByteArray &ba);
    void closeConnection();
    void finishReconnect();
};

#endif // BASEPORT_H
